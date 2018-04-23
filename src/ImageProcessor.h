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
  void show();
  void hide();

  void allocateMemory(unsigned char * const image_data_ptr,
                      const cv::Size image_size,
                      const int image_type,
                      const unsigned int image_data_size);

  void update(cv::Mat image);
  void getTrackedImagePoint(cv::Point & tracked_image_point);

private:
  unsigned long image_count_;
  Mode mode_;
  bool show_;
  bool windows_;

  static cv::Point tracked_image_point_;

  cv::Ptr<cv::BackgroundSubtractorMOG2> bg_sub_ptr_;
  static const size_t BACKGROUND_HISTORY = 200;
  static const size_t BACKGROUND_VAR_THRESHOLD = 16;
  static const bool BACKGROUND_DETECT_SHADOWS = false;
  static const double BACKGROUND_LEARNING_RATE = 0.15;
  static const size_t BACKGROUND_DIVISOR = 400;
  static const double MAX_PIXEL_VALUE = 255;

  unsigned char * image_data_ptr_;
  cv::Size image_size_;
  int image_type_;
  unsigned int image_data_size_;

  cv::Mat background_;
  cv::Mat foreground_mask_;
  cv::Mat foreground_;
  cv::Mat threshold_;

  unsigned char * background_data_ptr_;
  unsigned char * foreground_data_ptr_;
  unsigned char * foreground_mask_data_ptr_;
  unsigned char * threshold_data_ptr_;

  static const int THRESHOLD_VALUE_DEFAULT = 10;
  static int threshold_value_;

  static const double FRAME_RATE_ALPHA = 0.5;
  static const size_t FRAME_RATE_FRAME_COUNT = 100;
  double frame_rate_;
  int64 frame_tick_count_prev_;

  static const size_t DISPLAY_DIVISOR = 15;
  static const int DISPLAY_MARKER_RADIUS = 10;
  static const int DISPLAY_MARKER_THICKNESS = 2;

  cv::Scalar blue_;
  cv::Scalar yellow_;
  cv::Scalar green_;
  cv::Scalar red_;

  cv::Point frame_rate_display_position_;

  cv::Mat display_image_;

  void createWindows();
  void destroyWindows();
  void updateFrameRateMeasurement();
  void updateBackground(cv::Mat image);
  double getFrameRate();
  void findBlobLocation(cv::Mat image, cv::Point & location);
  void findClickedLocation(cv::Mat image, cv::Point & location);
  void displayImage(cv::Mat image);
  void showImageInWindow(const cv::String & winname, cv::Mat mat);
  static void trackbarThresholdHandler(int value, void * userdata);
  static void mouseClickHandler(int event, int x, int y, int flags, void * userdata);
};

#endif
