#pragma once

#include <string>
#include <map>

class AP4_File;

namespace glvideo {

typedef double seconds;

class TrackType {
public:
    TrackType( int specifier );

    friend std::ostream &operator<<( std::ostream &os, const TrackType &type )
    {
        return os << type.m_description;
    }

private:
    int m_specifier;
    std::string m_description;
};

class Error : public std::runtime_error {
public:
    Error( const std::string &what ) : std::runtime_error( what )
    {}
};

class Player {
public:
    Player( const std::string &filename );

    std::string getFormat() const;

    size_t getNumTracks() const;
    TrackType getTrackType( size_t index ) const;
    seconds getDuration() const;

private:
    AP4_File *m_file = NULL;
    std::map<size_t, uint32_t> m_trackIndexMap;
};
}