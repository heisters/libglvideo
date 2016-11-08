#include "Movie.h"
#include "Ap4.h"
#include <sstream>
#include <cmath>
#include "gl_includes.h"
#include "decoders/jpeg.h"
#include "decoders/hap.h"

using namespace std;
using namespace glvideo;


Movie::Movie( const Context::ref &context, const string &filename, const Options &options ) :
	m_context( context ),
	m_options( options ),
	m_cpuFrameBuffer( options.cpuBufferSize() ),
    m_pbos( options.gpuBufferSize(), 0 ),
    m_gpuFrameBuffer( options.gpuBufferSize() )
{
    AP4_Result result;
    AP4_ByteStream *input = NULL;


    // Create input stream

    result = AP4_FileByteStream::Create( filename.c_str(),
                                         AP4_FileByteStream::STREAM_MODE_READ,
                                         input );
    if ( AP4_FAILED( result )) {
        throw Error( "cannot open input file " + filename );
    }

    m_file = new AP4_File( *input, true );
    input->Release();


    // Read movie tracks, and metadata, find the video track

    auto item = m_file->GetMovie()->GetTracks().FirstItem();
    size_t index = 0;
    while ( item ) {
        auto track = item->GetData();
        m_trackIndexMap[index++] = track->GetId();

        if ( track->GetType() == AP4_Track::TYPE_VIDEO ) {

			if ( m_videoTrack != nullptr ) {
				throw Error( "found two video tracks in " + filename + ". I'm confused." );
			}
			m_videoTrack = track;

            m_width = (uint32_t) ((double)m_videoTrack->GetWidth() / double( 1 << 16 ) );
            m_height = (uint32_t) ((double)m_videoTrack->GetHeight() / double( 1 << 16 ) );
			m_numSamples = m_videoTrack->GetSampleCount();
            m_fps = decltype( m_fps )( (float) 1000 * m_numSamples / (float)m_videoTrack->GetDurationMs() );
            m_codec = getTrackCodec( m_videoTrack );
        }

        item = item->GetNext();
    }

    if ( m_videoTrack == nullptr ) {
        throw Error( "could not find video track in " + filename );
    }


    // Find and instantiate the decoder

    AP4_Sample sample;
    AP4_DataBuffer sampleData;

    if ( AP4_FAILED( m_videoTrack->ReadSample( 0, sample, sampleData ))) {
        throw Error( "could not read video track in " + filename );
    }


    // TODO: make decoder selection dynamic
    if ( decoders::JPEG::matches( m_codec )) {
        m_decoder = unique_ptr<Decoder>( new decoders::JPEG( m_width, m_height, &sampleData ));
    } else if ( decoders::Hap::matches( m_codec )) {
        m_decoder = unique_ptr<Decoder>( new decoders::Hap( m_width, m_height, &sampleData ));
    } else {
        throw UnsupportedCodecError( "unsupported codec: " + m_codec );
    }


    // Initialize GPU resources
    glGenBuffers( (GLsizei)m_pbos.size(), m_pbos.data() );

    prebuffer();
}

Movie::~Movie()
{
    if ( isPlaying() ) stop();
	if ( m_jobsPending ) waitForJobsToFinish();

    glDeleteBuffers( (GLsizei)m_pbos.size(), m_pbos.data() );
}

string Movie::getFormat() const
{
    if ( !m_file ) return "not loaded";

    AP4_FtypAtom *ftype = m_file->GetFileType();
    if ( ftype == NULL ) return "unknown";

    char four_cc[5];
    AP4_FormatFourChars( four_cc, ftype->GetMajorBrand());


    stringstream ss;
    ss << four_cc << ftype->GetMinorVersion();
    return ss.str();
}

size_t Movie::getNumTracks() const
{
    return m_file->GetMovie()->GetTracks().ItemCount();
}

seconds Movie::getDuration() const
{
    return (seconds) m_file->GetMovie()->GetDurationMs() / 1000.0;
}

seconds Movie::getRemainingTime() const
{
	return (double)( m_numSamples - m_currentSample ) / (double)m_numSamples * getDuration();
}

std::string Movie::getTrackCodec( size_t index ) const
{
    auto id = m_trackIndexMap.at( index );
    auto track = m_file->GetMovie()->GetTrack( id );
    return getTrackCodec( track );
}

std::string Movie::getTrackCodec( AP4_Track *track ) const
{
    string codec = "unknown";
    auto sd = track->GetSampleDescription( 0 );
    AP4_String c;
    if ( AP4_SUCCEEDED( sd->GetCodecString( c ))) {
        codec = c.GetChars();
    }

    return codec;
}

