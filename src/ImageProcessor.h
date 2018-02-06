// ----------------------------------------------------------------------------
// ImageProcessor.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _IMAGE_PROCESSOR_H_
#define _IMAGE_PROCESSOR_H_
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <sstream>

#include "FrameRateCounter.h"


class ImageProcessor
{
public:
  ImageProcessor();

  bool updateTrackedImagePoint(cv::Mat image, cv::Point * tracked_image_point_ptr);
  enum Mode
  {
    BLOB,
    MOUSE,
  };
  void setMode(Mode mode);

private:
  const static bool SUCCESS = true;
  static const size_t display_divisor_=10;
  unsigned long image_count_;
  cv::Point frame_rate_position_;
  cv::Point keypoints_position_;
  cv::Ptr<cv::BackgroundSubtractor> mog2_ptr_;
  cv::Mat foreground_mask_;
  static const size_t background_divisor_=10;
  static const int background_history_=100;
  static const float background_var_threshold_=16;
  static const double background_learing_rate_=0.01;
  static const bool background_shadow_detection_=false;
  cv::Scalar blue_;
  cv::Scalar yellow_;
  cv::Scalar green_;
  cv::Scalar red_;

  cv::Mat display_image_;

  static const size_t queue_length_=10;
  FrameRateCounter frame_rate_counter_;

  Mode mode_;
  struct MouseParams
  {
    cv::Mat image;
    cv::Point * tracked_image_point_ptr;
    bool success;
  };

  void updateFrameRateMeasurement();
  void updateBackground(cv::Mat & image);
  double getFrameRate();
  bool findBlobCenter(cv::Mat & image, cv::Point & blob_center);
  static void onMouse(int event, int x, int y, int flags, void * userdata);
};

#endif
