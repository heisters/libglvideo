#pragma once

#include <chrono>
#include <sstream>
#include <windows.h>

#define DBOUT( s ) { auto l = __LINE__;\
auto t = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();\
auto tid = std::this_thread::get_id();\
std::ostringstream os_;\
os_ << __FILE__ << ":" << l << ",th" << tid << "@" << t << " "  << s << "\n";\
OutputDebugString( os_.str().c_str() );\
}
