// ----------------------------------------------------------------------------
// BlobTracker.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "BlobTracker.h"


// public
BlobTracker::BlobTracker()
{
  image_n_ = 0;
  frame_rate_position_ = cv::Point(50,50);

  mog2_ptr_ = new cv::BackgroundSubtractorMOG2(background_history_,
                                               background_var_threshold_,
                                               background_shadow_detection_);

  // Create Window
  cv::namedWindow("Processed Images",cv::WINDOW_NORMAL);

  FrameRateCounter frame_rate_counter_(queue_length_);
  frame_rate_counter_.Reset();
}

bool BlobTracker::findBlobCenter(cv::Mat & image, cv::Point & blob_center)
{
  bool success = true;

  // Update frame rate measurement
  frame_rate_counter_.NewFrame();

  // Update background
  if ((image_n_ % background_divisor_) == 0)
  {
    mog2_ptr_->operator()(image,
                          foreground_,
                          background_learing_rate_);
    mog2_ptr_->getBackgroundImage(background_);
  }

  // Find blob center
  blob_center = cv::Point(0,0);
  cv::subtract(image,background_,foreground_);
  cv::threshold(foreground_,threshold_,threshold_value_,max_value_,cv::THRESH_BINARY_INV);

  // Update display
  if ((image_n_ % display_divisor_) == 0)
  {
    cv::cvtColor(threshold_,display_image_,CV_GRAY2BGR);
    std::stringstream frame_rate_ss;
    frame_rate_ss << getFrameRate();
    std::string frame_rate_string = std::string("Frame rate: ") + std::string(frame_rate_ss.str());
    cv::putText(display_image_,
                frame_rate_string,
                frame_rate_position_,
                cv::FONT_HERSHEY_SIMPLEX,
                1,
                cv::Scalar(0,200,200),
                4);
    cv::imshow("Processed Images",display_image_);
    cv::waitKey(1);
  }

  ++image_n_;

  return success;
}

double BlobTracker::getFrameRate()
{
  return frame_rate_counter_.GetFrameRate();
}

// private
