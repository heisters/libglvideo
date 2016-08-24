cmake_minimum_required(VERSION 3.6)
project(Simple_Video)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


include_directories("${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/Core" "${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/MetaData" "${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/Codecs")
add_subdirectory("lib/Bento4" EXCLUDE_FROM_ALL)
set (EXTRA_LIBS ${EXTRA_LIBS} ap4)



set(SOURCE_FILES src/main.cpp)
add_executable(Simple_Video ${SOURCE_FILES})
target_link_libraries(Simple_Video ${EXTRA_LIBS})