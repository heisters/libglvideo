#include "gl_includes.h"

#ifdef GLVIDEO_MSW

PFNGLCOMPRESSEDTEXIMAGE2DPROC   _glCompressedTexImage2D;
PFNGLBINDBUFFERPROC             _glBindBuffer;
PFNGLGENBUFFERSPROC             _glGenBuffers;
PFNGLDELETEBUFFERSPROC          _glDeleteBuffers;
PFNGLBUFFERDATAPROC             _glBufferData;
PFNGLMAPBUFFERPROC              _glMapBuffer;
PFNGLUNMAPBUFFERPROC            _glUnmapBuffer;
PFNGLDELETESYNCPROC             _glDeleteSync;
PFNGLFENCESYNCPROC              _glFenceSync;
PFNGLCLIENTWAITSYNCPROC         _glClientWaitSync;

void assignGlFunctionPointers()
{
    _glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)wglGetProcAddress( "glCompressedTexImage2D" );
    _glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress( "glBindBuffer" );
    _glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress( "glGenBuffers" );
    _glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress( "glDeleteBuffers" );
    _glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress( "glBufferData" );
    _glMapBuffer = (PFNGLMAPBUFFERPROC)wglGetProcAddress( "glMapBuffer" );
    _glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress( "glUnmapBuffer" );
    _glDeleteSync = (PFNGLDELETESYNCPROC)wglGetProcAddress( "glDeleteSync" );
    _glFenceSync = (PFNGLFENCESYNCPROC)wglGetProcAddress( "glFenceSync" );
    _glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)wglGetProcAddress( "glClientWaitSync" );
}

#else

void assignGlFunctionPointers(){}

#endif