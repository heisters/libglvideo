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

License
-------

Due to Bento4's [licensing](https://www.bento4.com/about/), libglvideo is licensed under
the GPL. Bento4 allows alternative licenses by negotiation with the author. If you obtain
an alternative Bento4 license, this code may be licensed under an MIT license. 