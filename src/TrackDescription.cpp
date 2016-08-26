#include <Ap4Track.h>
#include "TrackDescription.h"

using namespace glvideo;
using namespace std;

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