TrackDescription Movie::getTrackDescription( size_t index ) const
{
    auto id = m_trackIndexMap.at( index );
    return TrackDescription( m_file->GetMovie()->GetTrack( id )->GetType(), getTrackCodec( index ));
}

Movie & Movie::play()
{
	if ( m_isPlaying ) return *this;


	m_lastFrameQueuedAt = clock::now();
	m_isPlaying = true;
	queueRead();

	return *this;
}

Movie & Movie::stop()
{
    m_isPlaying = false;

	return *this;
}

Movie & Movie::pause()
{
	return *this;
}

void Movie::queueRead()
{
	m_jobsPending = true;
	m_context->queueJob( bind( &Movie::read, this ) );
}

void Movie::read()
{
    // Buffer frames into CPU memory

    bufferNextCPUSample();

    // Schedule next read job, or notify waiting thread that all jobs
    // have finished.

	if ( m_isPlaying ) {
		queueRead();
	}

	else {
		m_jobsPending = false;
		m_jobsPendingCV.notify_one();
	}
}

void Movie::waitForJobsToFinish()
{
	if ( m_jobsPending ) {
		unique_lock< mutex > lock( m_jobsMutex );
		m_jobsPendingCV.wait( lock, [&] { return ! m_jobsPending; } );
	}
}

void Movie::update()
{
    if ( ! m_currentFrame && m_cpuFrameBuffer.empty() ) bufferNextCPUSample();
    bufferNextGPUSample();

    const auto spf = decltype( m_fps )( decltype( m_fps )( 1.f ) / m_fps );
    const auto nextFrameTime = m_lastFrameQueuedAt + spf;

    auto now = clock::now();
    if ( ( now >= nextFrameTime || m_currentFrame == nullptr ) && ! m_gpuFrameBuffer.empty() ) {
        Frame::ref frame;
        if ( m_gpuFrameBuffer.try_pop( &frame ) ) {
            if ( ! m_currentFrame ) frame->waitForBuffer();

            frame->createTexture();
            m_currentFrame = frame->getTexture();
            m_currentSample = frame->getSample();
            m_lastFrameQueuedAt = now;
        }
    }
}

void Movie::bufferNextCPUSample()
{
    if ( ! m_cpuFrameBuffer.is_full() && m_readSample < m_numSamples ) {

        auto frame = getFrame( m_videoTrack, m_readSample );
        if ( frame ) {
            m_cpuFrameBuffer.push( frame );

            m_readSample++;
            if ( m_loop ) m_readSample = m_readSample % m_numSamples;
        }
    }
}

void Movie::bufferNextGPUSample()
{
    if ( ! m_gpuFrameBuffer.is_full() ) {
        Frame::ref frame;
        if ( m_cpuFrameBuffer.try_pop( &frame ) ) {
            frame->bufferTexture( m_pbos[ m_currentPBO ] );
            m_gpuFrameBuffer.push( frame );
            m_currentPBO = ( m_currentPBO + 1 ) % m_pbos.size();
        }
    }
}

void Movie::prebuffer()
{
    m_context->queueJob( bind( &Movie::doPrebufferWork, this ) );
}

void Movie::doPrebufferWork()
{
    while ( ! m_cpuFrameBuffer.is_full() ) {
        bufferNextCPUSample();
    }

    while ( ! m_gpuFrameBuffer.is_full() ) {
        bufferNextGPUSample();
    }
}

FrameTexture::ref Movie::getCurrentFrame() const
{
    return m_currentFrame;
}


Frame::ref Movie::getFrame( AP4_Track *track, size_t i_sample ) const
{
    AP4_Sample sample;
    AP4_DataBuffer sampleData;


    if ( AP4_FAILED( track->ReadSample( i_sample, sample, sampleData ))) {
		return nullptr;
    }


    Frame::ref frame = m_decoder->decode( &sampleData );
    if ( frame ) frame->setSample( i_sample );

	return frame;
}



Movie & Movie::seekToStart()
{
    return seekToSample( 0 );
}

Movie & Movie::seek( seconds time )
{
    auto d = getDuration();
    return seekToSample( fmod( time, d ) / d * m_numSamples );
}

Movie & Movie::seekToSample( size_t sample )
{
    m_readSample = sample;

    m_cpuFrameBuffer.clear();
    m_currentFrame = nullptr;

    update();

    return *this;
}