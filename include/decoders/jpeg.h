#pragma once

#include "Decoder.h"


namespace glvideo {
namespace decoders {

class JPEG : public Decoder {
public:
    static bool matches( const std::string &codec ) { return codec == "jpeg"; }

    JPEG( int w, int h, AP4_DataBuffer *sample0 ) : Decoder( w, h, sample0 ) {}

    virtual FrameTexture::ref decode( AP4_DataBuffer *sampleData ) override;

private:
};

}
}