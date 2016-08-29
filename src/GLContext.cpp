#include "GLContext.h"

using namespace std;
using namespace glvideo;

template<typename Data>
typename GLContextTemplate<Data>::ref GLContextTemplate<Data>::makeSharedFromCurrent()
{

#if defined( GLVIDEO_MAC )
    CGLContextObj prevContext = ::CGLGetCurrentContext();
    CGLPixelFormatObj sharedContextPixelFormat = ::CGLGetPixelFormat( prevContext );
    CGLContextObj newContext;
    if ( ::CGLCreateContext( sharedContextPixelFormat, prevContext, (CGLContextObj *) &newContext ) !=
         kCGLNoError ) {
        throw GLContextAllocationError();
    }

    GLContextDataMac::ref data( new GLContextDataMac( newContext ));
    return ref( new GLContextTemplate<GLContextDataMac>( data ));
#else
#error Platform not supported.
#endif
}

template<typename Data>
void GLContextTemplate<Data>::makeCurrent()
{
#if defined( GLVIDEO_MAC )
    ::CGLSetCurrentContext( m_data->getContext());
#else
#error Platform not supported.
#endif
}


namespace glvideo {
#if defined( GLVIDEO_MAC )

template
class GLContextTemplate<GLContextDataMac>;

#else
#error Platform not supported.
#endif
}