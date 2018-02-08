// ----------------------------------------------------------------------------
// ImageProcessor.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _IMAGE_PROCESSOR_H_
#define _IMAGE_PROCESSOR_H_
// #include "opencv2/imgcodecs.hpp"
// #include "opencv2/imgproc.hpp"
// #include "opencv2/videoio.hpp"
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
  static const size_t DISPLAY_DIVISOR = 10;
  unsigned long image_count_;
  cv::Point frame_rate_position_;
  cv::Ptr<cv::BackgroundSubtractor> mog2_ptr_;
  cv::Mat foreground_mask_;
  cv::Mat background_;
  cv::Mat foreground_;
  cv::Mat threshold_;
  static const size_t BACKGROUND_DIVISOR = 10;
  static const int BACKGROUND_HISTORY = 100;
  static const float BACKGROUND_VAR_THRESHOLD = 16;
  static const double BACKGROUND_LEARING_RATE = 0.01;
  static const bool BACKGROUND_SHADOW_DETECTION = false;
  static const double THRESHOLD_VALUE = 25;
  static const double MAX_PIXEL_VALUE = 255;
  cv::Scalar blue_;
  cv::Scalar yellow_;
  cv::Scalar green_;
  cv::Scalar red_;

  cv::Mat processed_image_;
  cv::Mat kernel_;
  static const int KERNEL_SHAPE = cv::MORPH_RECT;
  static const int KERNEL_SIZE = 3;

  cv::Mat display_image_;
  static const int DISPLAY_MARKER_RADIUS = 10;
  static const int DISPLAY_MARKER_THICKNESS = 2;

  static const size_t FRAME_RATE_QUEUE_LENGTH = 10;
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
  bool findBlobLocation(cv::Mat image, cv::Point & location);
  bool findClickedLocation(cv::Mat image, cv::Point & location);
  void displayImage(cv::Mat & image, cv::Point & tracked_point, const bool success);
  static void onMouse(int event, int x, int y, int flags, void * userdata);
};

#endif
