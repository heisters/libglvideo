#pragma once

#include "gl_includes.h"
#include <memory>

namespace glvideo {


/// \class  Frame
/// \brief  Represents a single frame of a Movie, and manages transfer to the GPU.
class Frame {
public:
    typedef std::shared_ptr<Frame> ref;

    /// Returns a ref to a new Frame
    static ref create( unsigned char const *const data, GLsizei w, GLsizei h, GLenum format )
    {
        return std::make_shared<Frame>( data, w, h, format );
    }

    /// Constructs a Frame from raw data, and uploads the data to the GPU.
    Frame( unsigned char const *const data, GLsizei w, GLsizei h, GLenum format );

    /// Uses RAII to clean up GPU textures.
    ~Frame();

    Frame( Frame const & ) = delete;

    Frame &operator=( Frame const & ) = delete;

    /// Returns the texture target of the frame. Usually GL_TEXTURE_2D.
    GLenum getTextureTarget() const { return m_target; }

    /// Returns the texture id/handle of the allocated texture.
    GLuint getTextureId() const { return m_tex; }

    /// Returns false if the texture was not successfully allocated.
    operator bool() const { return m_tex != 0; }

private:
    GLenum m_target = GL_TEXTURE_2D;
    GLuint m_tex = 0;
};

}