cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(glvideo-examples)

set(EXAMPLE_DIR "${PROJECT_SOURCE_DIR}")
set(LIBGLVIDEO_DIR "${EXAMPLE_DIR}/../")

find_package(libglvideo REQUIRED PATHS "${LIBGLVIDEO_DIR}/cmake" NO_DEFAULT_PATH)

include_directories("${LIBGLVIDEO_INCLUDE_DIRS}")
list( APPEND LIBS libglvideo )

## pez

include_directories(
        "${PROJECT_SOURCE_DIR}/lib/pez/lib/libpez"
        "${PROJECT_SOURCE_DIR}/lib/pez/lib/glew"
)

add_subdirectory(lib/pez EXCLUDE_FROM_ALL)


list( APPEND LIBS PezEcosystem )

IF (MSVC)
    add_definitions(-DGLEW_STATIC)
    add_definitions(-DOPENCTM_STATIC)
    add_definitions(/wd4996)
    set(PLATFORM_LIBS opengl32)
    set(CONSOLE_SYSTEM WIN32)
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

if(MSVC)
    add_executable(play WIN32 play.cpp)
else()
    add_executable(play play.cpp)
endif()
target_link_libraries(play ${PLATFORM_LIBS} ${LIBS})

## stress test
if(MSVC)
    add_executable(stress WIN32 stress.cpp)
else()
    add_executable(stress stress.cpp)
endif()
target_link_libraries(stress ${PLATFORM_LIBS} ${LIBS})
