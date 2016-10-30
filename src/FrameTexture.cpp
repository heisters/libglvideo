#include <iostream>
#include "FrameTexture.h"
#include "gl_includes.h"

using namespace glvideo;


#define GL_TEXTURE_COMPRESSION_HINT 0x84EF

#if defined( GLVIDEO_MSW )

typedef void ( APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DPROC ) ( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data );

#endif


FrameTexture::FrameTexture( GLuint pbo, GLsizei imageSize, Format format ) :
        m_target( GL_TEXTURE_2D )
{
#if defined( GLVIDEO_MSW )
		PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)wglGetProcAddress( "glCompressedTexImage2D" );
#endif

    glEnable( m_target );
    glGenTextures( 1, &m_tex );
    glBindTexture( m_target, m_tex );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, pbo );
    if ( format.compressed() ) {
        glCompressedTexImage2D( m_target,
                                0,
                                format.internalFormat(),
                                format.width(),
                                format.height(),
                                0,
                                imageSize,
                                NULL );
        glHint( GL_TEXTURE_COMPRESSION_HINT, GL_NICEST );
    } else {
        glTexImage2D( m_target,
                      0,
                      format.internalFormat(),
                      format.width(),
                      format.height(),
                      0,
                      format.format(),
                      GL_UNSIGNED_BYTE,
                      NULL );
    }
    glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
    glBindTexture( m_target, 0 );
}

FrameTexture::~FrameTexture()
{
    if ( m_ownsTexture && m_tex ) glDeleteTextures( 1, &m_tex );
}
