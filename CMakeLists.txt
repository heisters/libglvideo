cmake_minimum_required(VERSION 3.6)

project(glvideo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if(MSVC)
	string(REPLACE "/MD " "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	string(REPLACE "/MD " "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
	string(REPLACE "/MDd " "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

# AP4

include_directories(
        "${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/Core"
        "${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/MetaData"
        "${PROJECT_SOURCE_DIR}/lib/Bento4/Source/C++/Codecs"
)
if(WIN32)
	add_definitions(-DAP4_PLATFORM_BYTE_ORDER=AP4_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
endif()
add_subdirectory("lib/Bento4" EXCLUDE_FROM_ALL)
set(EXTRA_LIBS ${EXTRA_LIBS} ap4)


# STB

include_directories("${PROJECT_SOURCE_DIR}/lib/stb")


# Hap

include_directories("${PROJECT_SOURCE_DIR}/lib/hap/source")

## ... which requires snappy

include(CheckIncludeFile)

CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILE(stddef.h HAVE_STDDEF_H)
CHECK_INCLUDE_FILE(sys/uio.h HAVE_SYS_UIO_H)

if(HAVE_STDINT_H)
    set(ac_cv_have_stdint_h 1)
else()
    set(ac_cv_have_stdint_h 0)
endif()

if(HAVE_STDDEF_H)
    set(ac_cv_have_stddef_h 1)
else()
    set(ac_cv_have_stddef_h 0)
endif()

if(HAVE_SYS_UIO_H)
	set(ac_cv_have_sys_uio_h 1)
else()
	set(ac_cv_have_sys_uio_h 0)
endif()

if(MSVC)
    add_definitions(-DHAVE_WINDOWS_H)
endif()

configure_file(
	${PROJECT_SOURCE_DIR}/lib/snappy/snappy-stubs-public.h.in 
    ${CMAKE_CURRENT_BINARY_DIR}/snappy-stubs-public.h
    @ONLY
)

include_directories(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/lib/snappy"
)


# OpenGL

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
set(EXTRA_LIBS ${EXTRA_LIBS} ${OPENGL_LIBRARIES})


# glvideo

include_directories(include)
set(SOURCE_FILES
    lib/snappy/snappy-c.cc lib/snappy/snappy-c.h
    lib/snappy/snappy-sinksource.cc lib/snappy/snappy-sinksource.h
    lib/snappy/snappy.cc
    lib/hap/source/hap.c lib/hap/source/hap.h
    include/glvideo.h
    src/gl_includes.cpp include/gl_includes.h
    include/concurrency.h
    src/FrameTexture.cpp include/FrameTexture.h
    src/Movie.cpp include/Movie.h
    src/TrackDescription.cpp include/TrackDescription.h
    src/Context.cpp include/Context.h
    src/Decoder.cpp include/Decoder.h
    src/decoders/jpeg.cpp include/decoders/jpeg.h
    src/decoders/hap.cpp include/decoders/hap.h
	src/Frame.cpp include/Frame.h
    )

add_library(glvideo STATIC ${SOURCE_FILES})
target_link_libraries(glvideo ${EXTRA_LIBS})

if(MSVC)
	#get_target_property(FLAGS glvideo COMPILE_FLAGS)
	#set_target_properties(glvideo PROPERTIES COMPILE_FLAGS "${FLAGS} /FI\"snappy-win.h\"") # add more headers with /FI\"${ForcedHeader_B}\"
	set_target_properties(glvideo PROPERTIES COMPILE_FLAGS "/FI\"snappy-win.h\"") # add more headers with /FI\"${ForcedHeader_B}\"
endif()

# examples

## pez

include_directories(
        "${PROJECT_SOURCE_DIR}/lib/pez/lib/libpez"
		"${PROJECT_SOURCE_DIR}/lib/pez/lib/glew"
)

add_subdirectory(lib/pez EXCLUDE_FROM_ALL)


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
	add_executable(play WIN32 examples/play.cpp)
else()
	add_executable(play examples/play.cpp)
endif()
target_link_libraries(play glvideo PezEcosystem ${PLATFORM_LIBS})

## stress test
if(MSVC)
	add_executable(stress WIN32 examples/stress.cpp)
else()
	add_executable(stress examples/stress.cpp)
endif()
target_link_libraries(stress glvideo PezEcosystem ${PLATFORM_LIBS})
