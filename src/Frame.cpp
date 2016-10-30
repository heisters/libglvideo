#include "Frame.h"
#include <algorithm>
#include "gl_includes.h"

using namespace glvideo;
using namespace std;

Frame::Frame( unsigned char const *const data, GLsizei imageSize, FrameTexture::Format texFormat ) :
        m_texSize( imageSize ),
        m_texFormat( texFormat )
{
    m_texData = unique_ptr< unsigned char[] >( new unsigned char[ m_texSize + 1 ] );
    copy( data, data + m_texSize, m_texData.get() );
}

void Frame::createTexture( GLuint pbo )
{
    if ( m_ftex ) return;

    m_ftex = FrameTexture::create( pbo, m_texSize, m_texFormat );
}

bool Frame::bufferTexture( GLuint pbo )
{
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, pbo );
    glBufferDataARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_texSize, 0, GL_STREAM_DRAW_ARB );
    GLubyte* buffer = (GLubyte*)glMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );

    if ( buffer ) {
        copy( m_texData.get(), m_texData.get() + m_texSize, buffer );
        glUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB );
        return true;
    }

    return false;
}