#pragma once

#include "Decoder.h"

namespace glvideo {
namespace decoders {

class Hap : public Decoder {
public:
    static bool matches( const std::string &codec ) { return codec == "HapY"; }

    Hap( int w, int h ) : Decoder( w, h ) {}

    virtual Frame::ref decode( AP4_DataBuffer &sampleData );

private:
    Frame::ref decode( AP4_DataBuffer &sampleData, unsigned int outputBufferSize );
};

}
}