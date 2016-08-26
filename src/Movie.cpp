#include "Movie.h"
#include "Ap4.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG

#include <stb_image.h>
#include <sstream>

using namespace std;
using namespace glvideo;


Movie::Movie( const string &filename )
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

TrackDescription Movie::getTrackDescription( size_t index ) const
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

Frame::ref Movie::getFrame( size_t i_track, size_t i_sample ) const
{
    AP4_Sample sample;
    AP4_DataBuffer sampleData;
    int w, h;
    int comp;

    auto id = m_trackIndexMap.at( i_track );
    if ( AP4_FAILED( m_file->GetMovie()->GetTrack( id )->ReadSample( i_sample, sample, sampleData ))) {
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

