#pragma once

#include <string>
#include <map>
#include "Frame.h"
#include "TrackDescription.h"

class AP4_File;

namespace glvideo {
typedef double seconds;

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