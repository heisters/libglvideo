libglvideo
==========

A simple, cross-platform MP4/MOV video player with the following priorities:

* ease of use
* ease of maintenance
* performance and customizability

libglvideo is best suited for those that need simple, fast video in OpenGL and can
control their entire video workflow. Compared to other video players, libglvideo
 is best defined by what it lacks:

* audio (for now)
* support for containers other than MP4/MOV
* support for most codecs, especially codecs that do inter-frame compression

Platform Support
----------------

Tested on:

* OS X 10.11 (x86_64)
* Windows 10 (x64)

Codec Support
-------------

* MJPEG (slow, done on the CPU)
* Hap YCoCg

Coming soon
-----------

* other Hap formats
* OpenEXR?
* audio

Examples
--------

There are two bare-bones example applications in the [examples](examples/) directory:
play and stress. They require test videos under examples/videos/ which can be fetched
with git lfs: `git lfs fetch`. If that doesn't work, you can re-generate them by doing:

```
cd examples/videos
./generate.sh
```

This requires ffmpeg be installed with HapQ support.

You will need to run the examples from the examples directory for them to be able to find
the test videos. If all else fails, you can edit the paths in [play.cpp](examples/play.cpp)
or [stress.cpp](examples/stress.cpp) to point at really any HapQ mov/mp4.

Building
--------

```
git submodule update --init --recursive # pull in dependencies
mkdir build
cd build
cmake ../
make
```

or maybe use `cmake .. -G "Visual Studio 14 2015 Win64"` if you're on Windows.

CMake
-----

To incorporate this library into another project with CMake, add something like the
following lines to your CMakeLists.txt file:

    get_filename_component(LIBGLVIDEO_PATH  "${APP_PATH}/lib/libglvideo" ABSOLUTE)
    find_package(libglvideo REQUIRED PATHS "${LIBGLVIDEO_PATH}/cmake" NO_DEFAULT_PATH)
    include_directories("${LIBGLVIDEO_INCLUDE_DIRS}")
    target_link_libraries(my-executable libglvideo)

License
-------

Due to Bento4's [licensing](https://www.bento4.com/about/), libglvideo is licensed under
the GPL. Bento4 allows alternative licenses by negotiation with the author. If you obtain
an alternative Bento4 license, this code may be licensed under an MIT license. 
