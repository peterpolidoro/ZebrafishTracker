// ----------------------------------------------------------------------------
// ImageProcessor.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _IMAGE_PROCESSOR_H_
#define _IMAGE_PROCESSOR_H_
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/cuda.hpp>

#ifdef __CUDACC__
#include <cuda_runtime_api.h>
#include <cuda.h>
#endif

#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <sstream>


class ImageProcessor
{
public:
  ImageProcessor();

  enum Mode
  {
    BLOB,
    MOUSE,
  };
  void setMode(Mode mode);

  void updateTrackedImagePoint(cv::Mat image);
  void getTrackedImagePoint(cv::Point & tracked_image_point);

private:
  unsigned long image_count_;
  Mode mode_;

  static cv::Point tracked_image_point_;

  cv::Ptr<cv::BackgroundSubtractorMOG2> bg_sub_ptr_;
  static const size_t BACKGROUND_HISTORY = 200;
  static const size_t BACKGROUND_VAR_THRESHOLD = 16;
  static const bool BACKGROUND_DETECT_SHADOWS = false;
  static const double BACKGROUND_LEARNING_RATE = 0.15;
  static const size_t BACKGROUND_DIVISOR = 400;
  static const double MAX_PIXEL_VALUE = 255;
  cv::Mat background_;
  cv::Mat foreground_mask_;
  cv::Mat foreground_;
  cv::Mat threshold_;

  static const int THRESHOLD_VALUE_DEFAULT = 10;
  static int threshold_value_;

  static const double FRAME_RATE_ALPHA = 0.5;
  double frame_rate_;
  double frame_tick_count_prev_;

  static const size_t DISPLAY_DIVISOR = 15;
  static const int DISPLAY_MARKER_RADIUS = 10;
  static const int DISPLAY_MARKER_THICKNESS = 2;

  cv::Scalar blue_;
  cv::Scalar yellow_;
  cv::Scalar green_;
  cv::Scalar red_;

  cv::Point frame_rate_display_position_;

  cv::Mat display_image_;

  int cuda_device_count_;

  void updateFrameRateMeasurement();
  void updateBackground(cv::Mat image);
  double getFrameRate();
  void findBlobLocation(cv::Mat image, cv::Point & location);
  void findClickedLocation(cv::Mat image, cv::Point & location);
  void displayImage(cv::Mat image);
  static void trackbarThresholdHandler(int value, void * userdata);
  static void mouseClickHandler(int event, int x, int y, int flags, void * userdata);
};

#endif
