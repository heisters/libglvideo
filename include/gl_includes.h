#pragma once

#include "glvideo.h"

#include <glew.h>

#ifdef GLVIDEO_MAC
#include <OpenGL/gl.h>
#include <OpenGL/OpenGL.h>
#else
#ifdef GLVIDEO_MSW
  #include <windows.h>
#endif
#include <GL/gl.h>
#endif

