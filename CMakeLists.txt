cmake_minimum_required(VERSION 3.6)
project(glvideo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


# AP4

include_directories(
        "${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/Core"
        "${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/MetaData"
        "${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/Codecs"
)
add_subdirectory("lib/Bento4" EXCLUDE_FROM_ALL)
set(EXTRA_LIBS ${EXTRA_LIBS} ap4)


# STB

include_directories("${PROJECT_SOURCE_DIR}/lib/stb")


# OpenGL

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
set(EXTRA_LIBS ${EXTRA_LIBS} ${OPENGL_LIBRARIES})


# glvideo

include_directories(include)
set(SOURCE_FILES
        include/glvideo.h
        include/gl_includes.h
        src/Frame.cpp
        include/Frame.h
        src/Movie.cpp
        include/Movie.h
        src/TrackDescription.cpp
        include/TrackDescription.h
        include/GLContext.h)

include_directories(${INCLUDE_DIRS})
add_library(glvideo STATIC ${SOURCE_FILES})
target_link_libraries(glvideo ${EXTRA_LIBS})


# examples

## pez

include_directories(
        "${PROJECT_SOURCE_DIR}/lib/pez/lib/libpez"
)

add_subdirectory(lib/pez EXCLUDE_FROM_ALL)


IF (WIN32)

    add_definitions(-DGLEW_STATIC)
    add_definitions(-DOPENCTM_STATIC)
    add_definitions(/wd4996)
    set(PLATFORM_LIBS opengl32)

ELSEIF (APPLE)

    # Create a nibless project per http://blog.hyperjeff.net/code?id=292

    find_library(COCOA_LIBRARY Cocoa)

    set(PLATFORM_LIBS
            ${COCOA_LIBRARY}
            /System/Library/Frameworks/OpenGL.framework)

ELSEIF (UNIX)

    set(PLATFORM_LIBS X11 GL)

ENDIF ()


## play

add_executable(play examples/play.cpp)
target_link_libraries(play glvideo PezEcosystem ${PLATFORM_LIBS})