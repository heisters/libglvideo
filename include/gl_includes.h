#pragma once

#include "glvideo.h"

#ifdef GLVIDEO_MAC
#include <OpenGL/gl.h>
#else
#ifdef GLVIDEO_WIN
  #include <windows.h>
#endif
#include <GL/gl.h>
#endif

