cmake_minimum_required(VERSION 3.6)
project(glvideo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


# AP4

set(INCLUDE_DIRS ${INCLUDE_DIRS}
        "${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/Core"
        "${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/MetaData"
        "${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/Codecs"
)
add_subdirectory("lib/Bento4" EXCLUDE_FROM_ALL)
set (EXTRA_LIBS ${EXTRA_LIBS} ap4)

# glvideo


set(INCLUDE_DIRS ${INCLUDE_DIRS} include)
set(SOURCE_FILES src/glvideo.cpp include/glvideo.h)

include_directories(${INCLUDE_DIRS})
add_library(glvideo ${SOURCE_FILES})
target_link_libraries(glvideo ${EXTRA_LIBS})


# examples

add_executable(play examples/play.cpp)
target_link_libraries(play glvideo)