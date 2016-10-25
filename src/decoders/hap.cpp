#include <glew.h>
#include "decoders/hap.h"
#include "hap.h"
#include <iostream>
#include <Ap4DataBuffer.h>

using namespace glvideo;
using namespace std;


void HapMTDecode( HapDecodeWorkFunction function, void *p, unsigned int count, void *info )
{
    throw runtime_error( "I don't actually know how to decode a frame with multiple threads yet!" );
    int i;
    for ( i = 0; i < count; i++ ) {
        // Invoke your multithreading mechanism to cause this function to be called
        // on a suitable number of threads.
//        function( p, i );
    }
}


decoders::Hap::Hap( int w, int h, AP4_DataBuffer *sample0 ) :
        Decoder( w, h, sample0 )
{
    int size;
    unsigned int format;
    HapGetFrameTextureFormat( sample0->GetData(), sample0->GetDataSize(), 0, &format );

    int blockSize, channels;
    switch ( format ) {
        case HapTextureFormat_RGB_DXT1:
            blockSize = 8;
            channels = 3;
            break;
        case HapTextureFormat_RGBA_DXT5:
            blockSize = 4;
            channels = 4;
            break;
        case HapTextureFormat_YCoCg_DXT5:
            blockSize = 4;
            channels = 3;
            break;
        case HapTextureFormat_A_RGTC1:
            blockSize = 4;
            channels = 1;
            break;
        default:
            throw runtime_error( "unknown hap texture format" );
    }

    int roundedW = w + 4 - (w % 4);
    int roundedH = w + 4 - (h % 4);

    size = roundedW * roundedH * channels / blockSize;

    m_decompressedTextureBuffer.resize( size, 0 );
}


Frame::ref decoders::Hap::decode( AP4_DataBuffer *sampleData )
{
    unsigned int result;
    unsigned long decompressedSize;
    unsigned int numTextures;
    unsigned int texFormat;


    // Get the number of textures in the frame, as though we care

    result = HapGetFrameTextureCount( sampleData->GetData(), sampleData->GetDataSize(), &numTextures );
    if ( result != HapResult_No_Error ) {
        return nullptr;
    }

    if ( numTextures != 1 ) {
        // not supported yet
        throw runtime_error( "multiple textures per frame not supported yet!" );
    }



    // Decode the frame into compressed texture data

    result = HapDecode(
            sampleData->GetData(),
            sampleData->GetDataSize(),
            0, /* texture index */
            HapMTDecode, /* decode callback */
            NULL, /* info */
            m_decompressedTextureBuffer.data(),
            m_decompressedTextureBuffer.size(),
            &decompressedSize, /* bytes actually used in decompressedTexture */
            &texFormat /* output texture format */
    );


    if ( result == HapResult_Buffer_Too_Small ) {
        cerr << "WARNING, HAP: buffer too small, reallocating" << endl;
        m_decompressedTextureBuffer.resize( m_decompressedTextureBuffer.size() * 2 );
        return decode( sampleData );
    } else if ( result != HapResult_No_Error ) {
        return nullptr;
    }



    // Build the frame texture


    // texFormat already syncs with GL_COMPRESSED_*DXT*_EXT enums, with the exception of YCoCg,
    // for which we swizzle to one it understands.
    GLenum format = texFormat;
    if ( format == HapTextureFormat_YCoCg_DXT5 ) format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;


    FrameTexture::Format fmt;
    fmt
            .width( m_width )
            .height( m_height )
            .internalFormat( format )
            .compressed( true );
    return Frame::create( m_decompressedTextureBuffer.data(), decompressedSize, fmt );
}
