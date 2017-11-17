// ----------------------------------------------------------------------------
// BlobTracker.h
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
#include <opencv2/video/background_segm.hpp>

#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>
#include "FrameRateCounter.h"

#include <iostream>
#include <sstream>


class BlobTracker
{
public:
  BlobTracker();

  cv::Point findBlobCenter(cv::Mat & image);
  double getFrameRate();
private:
  static const size_t display_divisor_=10;
  unsigned long image_n_;
  cv::Point frame_rate_position_;
  cv::Ptr<cv::BackgroundSubtractor> mog2_ptr_;
  cv::Mat foreground_;
  cv::Mat background_;
  cv::Mat threshold_;
  static const size_t background_divisor_=10;
  static const int background_history_=100;
  static const float background_var_threshold_=16;
  // static const double background_learing_rate_=0.001;
  static const double background_learing_rate_=-1;
  static const bool background_shadow_detection_=false;
  static const double threshold_value_=25;
  static const double max_value_=255;
  cv::Mat display_image_;

  static const size_t queue_length_=10;
  FrameRateCounter frame_rate_counter_;
};

#endif
