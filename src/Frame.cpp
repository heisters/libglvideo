#include "Frame.h"
#include "gl_includes.h"
#include <algorithm>
#include <chrono>

using namespace glvideo;
using namespace std;

Frame::Frame( unsigned char const *const data, GLsizei imageSize, FrameTexture::Format texFormat ) :
        m_texSize( imageSize ),
        m_texFormat( texFormat )
{
    m_texData = make_unique< unsigned char[] >( m_texSize );
    copy( data, data + m_texSize, m_texData.get() );
}

Frame::~Frame()
{
    if ( m_sync ) glDeleteSync( m_sync );
}

void Frame::createTexture()
{
    if ( m_ftex ) return;

    m_ftex = FrameTexture::create( m_pbo, m_texSize, m_texFormat );
}

bool Frame::bufferTexture( GLuint pbo )
{
    glBindBuffer( GL_PIXEL_UNPACK_BUFFER, pbo );
    // Call glBufferDataARB to cancel any work the GPU is currently doing with
    // the PBO, to avoid glMapBufferARB blocking in the case that there is
    // pending work.
    glBufferData( GL_PIXEL_UNPACK_BUFFER, m_texSize, NULL, GL_STATIC_DRAW );

    GLubyte* buffer = (GLubyte*)glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY );
    if ( ! buffer ) return false;


    copy( m_texData.get(), m_texData.get() + m_texSize, buffer );
    glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER );
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0); // Unbind
    m_pbo = pbo;

    if ( m_sync ) glDeleteSync( m_sync );

    m_sync = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0L );
    glFlush();

    return true;
}

bool Frame::isBuffered()
{
    return waitForBuffer( 0ULL );
}

bool Frame::waitForBuffer( double timeoutSeconds )
{
    using namespace chrono;

    const GLuint64 timeout = duration_cast<nanoseconds>( duration< double >( timeoutSeconds ) ).count();

    return waitForBuffer( timeout );
}

bool Frame::waitForBuffer( GLuint64 timeoutNanoseconds )
{
    if ( m_pbo == 0 ) return false;
    if ( ! m_sync ) return false;

    GLenum status = glClientWaitSync( m_sync, GL_SYNC_FLUSH_COMMANDS_BIT, timeoutNanoseconds );
    switch ( status ) {
    case GL_CONDITION_SATISFIED:
    case GL_ALREADY_SIGNALED:
        return true;
        break;
    case GL_WAIT_FAILED:
    case GL_TIMEOUT_EXPIRED:
        return false;
        break;
    }

    return false;
}