#pragma once

#include <string>
#include "FrameTexture.h"

class AP4_DataBuffer;

namespace glvideo {

class Decoder {
public:

    static bool matches( const std::string &codec ) { return false; }

    Decoder( int width, int height, AP4_DataBuffer *sample0 ) : m_width( width ), m_height( height ) {}

    virtual FrameTexture::ref decode( AP4_DataBuffer *sampleData ) = 0;

protected:
    int m_width, m_height;
};

}