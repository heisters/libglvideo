#pragma once

#include "FrameTexture.h"

namespace glvideo {

/// \class Frame
/// \brief Contains a single Movie frame, and manages upload to the GPU.
class Frame {
public:
    typedef std::shared_ptr< Frame > ref;

    static ref create( unsigned char const *const data, GLsizei imageSize, FrameTexture::Format texFormat = FrameTexture::Format() )
    {
        return std::make_shared< Frame >( data, imageSize, texFormat );
    }

    Frame( unsigned char const *const data, GLsizei imageSize, FrameTexture::Format texFormat = FrameTexture::Format() );


    bool hasTexture() const { return *m_ftex; }
    FrameTexture::ref getTexture() { return m_ftex; }


    bool bufferTexture( GLuint pbo );
    void createTexture( GLuint pbo );

    size_t getSample() const { return m_sample; }

    void setSample( size_t sample ) { m_sample = sample; }
private:
    size_t                              m_sample = 0;
    std::unique_ptr< unsigned char[] >  m_texData = nullptr;
    GLsizei                             m_texSize = 0;
    FrameTexture::Format                m_texFormat = FrameTexture::Format();
    FrameTexture::ref                   m_ftex = nullptr;
};

}