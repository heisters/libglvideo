#include "glvideo.h"
#include "Ap4.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG

#include <stb_image.h>
#include <sstream>

using namespace std;
using namespace glvideo;


Player::Player( const string &filename )
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
        m_trackIndexMap[index++] = item->GetData()->GetId();
        item = item->GetNext();
    }

}

string Player::getFormat() const
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

size_t Player::getNumTracks() const
{
    return m_file->GetMovie()->GetTracks().ItemCount();
}

seconds Player::getDuration() const
{
    return (seconds) m_file->GetMovie()->GetDurationMs() / 1000.0;
}

TrackDescription Player::getTrackDescription( size_t index ) const
{
    string codec = "unknown";
    auto id = m_trackIndexMap.at( index );
    auto sd = m_file->GetMovie()->GetTrack( id )->GetSampleDescription( 0 );
    AP4_String c;
    if ( AP4_SUCCEEDED( sd->GetCodecString( c ))) {
        codec = c.GetChars();
    }
    return TrackDescription( m_file->GetMovie()->GetTrack( id )->GetType(), codec );
}

Frame::ref Player::getFrame( size_t index ) const
{
    AP4_Sample sample;
    AP4_DataBuffer sampleData;
    int w, h;
    int comp;

    auto id = m_trackIndexMap.at( index );
    if ( AP4_FAILED( m_file->GetMovie()->GetTrack( id )->ReadSample( 0, sample, sampleData ))) {
        return nullptr;
    }


    unsigned char *data = stbi_load_from_memory( sampleData.GetData(),
                                                 sampleData.GetDataSize(),
                                                 &w, &h,
                                                 &comp,
                                                 STBI_rgb );


    Frame::ref frame = Frame::create( data, w, h, comp == 3 ? GL_RGB : GL_RGBA );
    stbi_image_free( data );

    return frame;
}

TrackDescription::TrackDescription( int specifier, const string &codec ) :
        m_specifier( specifier ),
        m_codec( codec )
{
    switch ((AP4_Track::Type) specifier ) {
        case AP4_Track::TYPE_UNKNOWN:
            m_type = "unknown";
            break;
        case AP4_Track::TYPE_AUDIO:
            m_type = "audio";
            break;
        case AP4_Track::TYPE_VIDEO:
            m_type = "video";
            break;
        case AP4_Track::TYPE_SYSTEM:
            m_type = "system";
            break;
        case AP4_Track::TYPE_HINT:
            m_type = "hint";
            break;
        case AP4_Track::TYPE_TEXT:
            m_type = "text";
            break;
        case AP4_Track::TYPE_JPEG:
            m_type = "jpeg";
            break;
        case AP4_Track::TYPE_RTP:
            m_type = "rtp";
            break;
        case AP4_Track::TYPE_SUBTITLES:
            m_type = "subtitles";
            break;
        default:
            m_type = "unspecified";
    }
}

Frame::Frame( unsigned char const *const data, GLsizei w, GLsizei h, GLenum format ) :
m_target( GL_TEXTURE_2D )
{
    if ( data != nullptr ) {
        glGenTextures( 1, &m_tex );
        glBindTexture( m_target, m_tex );
        glTexImage2D( m_target, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data );
        glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glBindTexture( m_target, 0 );
    }
}

Frame::~Frame()
{
    glDeleteTextures( 1, &m_tex );
}
