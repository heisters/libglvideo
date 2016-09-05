#pragma once

#include <string>
#include <Ap4DataBuffer.h>
#include "Frame.h"

namespace glvideo {

class Decoder {
public:

    static bool matches( const std::string &codec ) { return false; }

    Decoder( int width, int height ) : m_width( width ), m_height( height ) {}

    virtual Frame::ref decode( AP4_DataBuffer &sampleData ) = 0;

protected:
    int m_width, m_height;
};

}