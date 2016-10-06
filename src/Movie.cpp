#include "Movie.h"
#include "Ap4.h"
#include <sstream>
#include <algorithm>
#include "decoders/jpeg.h"
#include "decoders/hap.h"


using namespace std;
using namespace glvideo;


Movie::Movie( const Context::ref &context, const string &filename, const Options &options ) :
        m_context( context ),
        m_options( options ),
        mt_frameBufferSize( options.bufferSize())
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
}

Movie::~Movie()
{
    if ( isPlaying() || m_jobsPending ) stop();
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


	mt_lastFrameQueuedAt = clock::now();
	m_isPlaying = true;
	queueRead();

	return *this;
}

Movie & Movie::stop()
{
    m_isPlaying = false;
	waitForJobsToFinish();

	mt_frameBuffer.clear();

	return *this;
}

Movie & Movie::pause()
{
	return *this;
}

void Movie::queueRead()
{
	m_jobsPending = true;
	m_context->queueJob( bind( &Movie::read, this, placeholders::_1 ) );
}

void Movie::read( GLContext::ref context )
{
	context->makeCurrent();

	const auto spf = decltype( m_fps )( decltype( m_fps )( 1.f ) / m_fps );
	const auto nextFrameTime = mt_lastFrameQueuedAt + spf;


	// decode

	if ( mt_frameBuffer.size() < mt_frameBufferSize && m_sample < m_numSamples ) {

		auto frame = getFrame( m_videoTrack, m_sample );
		if ( frame ) {
			mt_frameBuffer.push_back( frame );

			m_sample++;
			if ( m_loop ) m_sample = m_sample % m_numSamples;
		}
	}


	// queue

	if ( clock::now() >= nextFrameTime && ! mt_frameBuffer.empty() ) {

		m_currentFrame = mt_frameBuffer.front();
		mt_frameBuffer.pop_front();
		mt_lastFrameQueuedAt = clock::now();
	}


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

Frame::ref Movie::getCurrentFrame() const
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

    return frame;
}



