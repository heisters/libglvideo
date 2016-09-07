#include "Movie.h"
#include "Ap4.h"
#include <sstream>
#include <algorithm>
#include "decoders/jpeg.h"
#include "decoders/hap.h"


using namespace std;
using namespace glvideo;


Movie::Movie( const GLContext::ref &texContext, const string &filename, const Options &options ) :
        m_texContext( texContext ),
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
            m_width = max( m_width, (uint32_t) ((double) track->GetWidth() / double( 1 << 16 )));
            m_height = max( m_height, (uint32_t) ((double) track->GetHeight() / double( 1 << 16 )));
            m_fps = decltype( m_fps )(
                    max( m_fps.count(), (float) 1000 * track->GetSampleCount() / (float) track->GetDurationMs()));
            m_codec = getTrackCodec( track );
            m_videoTrack = track;
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
        m_decoder = unique_ptr<Decoder>( new decoders::JPEG( m_width, m_height, sampleData ));
    } else if ( decoders::Hap::matches( m_codec )) {
        m_decoder = unique_ptr<Decoder>( new decoders::Hap( m_width, m_height, sampleData ));
    } else {
        throw UnsupportedCodecError( "unsupported codec: " + m_codec );
    }
}

Movie::~Movie()
{
    if ( isPlaying()) stop();
    if ( m_readThread.joinable()) m_readThread.join();
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

void Movie::play()
{
    m_isPlaying = true;
    m_readThread = thread( bind( &Movie::read, this, m_texContext ));
}

void Movie::stop()
{
    m_isPlaying = false;
}

void Movie::pause()
{

}

void Movie::read( GLContext::ref context )
{
    context->makeCurrent();
    mt_lastFrameQueuedAt = clock::now();

    while ( m_isPlaying ) {
        const auto spf = decltype( m_fps )( decltype( m_fps )( 1.f ) / m_fps );
        const auto nextFrameTime = mt_lastFrameQueuedAt + spf;


        // decode

        if ( mt_frameBuffer.size() < mt_frameBufferSize ) {

            auto frame = getFrame( m_videoTrack, mt_sample++ );
            mt_frameBuffer.push_back( frame );
        }


        // queue

        if ( clock::now() >= nextFrameTime && ! mt_frameBuffer.empty() ) {

            m_currentFrame = mt_frameBuffer.front();
            mt_frameBuffer.pop_front();
            mt_lastFrameQueuedAt = clock::now();
        }
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


    Frame::ref frame = m_decoder->decode( sampleData );

    return frame;
}



