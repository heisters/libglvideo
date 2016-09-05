#pragma once

#include "Decoder.h"


namespace glvideo {
namespace decoders {

class JPEG : public Decoder {
public:
    static bool matches( const std::string &codec ) { return codec == "jpeg"; }

    JPEG( int w, int h ) : Decoder( w, h ) {}

    virtual Frame::ref decode( AP4_DataBuffer &sampleData );

private:
};

}
}