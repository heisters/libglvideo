#!/bin/bash

rm -v *.mov
duration=60

ffmpeg -f lavfi -i testsrc=duration=$duration:size=3840x2160:rate=60 -vcodec hap -format hap_q hap-3840x2160-60fps.mov
ffmpeg -f lavfi -i testsrc=duration=$duration:size=3840x2160:rate=24 -vcodec hap -format hap_q hap-3840x2160-24fps.mov
ffmpeg -f lavfi -i testsrc=duration=$duration:size=1920x1080:rate=24 -vcodec hap -format hap_q hap-1920x1080-24fps.mov

ffmpeg -f lavfi -i testsrc=duration=$duration:size=1920x1080:rate=24 -vcodec mjpeg mjpeg-1920x1080-24fps.mov
