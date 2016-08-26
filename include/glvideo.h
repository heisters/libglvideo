#pragma once

#include <string>
#include <map>
#include <iostream>
#include <memory>
#include "gl_includes.h"

class AP4_File;

namespace glvideo {

typedef double seconds;

class TrackDescription {
public:
    TrackDescription( int specifier, const std::string &codec );

    friend std::ostream &operator<<( std::ostream &os, const TrackDescription &d )
    {
        return os << d.m_type << "; " << d.m_codec;
    }

private:
    int m_specifier;
    std::string m_type;
    std::string m_codec;
};

class Error : public std::runtime_error {
public:
    Error( const std::string &what ) : std::runtime_error( what ) {}
};

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

class Movie {
public:
    Movie( const std::string &filename );

    std::string getFormat() const;

    size_t getNumTracks() const;

    TrackDescription getTrackDescription( size_t index ) const;

    seconds getDuration() const;

    Frame::ref getFrame( size_t index ) const;

private:
    AP4_File *m_file = NULL;
    std::map<size_t, uint32_t> m_trackIndexMap;
};
}