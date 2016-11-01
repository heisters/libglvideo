#pragma once

#if defined( __APPLE__ )
#define GLVIDEO_MAC
#elif defined( _WIN32 )
#define GLVIDEO_MSW
#endif


#ifdef GLVIDEO_MAC
  #include <OpenGL/gl.h>
  #include <OpenGL/OpenGL.h>
#else
  #ifdef GLVIDEO_MSW
    #include <windows.h>
  #endif
  #include <GL/gl.h>
#endif

#ifdef GLVIDEO_MSW

#include "glcorearb.h"

extern PFNGLCOMPRESSEDTEXIMAGE2DPROC   _glCompressedTexImage2D;
extern PFNGLBINDBUFFERPROC             _glBindBuffer;
extern PFNGLGENBUFFERSPROC             _glGenBuffers;
extern PFNGLDELETEBUFFERSPROC          _glDeleteBuffers;
extern PFNGLBUFFERDATAPROC             _glBufferData;
extern PFNGLMAPBUFFERPROC              _glMapBuffer;
extern PFNGLUNMAPBUFFERPROC            _glUnmapBuffer;
extern PFNGLDELETESYNCPROC             _glDeleteSync;
extern PFNGLFENCESYNCPROC              _glFenceSync;
extern PFNGLCLIENTWAITSYNCPROC         _glClientWaitSync;

#define glCompressedTexImage2D  _glCompressedTexImage2D
#define glBindBuffer _glBindBuffer
#define glGenBuffers _glGenBuffers
#define glDeleteBuffers _glDeleteBuffers
#define glBufferData _glBufferData
#define glMapBuffer _glMapBuffer
#define glUnmapBuffer _glUnmapBuffer
#define glDeleteSync _glDeleteSync;
#define glFenceSync _glFenceSync;
#define glClientWaitSync _glClientWaitSync;

#endif

void assignGlFunctionPointers();
