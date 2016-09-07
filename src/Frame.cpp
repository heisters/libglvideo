#include <iostream>
#include "Frame.h"
#include "gl_includes.h"

using namespace glvideo;

Frame::Frame( unsigned char const *const data, GLsizei imageSize, Format format ) :
        m_target( GL_TEXTURE_2D )
{
    if ( data != nullptr ) {
        glEnable( m_target );
        glGenTextures( 1, &m_tex );
		glBindTexture( m_target, m_tex );

        if ( format.compressed() ) {
            glCompressedTexImage2D( m_target, 0, format.internalFormat(), format.width(), format.height(), 0, imageSize, data );
            glHint( GL_TEXTURE_COMPRESSION_HINT, GL_NICEST );
        } else {
            glTexImage2D( m_target, 0, format.internalFormat(), format.width(), format.height(), 0, format.format(),
                          GL_UNSIGNED_BYTE, data );
        }
        glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        glBindTexture( m_target, 0 );
    }
}

Frame::~Frame()
{
    if ( m_tex ) glDeleteTextures( 1, &m_tex );
}
