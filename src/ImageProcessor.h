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

  static cv::Point tracked_image_point_;
  static bool tracked_image_point_is_valid_;

  cv::Ptr<cv::BackgroundSubtractorMOG2> bg_sub_ptr_;
  static const size_t BACKGROUND_HISTORY = 200;
  static const size_t BACKGROUND_VAR_THRESHOLD = 16;
  static const bool BACKGROUND_DETECT_SHADOWS = false;
  static const double BACKGROUND_LEARNING_RATE = 0.15;
  static const size_t BACKGROUND_DIVISOR = 400;
  static const double THRESHOLD_VALUE = 25;
  static const double MAX_PIXEL_VALUE = 255;
  cv::Mat background_;
  cv::Mat foreground_mask_;
  cv::Mat foreground_;
  cv::Mat threshold_;

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

  void updateFrameRateMeasurement();
  void updateBackground(cv::Mat image);
  double getFrameRate();
  bool findBlobLocation(cv::Mat image, cv::Point & location);
  bool findClickedLocation(cv::Mat image, cv::Point & location);
  void displayImage(cv::Mat image);
  static void onMouse(int event, int x, int y, int flags, void * userdata);
};

#endif
