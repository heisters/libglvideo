cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(LIBGLVIDEO_DIR "${CMAKE_CURRENT_LIST_DIR}/../")


#if(MSVC)
#    string(REPLACE "/MD " "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#    string(REPLACE "/MD " "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
#    string(REPLACE "/MDd " "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
#endif()

# AP4

include_directories(
        "${LIBGLVIDEO_DIR}/lib/Bento4/Source/C++/Core"
        "${LIBGLVIDEO_DIR}/lib/Bento4/Source/C++/MetaData"
        "${LIBGLVIDEO_DIR}/lib/Bento4/Source/C++/Codecs"
)
if(WIN32)
    add_definitions(-DAP4_PLATFORM_BYTE_ORDER=AP4_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
endif()
#add_subdirectory("${LIBGLVIDEO_DIR}/lib/Bento4" EXCLUDE_FROM_ALL)
set(AP4_SOURCE_ROOT "${LIBGLVIDEO_DIR}/lib/Bento4/Source/C++")
set(AP4_SOURCE_CODECS ${AP4_SOURCE_ROOT}/Codecs)
set(AP4_SOURCE_CORE ${AP4_SOURCE_ROOT}/Core)
set(AP4_SOURCE_CRYPTO ${AP4_SOURCE_ROOT}/Crypto)
set(AP4_SOURCE_METADATA ${AP4_SOURCE_ROOT}/MetaData)
set(AP4_SOURCE_SYSTEM ${AP4_SOURCE_ROOT}/System)
file(GLOB AP4_SOURCES
        ${AP4_SOURCE_CODECS}/*.cpp
        ${AP4_SOURCE_CORE}/*.cpp
        ${AP4_SOURCE_CRYPTO}/*.cpp
        ${AP4_SOURCE_METADATA}/*.cpp
        ${AP4_SOURCE_SYSTEM}/StdC/*.cpp
        )

if(WIN32)
    set(AP4_SOURCES ${AP4_SOURCES} ${AP4_SOURCE_SYSTEM}/Win32/Ap4Win32Random.cpp)
else()
    set(AP4_SOURCES ${AP4_SOURCES} ${AP4_SOURCE_SYSTEM}/Posix/Ap4PosixRandom.cpp)
endif()

include_directories(
        ${AP4_SOURCE_CORE}
        ${AP4_SOURCE_CODECS}
        ${AP4_SOURCE_CRYPTO}
        ${AP4_SOURCE_METADATA}
)

add_library(ap4 STATIC ${AP4_SOURCES})
set(EXTRA_LIBS ${EXTRA_LIBS} ap4)


# STB

include_directories("${LIBGLVIDEO_DIR}/lib/stb")


# Hap

include_directories("${LIBGLVIDEO_DIR}/lib/hap/source")

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
        ${LIBGLVIDEO_DIR}/lib/snappy/snappy-stubs-public.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/snappy-stubs-public.h
        @ONLY
)

include_directories(
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${LIBGLVIDEO_DIR}/lib/snappy"
)


# OpenGL

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
set(EXTRA_LIBS ${EXTRA_LIBS} ${OPENGL_LIBRARIES})


# libglvideo

list( APPEND LIBGLVIDEO_INCLUDE_DIRS
        "${LIBGLVIDEO_DIR}/include"
        )

list( APPEND LIBGLVIDEO_SOURCE_FILES
        "${LIBGLVIDEO_DIR}/lib/snappy/snappy-c.cc"
        "${LIBGLVIDEO_DIR}/lib/snappy/snappy-c.h"
        "${LIBGLVIDEO_DIR}/lib/snappy/snappy-sinksource.cc"
        "${LIBGLVIDEO_DIR}/lib/snappy/snappy-sinksource.h"
        "${LIBGLVIDEO_DIR}/lib/snappy/snappy.cc"
        "${LIBGLVIDEO_DIR}/lib/hap/source/hap.c"
        "${LIBGLVIDEO_DIR}/lib/hap/source/hap.h"
        "${LIBGLVIDEO_DIR}/include/debug.h"
        "${LIBGLVIDEO_DIR}/include/glvideo.h"
        "${LIBGLVIDEO_DIR}/include/gl_includes.h"
        "${LIBGLVIDEO_DIR}/include/glad/glad.h"
        "${LIBGLVIDEO_DIR}/src/glad.c"
        "${LIBGLVIDEO_DIR}/include/concurrency.h"
        "${LIBGLVIDEO_DIR}/src/FrameTexture.cpp"
        "${LIBGLVIDEO_DIR}/include/FrameTexture.h"
        "${LIBGLVIDEO_DIR}/src/Movie.cpp"
        "${LIBGLVIDEO_DIR}/include/Movie.h"
        "${LIBGLVIDEO_DIR}/src/TrackDescription.cpp"
        "${LIBGLVIDEO_DIR}/include/TrackDescription.h"
        "${LIBGLVIDEO_DIR}/src/Context.cpp"
        "${LIBGLVIDEO_DIR}/include/Context.h"
        "${LIBGLVIDEO_DIR}/src/Decoder.cpp"
        "${LIBGLVIDEO_DIR}/include/Decoder.h"
        "${LIBGLVIDEO_DIR}/src/decoders/jpeg.cpp"
        "${LIBGLVIDEO_DIR}/include/decoders/jpeg.h"
        "${LIBGLVIDEO_DIR}/src/decoders/hap.cpp"
        "${LIBGLVIDEO_DIR}/include/decoders/hap.h"
        "${LIBGLVIDEO_DIR}/src/Frame.cpp"
        "${LIBGLVIDEO_DIR}/include/Frame.h"
        )

add_library(libglvideo STATIC ${LIBGLVIDEO_SOURCE_FILES})
target_link_libraries(libglvideo PRIVATE ${EXTRA_LIBS})
target_include_directories(libglvideo PUBLIC ${LIBGLVIDEO_INCLUDE_DIRS})

if(MSVC)
    #get_target_property(FLAGS libglvideo COMPILE_FLAGS)
    #set_target_properties(libglvideo PROPERTIES COMPILE_FLAGS "${FLAGS} /FI\"snappy-win.h\"") # add more headers with /FI\"${ForcedHeader_B}\"
    set_target_properties(libglvideo PROPERTIES COMPILE_FLAGS "/FI\"snappy-win.h\"") # add more headers with /FI\"${ForcedHeader_B}\"
endif()
