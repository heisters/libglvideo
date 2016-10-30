#pragma once

#include "platform.h"
#include "gl_includes.h"
#include <chrono>
#include <thread>
#if defined( GLVIDEO_MAC )
#include <iostream>
#elif defined( GLVIDEO_MSW )
#include <sstream>
#include <windows.h>
#endif

#if defined( GLVIDEO_MAC )
#define DBOUT_STR( s ) std::cout << s << std::endl;
#elif defined( GLVIDEO_MSW )
#define DBOUT_STR( s ) OutputDebugString( s.c_str() );
#endif

#define DBOUT_INFO( f, l, s ) {\
auto t = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();\
auto tid = std::this_thread::get_id();\
std::ostringstream os_;\
os_ << f << ":" << l << ",th" << tid << "@" << t << " "  << s << "\n";\
DBOUT_STR( os_.str() )\
}

#define DBOUT( s ) { DBOUT_INFO( __FILE__, __LINE__, s ); }


inline void checkGlError( char * f, int line )
{
    GLenum err( glGetError() );
    while ( err != GL_NO_ERROR ) {
        std::string error;

        switch ( err ) {
        case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
        case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
        case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
        case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
        }

        DBOUT_INFO( f, line, "GL_" << error.c_str() );
        err = glGetError();
    }
}

#define DBGL { checkGlError( __FILE__, __LINE__ ); }
