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
#elif defined( GLVIDEO_MSW )
	HGLRC prevContext = ::wglGetCurrentContext();
	HDC dc = ::wglGetCurrentDC();
	HGLRC newContext = ::wglCreateContext( dc );
	::wglMakeCurrent( NULL, NULL );
	if ( ! ::wglShareLists( prevContext, newContext ) ) {
		throw GLContextAllocationError();
	}
	::wglMakeCurrent( dc, prevContext );

	GLContextDataMsw::ref data( new GLContextDataMsw( dc, newContext ) );
	return ref( new GLContextTemplate<GLContextDataMsw>( data ) );
#else
#error Platform not supported.
#endif
}

template<typename Data>
void GLContextTemplate<Data>::makeCurrent()
{
#if defined( GLVIDEO_MAC )
    ::CGLSetCurrentContext( m_data->getContext());
#elif defined( GLVIDEO_MSW )
	::wglMakeCurrent( m_data->getDeviceContext(), m_data->getContext() );
#else
#error Platform not supported.
#endif
}


namespace glvideo {
#if defined( GLVIDEO_MAC )

template
class GLContextTemplate<GLContextDataMac>;

#elif defined( GLVIDEO_MSW )

template
class GLContextTemplate<GLContextDataMsw>;

#else
#error Platform not supported.
#endif
}