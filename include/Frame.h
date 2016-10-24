#pragma once

#include "FrameTexture.h"

namespace glvideo {

/// \class Frame
/// \brief Contains a single Movie frame, and manages upload to the GPU.
class Frame {
public:
    typedef std::shared_ptr< Frame > ref;

    static ref create( size_t sample, const FrameTexture::ref & ftex = nullptr  )
    {
        return std::make_shared< Frame >( sample, ftex );
    }

    Frame( size_t sample, const FrameTexture::ref & ftex = nullptr );


    bool hasTexture() const { return *m_ftex; }
    FrameTexture::ref getTexture() { return m_ftex; }
    size_t getSample() const { return m_sample; }
private:
    size_t m_sample = 0;
    FrameTexture::ref m_ftex = nullptr;
};

}