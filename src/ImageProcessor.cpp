// ----------------------------------------------------------------------------
// ImageProcessor.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "ImageProcessor.h"


// public
ImageProcessor::ImageProcessor()
{
  image_count_ = 0;
  frame_rate_position_ = cv::Point(50,50);
  keypoints_position_ = cv::Point(50,100);

  mog2_ptr_ = cv::createBackgroundSubtractorMOG2(background_history_,
                                                 background_var_threshold_,
                                                 background_shadow_detection_);

  // Create Window
  cv::namedWindow("Image",cv::WINDOW_NORMAL);

  blue_ = cv::Scalar(255,0,0);
  yellow_ = cv::Scalar(0,255,255);
  green_ = cv::Scalar(0,255,0);
  red_ = cv::Scalar(0,0,255);

  FrameRateCounter frame_rate_counter_(queue_length_);
  frame_rate_counter_.Reset();

  mode_ = BLOB;
}

bool ImageProcessor::updateTrackedImagePoint(cv::Mat image, cv::Point * tracked_image_point_ptr)
{
  updateFrameRateMeasurement();
  bool success = false;
  switch (mode_)
  {
    case BLOB:
    {
      updateBackground(image);

      cv::Point blob_center;
      success = findBlobCenter(image,blob_center);

      if (success)
      {
        *tracked_image_point_ptr = blob_center;
      }
      break;
    }
    case MOUSE:
    {
      MouseParams mp;
      mp.image = image;
      mp.tracked_image_point_ptr = tracked_image_point_ptr;
      mp.success = false;
      cv::setMouseCallback("Image",onMouse,&mp);
      cv::imshow("Image",image);
      cv::waitKey(1);
      success = mp.success;
      break;
    }
  }
  ++image_count_;

  return success;
}

void ImageProcessor::setMode(ImageProcessor::Mode mode)
{
  mode_ = mode;
}

// private

void ImageProcessor::updateFrameRateMeasurement()
{
  frame_rate_counter_.NewFrame();
}

void ImageProcessor::updateBackground(cv::Mat & image)
{
  if ((image_count_ % background_divisor_) == 0)
  {
    mog2_ptr_->apply(image,
                     foreground_mask_,
                     background_learing_rate_);
  }
}

double ImageProcessor::getFrameRate()
{
  return frame_rate_counter_.GetFrameRate();
}

bool ImageProcessor::findBlobCenter(cv::Mat & image, cv::Point & blob_center)
{
  // Find blob center
  blob_center = cv::Point(0,0);

  // // Setup SimpleBlobDetector parameters.
  // cv::SimpleBlobDetector::Params params;

  // // Change thresholds
  // params.minThreshold = 25;
  // // params.maxThreshold = 200;
  // params.maxThreshold = 30;

  // // Filter by Area.
  // // params.filterByArea = true;
  // params.filterByArea = false;
  // params.minArea = 1500;

  // // Filter by Circularity
  // // params.filterByCircularity = true;
  // params.filterByCircularity = false;
  // params.minCircularity = 0.1;

  // // Filter by Convexity
  // // params.filterByConvexity = true;
  // params.filterByConvexity = false;
  // params.minConvexity = 0.87;

  // // Filter by Inertia
  // // params.filterByInertia = true;
  // params.filterByInertia = false;
  // params.minInertiaRatio = 0.01;

  // // Set up the detector with default parameters.
  // cv::SimpleBlobDetector detector(params);

  // // Detect blobs.
  // std::vector<cv::KeyPoint> keypoints;
  // // detector.detect(threshold_,keypoints);
  // detector.detect(image,keypoints);

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
  // if ((image_count_ % display_divisor_) == 0)
  // {
  //   cv::cvtColor(image,display_image_,CV_GRAY2BGR);
  //   // cv::cvtColor(threshold_,display_image_,CV_GRAY2BGR);

  //   // Draw detected blobs as red circles.
  //   // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
  //   // cv::drawKeypoints(image,
  //   //                   keypoints,
  //   //                   display_image_,
  //   //                   cv::Scalar(0,0,255),
  //   //                   cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

  //   // // Draw Contour on Output Image
  //   // std::vector<std::vector<cv::Point> > contours_to_draw;
  //   // contours_to_draw.push_back(contours[max_ind]);
  //   // cv::drawContours(display_image_,contours_to_draw,-1,blue_,2);

  //   std::stringstream frame_rate_ss;
  //   frame_rate_ss << getFrameRate();
  //   std::string frame_rate_string = std::string("Frame rate: ") + std::string(frame_rate_ss.str());
  //   cv::putText(display_image_,
  //               frame_rate_string,
  //               frame_rate_position_,
  //               cv::FONT_HERSHEY_SIMPLEX,
  //               1,
  //               yellow_,
  //               4);

  //   std::stringstream keypoints_ss;
  //   keypoints_ss << keypoints.size();
  //   std::string keypoints_string = std::string("Keypoints count: ") + std::string(keypoints_ss.str());
  //   cv::putText(display_image_,
  //               keypoints_string,
  //               keypoints_position_,
  //               cv::FONT_HERSHEY_SIMPLEX,
  //               1,
  //               yellow_,
  //               4);

  //   cv::imshow("Image",display_image_);
  //   cv::waitKey(1);
  // }

  return SUCCESS;
}

void ImageProcessor::onMouse(int event, int x, int y, int flags, void * userdata)
{
  MouseParams * mp_ptr = (MouseParams *)userdata;
  if(event != cv::EVENT_LBUTTONDOWN)
  {
    mp_ptr->success = false;
    return;
  }
  mp_ptr->tracked_image_point_ptr->x = x;
  mp_ptr->tracked_image_point_ptr->y = y;
  mp_ptr-> success = true;

  std::cout << "Clicked point x: " << x << ", y: " << y << std::endl;

  // Point seed = Point(x,y);
  // int lo = ffillMode == 0 ? 0 : loDiff;
  // int up = ffillMode == 0 ? 0 : upDiff;
  // int flags = connectivity + (newMaskVal << 8) +
  //   (ffillMode == 1 ? FLOODFILL_FIXED_RANGE : 0);
  // int b = (unsigned)theRNG() & 255;
  // int g = (unsigned)theRNG() & 255;
  // int r = (unsigned)theRNG() & 255;
  // Rect ccomp;

  // Scalar newVal = isColor ? Scalar(b, g, r) : Scalar(r*0.299 + g*0.587 + b*0.114);
  // Mat dst = isColor ? image : gray;
  // int area;

  // if( useMask )
  // {
  //   threshold(mask, mask, 1, 128, THRESH_BINARY);
  //   area = floodFill(dst, mask, seed, newVal, &ccomp, Scalar(lo, lo, lo),
  //                    Scalar(up, up, up), flags);
  //   imshow( "mask", mask );
  // }
  // else
  // {
  //   area = floodFill(dst, seed, newVal, &ccomp, Scalar(lo, lo, lo),
  //                    Scalar(up, up, up), flags);
  // }

  // imshow("image", dst);
  // cout << area << " pixels were repainted\n";
}
