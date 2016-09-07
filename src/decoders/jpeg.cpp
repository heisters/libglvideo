#include "decoders/jpeg.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG

#include <stb_image.h>
#include <Ap4DataBuffer.h>

using namespace glvideo;

Frame::ref decoders::JPEG::decode( AP4_DataBuffer *sampleData )
{
    int w, h;
    int comp;

    unsigned char *data = stbi_load_from_memory( sampleData->GetData(),
                                                 sampleData->GetDataSize(),
                                                 &w, &h,
                                                 &comp,
                                                 STBI_rgb );


    Frame::Format fmt;
    fmt
            .width( w )
            .height( h )
            .internalFormat( comp == 3 ? GL_RGB : GL_RGBA )
            .format( comp == 3 ? GL_RGB : GL_RGBA );
    Frame::ref frame = Frame::create( data, w * h * comp, fmt );
    stbi_image_free( data );

    return frame;
}
