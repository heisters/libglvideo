#pragma once

#include "gl_includes.h"
#include <memory>
namespace glvideo {

class Frame {
public:
    typedef std::shared_ptr<Frame> ref;

    static ref create( unsigned char const *const data, GLsizei w, GLsizei h, GLenum format )
    {
        return std::make_shared<Frame>( data, w, h, format );
    }

    Frame( unsigned char const *const data, GLsizei w, GLsizei h, GLenum format );

    ~Frame();

    Frame( Frame const & ) = delete;

    Frame &operator=( Frame const & ) = delete;


    GLenum getTextureTarget() const { return m_target; }

    GLuint getTextureId() const { return m_tex; }

    operator bool() const { return m_tex != 0; }

private:
    GLenum m_target = GL_TEXTURE_2D;
    GLuint m_tex = 0;
};

}