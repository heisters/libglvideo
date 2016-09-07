#pragma once

#include "glvideo.h"
#include <memory>
#include "gl_includes.h"

namespace glvideo {
class GLContextAllocationError : public std::exception {
};


#if defined( GLVIDEO_MAC )

class GLContextDataMac {
public:
    typedef std::shared_ptr<GLContextDataMac> ref;

    GLContextDataMac( const CGLContextObj &ctx ) : m_context( ctx ) {};

    CGLContextObj &getContext() { return m_context; }

private:
    CGLContextObj m_context;
};

#elif defined( GLVIDEO_MSW )

class GLContextDataMsw {
public:
	typedef std::shared_ptr<GLContextDataMsw> ref;

	GLContextDataMsw( const HDC &dc, const HGLRC &ctx ) : m_context( ctx ), m_deviceContext( dc ) {};
	~GLContextDataMsw()
	{
		::wglDeleteContext( m_context );
	}

	HGLRC & getContext() { return m_context; }
	HDC & getDeviceContext() { return m_deviceContext; }

private:
	HGLRC m_context;
	HDC m_deviceContext;
};

#else
#error Platform not supported.
#endif

template<typename Data>
class GLContextTemplate {
public:
    typedef typename std::shared_ptr<GLContextTemplate<Data>> ref;

    static ref makeSharedFromCurrent();

    GLContextTemplate() = delete;

private:
    GLContextTemplate( const typename Data::ref &data ) : m_data( data ) {};

public:

    void makeCurrent();


private:

    typename Data::ref m_data;
};

#if defined( GLVIDEO_MAC )

typedef GLContextTemplate<GLContextDataMac> GLContext;

#elif defined( GLVIDEO_MSW )

typedef GLContextTemplate<GLContextDataMsw> GLContext;

#else
#error Platform not supported.
#endif
}