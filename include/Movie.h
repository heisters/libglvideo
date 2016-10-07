#pragma once

#include <string>
#include <map>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include "Frame.h"
#include "TrackDescription.h"
#include "Context.h"
#include "Decoder.h"
#include "concurrency.h"

class AP4_File;
class AP4_Track;

namespace glvideo {


/// Type alias to use for any representation of time.
typedef double seconds;

class UnsupportedCodecError : public std::runtime_error {
public:
    UnsupportedCodecError( const std::string &what ) : std::runtime_error( what ) {}
};

struct FrameAndMetdata {
	size_t sample;
	Frame::ref frame;
};

/// \class Movie
/// \brief Plays a movie from a file.
class Movie {
public:
    /// Options for a Movie
    class Options {
    public:
        Options() {}

        /// Set the number of \a frames to be buffered by the movie
        Options &bufferSize( size_t frames ) { m_bufferSize = frames; return *this; }
        size_t bufferSize() const { return m_bufferSize; }
        size_t &bufferSize() { return m_bufferSize; }

    private:
        size_t m_bufferSize = 5;
    };


    typedef std::shared_ptr<Movie> ref;
    typedef std::chrono::high_resolution_clock clock;

    /// Returns a ref to a movie constructed from a source \a filename.
    static ref
    create( const Context::ref &context, const std::string &filename, const Options &options = Options() )
    {
        return ref( new Movie( context, filename, options ));
    }

    /// Constructs a movie from a \a texContext, and a source \a filename.
    ///
    /// @param  texContext  a shared GL context that will be used to create textures on a separate thread.
    /// @param  filename    the filename to read from.
    ///
    /// \throws Error if the file cannot be read
    Movie( const Context::ref &context, const std::string &filename, const Options &options = Options() );

    ~Movie();

    /// Returns a string representation of the container format (eg. "qt 512").
    std::string getFormat() const;

    /// Returns a string representation of the primary video track's codec
    std::string getCodec() const { return m_codec; }

    /// Returns the number of tracks found in the container.
    size_t getNumTracks() const;

    /// Returns the TrackDescription for the track at \a index. Indexes start at 0, and are internally mapped to track ids.
    TrackDescription getTrackDescription( size_t index ) const;

    /// Returns the length of the movie in seconds.
    seconds getDuration() const;

	/// Returns the remaining amount of time in seconds.
	seconds getRemainingTime() const;

    /// Returns the framerate
    float getFramerate() const { return m_fps.count(); }

    /// Returns the width
    uint32_t getWidth() const { return m_width; }

    /// Returns the height
    uint32_t getHeight() const { return m_height; }

    /// Starts playing the movie.
    Movie & play();

    /// Is the movie playing?
    bool isPlaying() const { return m_isPlaying; }

    /// Stops playing the movie, terminating the read thread.
    Movie & stop();

    /// Pauses playing the movie, but leaves the read thread running. Same as setting the playback rate to 0.
    Movie & pause();

	/// Set whether the movie should automatically loop when it reaches the end (chainable).
	Movie & loop( bool loop = true ) { m_loop = loop; return *this; }

	/// Set the playhead to the beginning of the video.
	Movie & seekToStart() { m_readSample = 0; m_frameBuffer.clear(); return *this; }

    /// Returns the current Frame.
    Frame::ref getCurrentFrame();

private:
    std::string getTrackCodec( size_t index ) const;
    std::string getTrackCodec( AP4_Track * track ) const;


    AP4_File *m_file = NULL;
    std::map<size_t, uint32_t> m_trackIndexMap;
    Options m_options;
    Frame::ref m_currentFrame = nullptr;
    Context::ref m_context = nullptr;
    AP4_Track * m_videoTrack = nullptr;
    std::chrono::duration< float > m_fps;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    std::string m_codec;
	size_t m_numSamples = 0;
	size_t m_currentSample = 0;


    /// Extracts and decodes the sample with index \a i_sample from track with index \a i_track and returns a Frame.
    FrameAndMetdata getFrame( AP4_Track * track, size_t i_sample ) const;
    std::unique_ptr< Decoder > m_decoder;

    /// Reads frames into the frame buffer on a thread, and queues them.
	void queueRead();
	void read( GLContext::ref context );
	void waitForJobsToFinish();

	std::atomic_bool m_isPlaying{ false };
	std::atomic_bool m_jobsPending{ false };
	std::condition_variable m_jobsPendingCV;
	std::mutex m_jobsMutex;
	std::atomic_bool m_loop{ false };
	std::atomic< size_t > m_readSample{ 0 };
	concurrent_buffer< FrameAndMetdata > m_frameBuffer;
	clock::time_point m_lastFrameQueuedAt;
};
}