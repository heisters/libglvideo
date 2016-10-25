#include "Frame.h"
#include <algorithm>

using namespace glvideo;
using namespace std;

Frame::Frame( unsigned char const *const data, GLsizei imageSize, FrameTexture::Format texFormat ) :
        m_texSize( imageSize ),
        m_texFormat( texFormat )
{
    m_texData = unique_ptr< unsigned char[] >( new unsigned char[ m_texSize + 1 ] );
    copy( data, data + imageSize, m_texData.get() );
}

void Frame::createTexture()
{
    if ( m_ftex ) return;

    m_ftex = FrameTexture::create( m_texData.get(), m_texSize, m_texFormat );
}