// ----------------------------------------------------------------------------
// ImageProcessor.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _IMAGE_PROCESSOR_H_
#define _IMAGE_PROCESSOR_H_
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>
#include "FrameRateCounter.h"

#include <iostream>
#include <sstream>


class ImageProcessor
{
public:
  ImageProcessor();

  void processImage(cv::Mat & image);
  double getFrameRate();
private:
  static const size_t display_divisor_=10;
  size_t image_n_;
  cv::Point frame_rate_position_;

  static const size_t queue_length_=100;
  FrameRateCounter frame_rate_counter_;
};

#endif
