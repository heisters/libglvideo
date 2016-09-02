#include "decoders/jpeg.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG

#include <stb_image.h>

using namespace glvideo;
using namespace std;

Frame::ref decodeJpegFrame( AP4_DataBuffer &sampleData )
{
    int w, h;
    int comp;

    unsigned char *data = stbi_load_from_memory( sampleData.GetData(),
                                                 sampleData.GetDataSize(),
                                                 &w, &h,
                                                 &comp,
                                                 STBI_rgb );


    Frame::ref frame = Frame::create( data, w, h, comp == 3 ? GL_RGB : GL_RGBA );
    stbi_image_free( data );

    return frame;
}
