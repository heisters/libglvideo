#pragma once

#include "gl_includes.h"
#include <memory>

namespace glvideo {


/// \class  Frame
/// \brief  Represents a single frame of a Movie, and manages transfer to the GPU.
class Frame {
public:
    typedef std::shared_ptr<Frame> ref;


    class Format {
    public:
        Format() {}

        Format &width( GLsizei w )
        {
            m_width = w;
            return *this;
        }

        GLsizei &width() { return m_width; }

        GLsizei width() const { return m_width; }


        Format &height( GLsizei h )
        {
            m_height = h;
            return *this;
        }

        GLsizei &height() { return m_height; }

        GLsizei height() const { return m_height; }


        Format &internalFormat( GLenum f )
        {
            m_internalFormat = f;
            return *this;
        }

        GLenum &internalFormat() { return m_internalFormat; }

        GLenum internalFormat() const { return m_internalFormat; }


        Format &format( GLenum f )
        {
            m_format = f;
            return *this;
        }

        GLenum &format() { return m_format; }

        GLenum format() const { return m_format; }


        Format &compressed( bool c )
        {
            m_compressed = c;
            return *this;
        }

        bool &compressed() { return m_compressed; }

        bool compressed() const { return m_compressed; }

    private:
        GLsizei m_width;
        GLsizei m_height;
        GLenum m_internalFormat;
        GLenum m_format;
        bool m_compressed = false;
    };


    /// Returns a ref to a new Frame
    static ref create( unsigned char const *const data, GLsizei imageSize, Format format = Format())
    {
        return std::make_shared<Frame>( data, imageSize, format );
    }

    /// Constructs a Frame from raw data, and uploads the data to the GPU.
    Frame( unsigned char const *const data, GLsizei imageSize, Format format = Format() );

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

	/// Set whether the frame should delete the texture when the frame is destroyed.
	/// Call this if you pass ownership off to another object.
	void setOwnsTexture( bool owns = true ) { m_ownsTexture = owns; }

private:
    GLenum m_target = GL_TEXTURE_2D;
    GLuint m_tex = 0;
	bool m_ownsTexture = true;
};

}