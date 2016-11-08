#pragma once

#include <memory>

typedef int GLsizei;
typedef unsigned int GLenum;
typedef unsigned int GLuint;

namespace glvideo {

/// \class  FrameTexture
/// \brief  Represents a single frame of a Movie on the GPU.
class FrameTexture {
public:
    typedef std::shared_ptr<FrameTexture> ref;


    class Format {
    public:
        Format() {}

        Format &width( GLsizei w ) { m_width = w; return *this; }
        GLsizei &width() { return m_width; }
        GLsizei width() const { return m_width; }


        Format &height( GLsizei h ) { m_height = h; return *this; }
        GLsizei &height() { return m_height; }
        GLsizei height() const { return m_height; }


        Format &internalFormat( GLenum f ) { m_internalFormat = f; return *this; }
        GLenum &internalFormat() { return m_internalFormat; }
        GLenum internalFormat() const { return m_internalFormat; }


        Format &format( GLenum f ) { m_format = f; return *this; }
        GLenum &format() { return m_format; }
        GLenum format() const { return m_format; }


        Format &compressed( bool c ) { m_compressed = c; return *this; }
        bool &compressed() { return m_compressed; }
        bool compressed() const { return m_compressed; }

    private:
        GLsizei m_width;
        GLsizei m_height;
        GLenum m_internalFormat;
        GLenum m_format;
        bool m_compressed = false;
    };


    /// Returns a ref to a new FrameTexture
    static ref create( GLuint pbo, GLsizei imageSize, Format format = Format())
    {
        return std::make_shared<FrameTexture>( pbo, imageSize, format );
    }

    /// Constructs a FrameTexture from data buffered in a PBO
    FrameTexture( GLuint pbo, GLsizei imageSize, Format format = Format() );

    /// Uses RAII to clean up GPU textures.
    ~FrameTexture();

    FrameTexture( FrameTexture const & ) = delete;
    FrameTexture &operator=( FrameTexture const & ) = delete;

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
    GLenum m_target;
    GLuint m_tex = 0;
	bool m_ownsTexture = true;
};


}