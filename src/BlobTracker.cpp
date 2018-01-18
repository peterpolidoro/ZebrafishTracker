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
  keypoints_position_ = cv::Point(50,100);

  mog2_ptr_ = new cv::BackgroundSubtractorMOG2(background_history_,
                                               background_var_threshold_,
                                               background_shadow_detection_);

  // Create Window
  cv::namedWindow("Processed Images",cv::WINDOW_NORMAL);

  blue_ = cv::Scalar(255,0,0);
  yellow_ = cv::Scalar(0,255,255);
  green_ = cv::Scalar(0,255,0);
  red_ = cv::Scalar(0,0,255);

  FrameRateCounter frame_rate_counter_(queue_length_);
  frame_rate_counter_.Reset();
}

void BlobTracker::setHomographyImageToStage(cv::Mat homography_image_to_stage)
{
  homography_image_to_stage_ = homography_image_to_stage;
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
  cv::threshold(foreground_,threshold_,threshold_value_,max_value_,cv::THRESH_BINARY);

  // Setup SimpleBlobDetector parameters.
  cv::SimpleBlobDetector::Params params;

  // Change thresholds
  params.minThreshold = 25;
  // params.maxThreshold = 200;
  params.maxThreshold = 30;

  // Filter by Area.
  // params.filterByArea = true;
  params.filterByArea = false;
  params.minArea = 1500;

  // Filter by Circularity
  // params.filterByCircularity = true;
  params.filterByCircularity = false;
  params.minCircularity = 0.1;

  // Filter by Convexity
  // params.filterByConvexity = true;
  params.filterByConvexity = false;
  params.minConvexity = 0.87;

  // Filter by Inertia
  // params.filterByInertia = true;
  params.filterByInertia = false;
  params.minInertiaRatio = 0.01;

  // Set up the detector with default parameters.
  cv::SimpleBlobDetector detector(params);

  // Detect blobs.
  std::vector<cv::KeyPoint> keypoints;
  // detector.detect(threshold_,keypoints);
  detector.detect(image,keypoints);

  // std::vector<std::vector<cv::Point> > contours;
  // cv::findContours(threshold_,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

  // double max_len = 0;
  // int max_ind = -1;
  // double c_len;

  // // Make sure contours detected before calculating features
  // if (contours.size() > 0)
  // {
  //   // Find contour with largest perimeter length
  //   for (int i=0; i<contours.size); ++i)
  //   {
  //     c_len = cv::arcLength(cv::Mat(contours[i]), false);
  //     if(c_len >= max_len)
  //     {
  //       max_len = c_len;
  //       max_ind = i;
  //     }
  //   }
  // }

  // Update display
  if ((image_n_ % display_divisor_) == 0)
  {
    cv::cvtColor(image,display_image_,CV_GRAY2BGR);
    // cv::cvtColor(threshold_,display_image_,CV_GRAY2BGR);

    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    cv::drawKeypoints(image,
                      keypoints,
                      display_image_,
                      cv::Scalar(0,0,255),
                      cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    // // Draw Contour on Output Image
    // std::vector<std::vector<cv::Point> > contours_to_draw;
    // contours_to_draw.push_back(contours[max_ind]);
    // cv::drawContours(display_image_,contours_to_draw,-1,blue_,2);

    std::stringstream frame_rate_ss;
    frame_rate_ss << getFrameRate();
    std::string frame_rate_string = std::string("Frame rate: ") + std::string(frame_rate_ss.str());
    cv::putText(display_image_,
                frame_rate_string,
                frame_rate_position_,
                cv::FONT_HERSHEY_SIMPLEX,
                1,
                yellow_,
                4);

    std::stringstream keypoints_ss;
    keypoints_ss << keypoints.size();
    std::string keypoints_string = std::string("Keypoints count: ") + std::string(keypoints_ss.str());
    cv::putText(display_image_,
                keypoints_string,
                keypoints_position_,
                cv::FONT_HERSHEY_SIMPLEX,
                1,
                yellow_,
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
