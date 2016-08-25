#pragma once

#include <string>
#include <map>
#include <iostream>

class AP4_File;

namespace glvideo {

typedef double seconds;

class TrackDescription {
public:
    TrackDescription( int specifier, const std::string &codec );

    friend std::ostream &operator<<( std::ostream &os, const TrackDescription &d)
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
    Error( const std::string &what ) : std::runtime_error( what )
    {}
};

class Player {
public:
    Player( const std::string &filename );

    std::string getFormat() const;
    size_t getNumTracks() const;
    TrackDescription getTrackDescription( size_t index ) const;
    seconds getDuration() const;

private:
    AP4_File *m_file = NULL;
    std::map<size_t, uint32_t> m_trackIndexMap;
};
}