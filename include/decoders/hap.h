#pragma once

#include <vector>
#include "Decoder.h"

namespace glvideo {
namespace decoders {

class Hap : public Decoder {
public:
    static bool matches( const std::string &codec ) { return codec == "Hap1" || codec == "Hap5" || codec == "HapY"; }

    Hap( int w, int h, AP4_DataBuffer *sample0 );

    virtual Frame::ref decode( AP4_DataBuffer *sampleData );

private:
    std::vector< unsigned char > m_decompressedTextureBuffer;
};

}
}