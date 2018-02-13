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

#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <sstream>

#include "FrameRateCounter.h"


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

  bool updateTrackedImagePoint(cv::Mat image);
  bool getTrackedImagePoint(cv::Point & tracked_image_point);

private:
  const static bool SUCCESS = true;

  unsigned long image_count_;
  Mode mode_;

  cv::Point tracked_image_point_;
  bool tracked_image_point_is_valid_;

  static const size_t BACKGROUND_DIVISOR = 200;
  static const double THRESHOLD_VALUE = 25;
  static const double MAX_PIXEL_VALUE = 255;
  cv::Mat background_;
  cv::Mat foreground_;
  cv::Mat threshold_;

  cv::Mat eroded_;
  cv::Mat dilated_;
  cv::Mat kernel_;
  // int erosion_elem_;
  // int erosion_size_;
  // int dilation_elem_;
  // int dilation_size_;
  static const int MAX_KERNEL_ELEM = 2;
  static const int MAX_KERNEL_SIZE = 21;

  static const int KERNEL_SHAPE = cv::MORPH_RECT;
  static const int KERNEL_SIZE = 3;

  static const size_t FRAME_RATE_QUEUE_LENGTH = 100;
  FrameRateCounter frame_rate_counter_;

  static const size_t DISPLAY_DIVISOR = 10;
  static const int DISPLAY_MARKER_RADIUS = 10;
  static const int DISPLAY_MARKER_THICKNESS = 2;

  cv::Scalar blue_;
  cv::Scalar yellow_;
  cv::Scalar green_;
  cv::Scalar red_;

  cv::Point frame_rate_display_position_;

  cv::Mat display_image_;

  struct MouseParams
  {
    cv::Mat image;
    cv::Point * tracked_image_point_ptr;
    bool success;
  };

  void updateFrameRateMeasurement();
  void updateBackground(cv::Mat image);
  double getFrameRate();
  bool findBlobLocation(cv::Mat image, cv::Point & location);
  bool findClickedLocation(cv::Mat image, cv::Point & location);
  void displayImage(cv::Mat image);
  static void onMouse(int event, int x, int y, int flags, void * userdata);
};

#endif
