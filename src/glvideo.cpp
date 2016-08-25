#include "glvideo.h"
#include "Ap4.h"
#include "Ap4File.h"
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

TrackType Player::getTrackType( size_t index ) const
{
    auto id = m_trackIndexMap.at( index );
    return TrackType( m_file->GetMovie()->GetTrack( id )->GetType() );
}

TrackType::TrackType( int specifier ) :
        m_specifier( specifier )
{
    switch ((AP4_Track::Type) specifier ) {
        case AP4_Track::TYPE_UNKNOWN:
            m_description = "unknown";
            break;
        case AP4_Track::TYPE_AUDIO:
            m_description = "audio";
            break;
        case AP4_Track::TYPE_VIDEO:
            m_description = "video";
            break;
        case AP4_Track::TYPE_SYSTEM:
            m_description = "system";
            break;
        case AP4_Track::TYPE_HINT:
            m_description = "hint";
            break;
        case AP4_Track::TYPE_TEXT:
            m_description = "text";
            break;
        case AP4_Track::TYPE_JPEG:
            m_description = "jpeg";
            break;
        case AP4_Track::TYPE_RTP:
            m_description = "rtp";
            break;
        case AP4_Track::TYPE_SUBTITLES:
            m_description = "subtitles";
            break;
        default:
            m_description = "unspecified";
    }
}
