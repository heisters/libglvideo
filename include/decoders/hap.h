#pragma once

#include "Frame.h"
#include <AP4DataBuffer.h>

glvideo::Frame::ref decodeHapFrame( AP4_DataBuffer &sampleData );
