#include "decoders/hap.h"
#include "hap.h"
#include <iostream>

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


decoders::Hap::Hap( int w, int h ) :
        Decoder( w, h )
{
    m_decompressedTextureBuffer.resize( w * h * 3, 0 );

    // HapGetFrameTextureFormat
    // Usually (ie always for mov or avi) dimensions are stored
    // independently in the container, so armed with those and
    // the texture format (from HapGetFrameTextureFormat()) you
    // can calculate it exactly (round dimensions up to a
    // multiple of four, apply known DXT block size).
}


Frame::ref decoders::Hap::decode( AP4_DataBuffer &sampleData )
{
    unsigned int result;
    unsigned long decompressedSize;
    unsigned int numTextures;
    unsigned int texFormat;


    // Get the number of textures in the frame, as though we care

    result = HapGetFrameTextureCount( sampleData.GetData(), sampleData.GetDataSize(), &numTextures );
    if ( result != HapResult_No_Error ) {
        return nullptr;
    }

    if ( numTextures != 1 ) {
        // not supported yet
        throw runtime_error( "multiple textures per frame not supported yet!" );
    }



    // Decode the frame into compressed texture data

    result = HapDecode(
            sampleData.GetData(),
            sampleData.GetDataSize(),
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


    Frame::Format fmt;
    fmt
            .width( m_width )
            .height( m_height )
            .internalFormat( format )
            .compressed( true );
    return Frame::create( m_decompressedTextureBuffer.data(), decompressedSize, fmt );
}
