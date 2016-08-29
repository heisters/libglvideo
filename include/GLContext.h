#pragma once

#include "glvideo.h"
#include <memory>

#if defined( GLVIDEO_MAC )

#include <OpenGL/OpenGL.h>

#elif defined( GLVIDEO_MSW )
#include <windows.h>
#endif

namespace glvideo {
class GLContextAllocationError : public std::exception {
};


#if defined( GLVIDEO_MAC )

class GLContextDataMac {
public:
    typedef std::shared_ptr< GLContextDataMac > ref;
    GLContextDataMac( const CGLContextObj &ctx ) : m_context( ctx ) {};

    CGLContextObj &getContext() { return m_context; }

private:
    CGLContextObj m_context;
};

#else
#error Platform not supported.
#endif

template<typename Data>
class GLContextTemplate {
public:
    typedef typename std::shared_ptr<GLContextTemplate<Data>> ref;

    static ref makeSharedFromCurrent()
    {

#if defined( GLVIDEO_MAC )
        CGLContextObj prevContext = ::CGLGetCurrentContext();
        CGLPixelFormatObj sharedContextPixelFormat = ::CGLGetPixelFormat( prevContext );
        CGLContextObj newContext;
        if ( ::CGLCreateContext( sharedContextPixelFormat, prevContext, (CGLContextObj *) &newContext ) !=
             kCGLNoError ) {
            throw GLContextAllocationError();
        }

        GLContextDataMac::ref data( new GLContextDataMac( newContext ) );
        return ref( new GLContextTemplate<GLContextDataMac>( data ));
#else
#error Platform not supported.
#endif
    }

    GLContextTemplate() = delete;

private:
    GLContextTemplate( const typename Data::ref &data ) : m_data( data ) {};

public:

    void makeCurrent()
    {
#if defined( GLVIDEO_MAC )
        ::CGLSetCurrentContext( m_data->getContext());
#else
#error Platform not supported.
#endif
    }


private:

    typename Data::ref m_data;
};


#if defined( GLVIDEO_MAC )
typedef GLContextTemplate<GLContextDataMac> GLContext;
#else
#error Platform not supported.
#endif
}