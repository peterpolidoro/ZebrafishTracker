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

  // Create Window
  cv::namedWindow("Image",cv::WINDOW_NORMAL);
  cv::namedWindow("Background",cv::WINDOW_NORMAL);
  cv::namedWindow("Foreground",cv::WINDOW_NORMAL);
  cv::namedWindow("Threshold",cv::WINDOW_NORMAL);
  cv::namedWindow("Processed",cv::WINDOW_NORMAL);

  kernel_ = cv::getStructuringElement(KERNEL_SHAPE,
                                      cv::Size(KERNEL_SIZE,KERNEL_SIZE));
  blue_ = cv::Scalar(255,0,0);
  yellow_ = cv::Scalar(0,255,255);
  green_ = cv::Scalar(0,255,0);
  red_ = cv::Scalar(0,0,255);

  FrameRateCounter frame_rate_counter_(FRAME_RATE_QUEUE_LENGTH);
  frame_rate_counter_.Reset();

  mode_ = BLOB;
}

bool ImageProcessor::updateTrackedImagePoint(cv::Mat image, cv::Point * tracked_image_point_ptr)
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
    *tracked_image_point_ptr = tracked_point;
  }

  displayImage(image,tracked_point,success);

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
  if ((image_count_ % BACKGROUND_DIVISOR) == 0)
  {
    if (background_.size() == image.size())
    {
      cv::max(background_,image,background_);
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

  cv::subtract(image,background_,foreground_);
  cv::threshold(foreground_,threshold_,THRESHOLD_VALUE,MAX_PIXEL_VALUE,cv::THRESH_BINARY);

  cv::erode(threshold_,
            processed_image_,
            kernel_);

  std::vector<cv::Point> locations;
  cv::findNonZero(processed_image_,locations);

  // Choose one
  // for now arbitrarily pick first, but could take the mean or something
  if (locations.size() > 0)
  {
    location = locations[0];
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

void ImageProcessor::displayImage(cv::Mat & image, cv::Point & tracked_point, const bool success)
{
  // Update display
  if ((image_count_ % DISPLAY_DIVISOR) == 0)
  {
    cv::cvtColor(image,display_image_,CV_GRAY2BGR);

    if (success)
    {
      cv::circle(display_image_,
                 tracked_point,
                 DISPLAY_MARKER_RADIUS,
                 red_,
                 DISPLAY_MARKER_THICKNESS);
    }

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

    cv::imshow("Image",display_image_);
    cv::imshow("Background",background_);
    cv::imshow("Foreground",foreground_);
    cv::imshow("Threshold",threshold_);
    cv::imshow("Processed",processed_image_);
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
