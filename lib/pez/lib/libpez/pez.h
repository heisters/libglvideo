// Pez was developed by Philip Rideout and released under the MIT License.

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

// Pez is a platform abstraction layer for OpenGL; it's an extra-tiny alternative to GLUT.
// To create a cross-platform app, simply link against the Pez source and implement these four functions.
//
const char* PezInitialize(int width, int height); // receive window size and return window title
void PezRender();                                 // draw scene (Pez swaps the backbuffer for you)
void PezUpdate(unsigned int milliseconds);        // receive elapsed time (e.g., update physics)
void PezHandleMouse(int x, int y, int action);    // handle mouse action: PEZ_DOWN, PEZ_UP, or PEZ_MOVE

// Here's the Pez configuration section.  Modify these constants to your liking!
//
#define PEZ_VIEWPORT_WIDTH 853
#define PEZ_VIEWPORT_HEIGHT 480
#define PEZ_ENABLE_MULTISAMPLING 1
#define PEZ_VERTICAL_SYNC 1

#if defined(__APPLE__)
#define PEZ_GL_VERSION_TOKEN "GL2"
#define PEZ_FORWARD_COMPATIBLE_GL 0
#else
#define PEZ_GL_VERSION_TOKEN "GL3"
#define PEZ_FORWARD_COMPATIBLE_GL 1
#endif

// Pez also defines a small handful of fixed constants and macros:
//
enum {PEZ_DOWN, PEZ_UP, PEZ_MOVE};
#define TwoPi (6.28318531f)
#define Pi (3.14159265f)
#define countof(A) (sizeof(A) / sizeof(A[0]))

// Additionally, Pez provides the following utility functions as alternatives to printf, exceptions, and asserts.
// On Windows, strings get sent to the debugger, so use the VS debugger window or dbgview to see them.
//
void PezDebugString(const char* pStr, ...);
void PezDebugStringW(const wchar_t* pStr, ...);
void PezFatalError(const char* pStr, ...);
void PezFatalErrorW(const wchar_t* pStr, ...);
#define PezCheckCondition(A, B) if (!(A)) { PezFatalError(B); }
#define PezCheckConditionW(A, B) if (!(A)) { PezFatalErrorW(B); }

#ifdef __cplusplus
}
#endif
