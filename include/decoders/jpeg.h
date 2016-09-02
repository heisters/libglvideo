#pragma once

#include "Frame.h"
#include <AP4DataBuffer.h>

glvideo::Frame::ref decodeJpegFrame( AP4_DataBuffer &sampleData );
