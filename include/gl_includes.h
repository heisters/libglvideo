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

typedef ptrdiff_t GLsizeiptr;
typedef void ( APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DPROC ) ( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data );
typedef void ( APIENTRY * PFNGLBINDBUFFERPROC ) ( GLenum target, GLuint buffer );
typedef void ( APIENTRY * PFNGLGENBUFFERSPROC ) ( GLsizei n, GLuint *buffers );
typedef void ( APIENTRY * PFNGLDELETEBUFFERSPROC ) ( GLsizei n, const GLuint *buffers );
typedef void ( APIENTRY * PFNGLBUFFERDATAPROC ) ( GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage );
typedef GLvoid* ( APIENTRY * PFNGLMAPBUFFERPROC ) ( GLenum target, GLenum access );
typedef GLboolean ( APIENTRY * PFNGLUNMAPBUFFERPROC ) ( GLenum target );

extern PFNGLCOMPRESSEDTEXIMAGE2DPROC   _glCompressedTexImage2D;
extern PFNGLBINDBUFFERPROC             _glBindBuffer;
extern PFNGLGENBUFFERSPROC             _glGenBuffers;
extern PFNGLDELETEBUFFERSPROC          _glDeleteBuffers;
extern PFNGLBUFFERDATAPROC             _glBufferData;
extern PFNGLMAPBUFFERPROC              _glMapBuffer;
extern PFNGLUNMAPBUFFERPROC            _glUnmapBuffer;

#define glCompressedTexImage2D _glCompressedTexImage2D
#define glBindBuffer _glBindBuffer
#define glGenBuffers _glGenBuffers
#define glDeleteBuffers _glDeleteBuffers
#define glBufferData _glBufferData
#define glMapBuffer _glMapBuffer
#define glUnmapBuffer _glUnmapBuffer

#define GL_TEXTURE_COMPRESSION_HINT 0x84EF
#define GL_PIXEL_UNPACK_BUFFER      0x88EC
#define GL_STATIC_DRAW              0x88E4
#define GL_WRITE_ONLY               0x88B9

#endif

void assignGlFunctionPointers();
