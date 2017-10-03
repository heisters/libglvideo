CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

PROJECT( PezPackage )

FILE( GLOB GLEW lib/glew/*.c lib/glew/*.h )
FILE( GLOB GLSW lib/glsw/*.c lib/glsw/*.h )
FILE( GLOB LZMA lib/liblzma/*.c lib/liblzma/*.h )
FILE( GLOB OPENCTM lib/openctm/*.c lib/openctm/*.h )
FILE( GLOB PNGLITE lib/pnglite/*.c lib/pnglite/*.h )
FILE( GLOB VECTORMATH lib/vectormath/cpp/*.h )
FILE( GLOB ZLIB lib/zlib/*.c lib/zlib/*.h )

IF( WIN32 )

    FILE( GLOB PEZ
        lib/libpez/pez.h
        lib/libpez/pez.windows.c
    )

    SOURCE_GROUP( "glew" FILES ${GLEW} )
    SOURCE_GROUP( "glsw" FILES ${GLSW} )
    SOURCE_GROUP( "lzma" FILES ${LZMA} )
    SOURCE_GROUP( "openctm" FILES ${OPENCTM} )
    SOURCE_GROUP( "pnglite" FILES ${PNGLITE} )
    SOURCE_GROUP( "vectormath" FILES ${VECTORMATH} )
    SOURCE_GROUP( "zlib" FILES ${ZLIB} )
    SOURCE_GROUP( "pez" FILES ${PEZ} )
    SOURCE_GROUP( "shaders" FILES ${SHADERS} )

    ADD_DEFINITIONS( -DGLEW_STATIC )
    ADD_DEFINITIONS( -DOPENCTM_STATIC )
    ADD_DEFINITIONS( /wd4996 )
    SET( PLATFORM_LIBS opengl32 )
    SET( CONSOLE_SYSTEM WIN32 )

ELSEIF( APPLE )

    # Create a nibless project per http://blog.hyperjeff.net/code?id=292
    
    FILE( GLOB PEZ
        lib/libpez/pez.h
        lib/libpez/pez.cocoa.m
    )

    FIND_LIBRARY( COCOA_LIBRARY Cocoa )
    
    SET( PLATFORM_LIBS
        ${COCOA_LIBRARY}
        /System/Library/Frameworks/OpenGL.framework )

ELSEIF( UNIX )

    FILE( GLOB PEZ
        lib/libpez/pez.h
        lib/libpez/pez.linux.c
    )

    SET( PLATFORM_LIBS X11 GL )

ENDIF()

INCLUDE_DIRECTORIES(
    lib/glew
    lib/glsw
    lib/liblzma
    lib/libpez
    lib/openctm
    lib/pnglite
    lib/zlib
)

ADD_LIBRARY( PezEcosystem
    ${GLEW}
    ${GLSW}
    ${LZMA}
    ${OPENCTM}
    ${PNGLITE}
    ${VECTORMATH}
    ${ZLIB}
    ${PEZ}
)

# ANSI C SAMPLES:

INCLUDE_DIRECTORIES( lib/vectormath/c )
ADD_EXECUTABLE( PezIntro1 ${CONSOLE_SYSTEM} demo/PezIntro1.Clear.c )
TARGET_LINK_LIBRARIES( PezIntro1 PezEcosystem ${PLATFORM_LIBS} )
ADD_EXECUTABLE( PezIntro2 ${CONSOLE_SYSTEM} demo/PezIntro2.Triangle.c demo/Simple.glsl )
TARGET_LINK_LIBRARIES( PezIntro2 PezEcosystem ${PLATFORM_LIBS} )
ADD_EXECUTABLE( PezIntro3 ${CONSOLE_SYSTEM} demo/PezIntro3.Texture.c demo/Simple.glsl demo/Gandhi.png )
TARGET_LINK_LIBRARIES( PezIntro3 PezEcosystem ${PLATFORM_LIBS} )

# C++ SAMPLES:

INCLUDE_DIRECTORIES( BEFORE lib/vectormath/cpp )
ADD_EXECUTABLE( PezIntro4 ${CONSOLE_SYSTEM} demo/PezIntro4.Mesh.cpp demo/PixelLighting.glsl demo/HeadlessGiant.ctm )
TARGET_LINK_LIBRARIES( PezIntro4 PezEcosystem ${PLATFORM_LIBS} )
