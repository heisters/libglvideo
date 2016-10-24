#pragma once

#include "platform.h"
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

#define DBOUT( s ) { auto l = __LINE__;\
auto t = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();\
auto tid = std::this_thread::get_id();\
std::ostringstream os_;\
os_ << __FILE__ << ":" << l << ",th" << tid << "@" << t << " "  << s << "\n";\
DBOUT_STR( os_.str() )\
}

