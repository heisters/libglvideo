#pragma once

#include <string>
#include <map>
#include "Frame.h"
#include "TrackDescription.h"

class AP4_File;

namespace glvideo {

/// Type alias to use for any representation of time.
typedef double seconds;

/// \class Movie
/// \brief Plays a movie from a file.
class Movie {
public:
    /// Constructs a movie from a source \a filename.
    /// \throws Error if the file cannot be read
    Movie( const std::string &filename );

    /// Returns a string representation of the container format (eg. "qt 512").
    std::string getFormat() const;

    /// Returns the number of tracks found in the container.
    size_t getNumTracks() const;

    /// Returns the TrackDescription for the track at \a index. Indexes start at 0, and are internally mapped to track ids.
    TrackDescription getTrackDescription( size_t index ) const;

    /// Returns the length of the movie in seconds.
    seconds getDuration() const;

private:
    /// Extracts and decodes the sample with index \a i_sample from track with index \a i_track and returns a Frame.
    Frame::ref getFrame( size_t i_track, size_t i_sample ) const;

    AP4_File *m_file = NULL;
    std::map<size_t, uint32_t> m_trackIndexMap;

};
}