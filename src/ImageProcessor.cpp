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
  mode_ = BLOB;

  tracked_image_point_ = cv::Point(0,0);
  tracked_image_point_is_valid_ = false;

  kernel_ = cv::getStructuringElement(KERNEL_SHAPE,
                                      cv::Size(KERNEL_SIZE,KERNEL_SIZE));

  FrameRateCounter frame_rate_counter_(FRAME_RATE_QUEUE_LENGTH);
  frame_rate_counter_.Reset();

  blue_ = cv::Scalar(255,0,0);
  yellow_ = cv::Scalar(0,255,255);
  green_ = cv::Scalar(0,255,0);
  red_ = cv::Scalar(0,0,255);

  frame_rate_display_position_ = cv::Point(50,50);

}

bool ImageProcessor::updateTrackedImagePoint(cv::Mat image)
{
  updateFrameRateMeasurement();
  bool success = false;
  cv::Point tracked_point = cv::Point(0,0);
  switch (mode_)
  {
    case BLOB:
    {
      updateBackground(image);

      success = findBlobLocation(image,tracked_point);
      break;
    }
    case MOUSE:
    {
      success = findClickedLocation(image,tracked_point);
      break;
    }
  }

  if (success)
  {
    tracked_image_point_ = tracked_point;
    tracked_image_point_is_valid_ = true;
  }
  else
  {
    tracked_image_point_is_valid_ = false;
  }

  displayImage(image);

  ++image_count_;

  return success;
}

bool ImageProcessor::getTrackedImagePoint(cv::Point & tracked_image_point)
{
  if (tracked_image_point_is_valid_)
  {
    tracked_image_point = tracked_image_point_;
    return true;
  }
  return false;
}

void ImageProcessor::setMode(ImageProcessor::Mode mode)
{
  mode_ = mode;

  cv::destroyAllWindows();

  cv::namedWindow("Image",cv::WINDOW_NORMAL);

  switch (mode_)
  {
    case BLOB:
    {
      cv::namedWindow("Background",cv::WINDOW_NORMAL);
      cv::namedWindow("Foreground",cv::WINDOW_NORMAL);
      cv::namedWindow("Threshold",cv::WINDOW_NORMAL);
      // cv::namedWindow("Eroded",cv::WINDOW_NORMAL);
      // cv::namedWindow("Dilated",cv::WINDOW_NORMAL);
      break;
    }
    case MOUSE:
    {
      break;
    }
  }

}

// private

void ImageProcessor::updateFrameRateMeasurement()
{
  frame_rate_counter_.NewFrame();
}

void ImageProcessor::updateBackground(cv::Mat image)
{
  if ((image_count_ % BACKGROUND_DIVISOR) == 0)
  {
    if (background_.size() == image.size())
    {
      cv::Mat background;
      cv::max(background_,image,background);
      background_ = background;
    }
    else
    {
      background_ = image;
    }
  }
}

double ImageProcessor::getFrameRate()
{
  return frame_rate_counter_.GetFrameRate();
}

bool ImageProcessor::findBlobLocation(cv::Mat image, cv::Point & location)
{
  bool success = false;

  cv::subtract(background_,image,foreground_);
  cv::threshold(foreground_,threshold_,THRESHOLD_VALUE,MAX_PIXEL_VALUE,cv::THRESH_BINARY);

  // cv::erode(threshold_,
  //           eroded_,
  //           kernel_);

  // cv::dilate(eroded_,
  //            dilated_,
  //            kernel_);

  // try
  // {
  //   cv::Moments moments = cv::moments(threshold_);
  //   location.x = moments.m10/moments.m00;
  //   location.y = moments.m01/moments.m00;
  //   success = true;
  // }
  // catch(cv::Exception & e)
  // {
  //   const char * err_msg = e.what();
  //   std::cout << "moments: " << err_msg << std::endl;
  // }


  std::vector<cv::Point> locations;
  cv::findNonZero(threshold_,locations);

  // Choose one
  // for now arbitrarily pick first, but could take the mean or something
  if (locations.size() > 0)
  {
    cv::Point2f sum(0,0);
    for (size_t i=0; i<locations.size(); ++i)
    {
      sum = sum + cv::Point2f(locations[i]);
    }
    cv::Point2f mean;
    mean.x = sum.x/locations.size();
    mean.y = sum.y/locations.size();
    location = mean;
    // std::cout << "locations[0] = " << locations[0] << std::endl;
    // std::cout << "location = " << location << std::endl;
    // location = locations[0];

    success = true;
  }

  return success;
}

bool ImageProcessor::findClickedLocation(cv::Mat image, cv::Point & location)
{
  MouseParams mp;
  mp.image = image;
  mp.tracked_image_point_ptr = &location;
  mp.success = false;
  cv::setMouseCallback("Image",onMouse,&mp);
  cv::imshow("Image",image);
  cv::waitKey(1);
  return mp.success;
}

void ImageProcessor::displayImage(cv::Mat image)
{
  // Update display
  if ((image_count_ % DISPLAY_DIVISOR) == 0)
  {
    cv::cvtColor(image,display_image_,CV_GRAY2BGR);

    if (tracked_image_point_is_valid_)
    {
      cv::circle(display_image_,
                 tracked_image_point_,
                 DISPLAY_MARKER_RADIUS,
                 red_,
                 DISPLAY_MARKER_THICKNESS);
    }

    std::stringstream frame_rate_ss;
    frame_rate_ss << getFrameRate();
    std::string frame_rate_string = std::string("Frame rate: ") + std::string(frame_rate_ss.str());
    cv::putText(display_image_,
                frame_rate_string,
                frame_rate_display_position_,
                cv::FONT_HERSHEY_SIMPLEX,
                1,
                yellow_,
                4);

    cv::imshow("Image",display_image_);

    switch (mode_)
    {
      case BLOB:
      {
        cv::imshow("Background",background_);
        cv::imshow("Foreground",foreground_);
        cv::imshow("Threshold",threshold_);
        // cv::imshow("Eroded",eroded_);
        // cv::imshow("Dilated",dilated_);
        break;
      }
      case MOUSE:
      {
        break;
      }
    }
    cv::waitKey(1);
  }
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
}
