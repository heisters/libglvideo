#include "Frame.h"

using namespace glvideo;

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
