#pragma once

#include <string>
#include <iostream>

namespace glvideo {

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

}