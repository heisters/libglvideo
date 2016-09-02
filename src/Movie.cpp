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
        m_frameBuffer( options.bufferSize())
{
    AP4_Result result;
    AP4_ByteStream *input = NULL;


    // create input stream
    result = AP4_FileByteStream::Create( filename.c_str(),
                                         AP4_FileByteStream::STREAM_MODE_READ,
                                         input );
    if ( AP4_FAILED( result )) {
        throw Error( "cannot open input file " + filename );
    }

    m_file = new AP4_File( *input, true );
    input->Release();


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

    if ( m_codec == "jpeg" ) {
        m_decoder = decodeJpegFrame;
    } else if ( m_codec == "HapY" ) {
        m_decoder = decodeHapFrame;
    } else {
        throw UnsupportedCodecError( "unsupported codec: " + m_codec );
    }
}

Movie::~Movie()
{
    if ( isPlaying()) stop();
    if ( m_readThread.joinable()) m_readThread.join();
    if ( m_queueThread.joinable()) m_queueThread.join();
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
    return TrackDescription( m_file->GetMovie()->GetTrack( id )->GetType(), getTrackCodec( index ) );
}

void Movie::play()
{
    m_isPlaying = true;
    m_readThread = thread( bind( &Movie::read, this, m_texContext ));
    m_queueThread = thread( bind( &Movie::queueFrames, this ));
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

    while ( m_isPlaying ) {
        if ( m_frameBuffer.is_full()) {
            //FIXME: just trying not to burn CPU too much. Should calculate this based on framerate.
            //this_thread::sleep_for( chrono::milliseconds( 10 ));

        } else {
            auto frame = getFrame( m_videoTrack, mt_sample++ );
            m_frameBuffer.push( frame );
        }
    }
}

// TODO: implement timing
void Movie::queueFrames()
{
    mt_lastFrameQueuedAt = clock::now();

    while ( m_isPlaying ) {

        if ( !m_nextFrameFresh ) {

            Frame::ref frame;
            if ( m_frameBuffer.try_pop( &frame )) {


                m_nextFrame = frame;
                m_nextFrameFresh = true;


                auto now = clock::now();

                auto spf = decltype( m_fps )( decltype( m_fps )( 1.f ) / m_fps );
                auto nextFrameTime = mt_lastFrameQueuedAt + spf;
                mt_lastFrameQueuedAt = now;

                //cout << chrono::duration_cast<chrono::milliseconds>((nextFrameTime - now)).count() << endl;
                if ( nextFrameTime > now ) {
                    this_thread::sleep_for( nextFrameTime - now );
                }
            }

        }

    }
}

Frame::ref Movie::getCurrentFrame()
{
    if ( m_nextFrameFresh ) {
        m_currentFrame = m_nextFrame;
        m_nextFrameFresh = false;
    }
    return m_currentFrame;
}



Frame::ref Movie::getFrame( AP4_Track * track, size_t i_sample ) const
{
    AP4_Sample sample;
    AP4_DataBuffer sampleData;


    if ( AP4_FAILED( track->ReadSample( i_sample, sample, sampleData ))) {
        return nullptr;
    }


    Frame::ref frame = m_decoder( sampleData );

    return frame;
}



