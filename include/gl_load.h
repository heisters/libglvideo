#include "gl_includes.h"

#ifdef GLVIDEO_MSW

typedef void ( APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DPROC ) ( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data );
typedef void ( APIENTRY * PFNGLBINDBUFFERPROC ) ( GLenum target, GLuint buffer );
typedef void ( APIENTRY * PFNGLGENBUFFERSPROC ) ( GLsizei n, GLuint *buffers );
typedef void ( APIENTRY * PFNGLDELETEBUFFERSPROC ) ( GLsizei n, const GLuint *buffers );
typedef void ( APIENTRY * PFNGLBUFFERDATAPROC ) ( GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage );
typedef GLvoid* ( APIENTRY * PFNGLMAPBUFFERPROC ) ( GLenum target, GLenum access );
typedef GLboolean( APIENTRY * PFNGLUNMAPBUFFERPROC ) ( GLenum target );
typedef void ( APIENTRY * PFNGLDELETESYNCPROC ) ( GLsync sync );
typedef GLsync( APIENTRY * PFNGLFENCESYNCPROC ) ( GLenum condition, GLbitfield flags );
typedef GLenum( APIENTRY * PFNGLCLIENTWAITSYNCPROC ) ( GLsync sync, GLbitfield flags, GLuint64 timeout );

#define GL_TEXTURE_COMPRESSION_HINT         0x84EF
#define GL_PIXEL_UNPACK_BUFFER              0x88EC
#define GL_STATIC_DRAW                      0x88E4
#define GL_WRITE_ONLY                       0x88B9
#define GL_INVALID_FRAMEBUFFER_OPERATION    0x0506
#define GL_SYNC_GPU_COMMANDS_COMPLETE       0x9117
#define GL_SYNC_FLUSH_COMMANDS_BIT          0x00000001
#define GL_UNSIGNALED                       0x9118
#define GL_SIGNALED                         0x9119
#define GL_ALREADY_SIGNALED                 0x911A
#define GL_TIMEOUT_EXPIRED                  0x911B
#define GL_CONDITION_SATISFIED              0x911C
#define GL_WAIT_FAILED                      0x911D
#define GL_TEXTURE_RECTANGLE                0x84F5

extern PFNGLCOMPRESSEDTEXIMAGE2DPROC        _glCompressedTexImage2D;
extern PFNGLBINDBUFFERPROC                  _glBindBuffer;
extern PFNGLGENBUFFERSPROC                  _glGenBuffers;
extern PFNGLDELETEBUFFERSPROC               _glDeleteBuffers;
extern PFNGLBUFFERDATAPROC                  _glBufferData;
extern PFNGLMAPBUFFERPROC                   _glMapBuffer;
extern PFNGLUNMAPBUFFERPROC                 _glUnmapBuffer;
extern PFNGLDELETESYNCPROC                  _glDeleteSync;
extern PFNGLFENCESYNCPROC                   _glFenceSync;
extern PFNGLCLIENTWAITSYNCPROC              _glClientWaitSync;

#define glCompressedTexImage2D              _glCompressedTexImage2D
#define glBindBuffer                        _glBindBuffer
#define glGenBuffers                        _glGenBuffers
#define glDeleteBuffers                     _glDeleteBuffers
#define glBufferData                        _glBufferData
#define glMapBuffer                         _glMapBuffer
#define glUnmapBuffer                       _glUnmapBuffer
#define glDeleteSync                        _glDeleteSync;
#define glFenceSync                         _glFenceSync;
#define glClientWaitSync                    _glClientWaitSync;

#endif

void assignGlFunctionPointers();
