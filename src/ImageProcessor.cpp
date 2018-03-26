// ----------------------------------------------------------------------------
// ImageProcessor.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "ImageProcessor.h"


cv::Point ImageProcessor::tracked_image_point_;
int ImageProcessor::threshold_value_;

// public
ImageProcessor::ImageProcessor()
{
  image_count_ = 0;
  mode_ = BLOB;
  show_ = true;
  windows_ = false;

  tracked_image_point_ = cv::Point(0,0);

  threshold_value_ = THRESHOLD_VALUE_DEFAULT;

  frame_rate_ = 0;
  frame_tick_count_prev_ = 0;

  blue_ = cv::Scalar(255,0,0);
  yellow_ = cv::Scalar(0,255,255);
  green_ = cv::Scalar(0,255,0);
  red_ = cv::Scalar(0,0,255);

  frame_rate_display_position_ = cv::Point(50,50);

  gpu_enabled_ = false;
}

void ImageProcessor::setMode(ImageProcessor::Mode mode)
{
  mode_ = mode;
}

void ImageProcessor::show()
{
  show_ = true;
}

void ImageProcessor::hide()
{
  show_ = false;
}

void ImageProcessor::enableGpu()
{
  gpu_enabled_ = true;
}

void ImageProcessor::allocateMemory(unsigned char * const image_data_ptr,
                                    const cv::Size image_size,
                                    const int image_type,
                                    const unsigned int image_data_size)
{
  image_data_ptr_ = image_data_ptr;
  image_size_ = image_size;
  image_type_ = image_type;
  image_data_size_ = image_data_size;

  bg_sub_ptr_ = cv::createBackgroundSubtractorMOG2();
  bg_sub_ptr_->setHistory(BACKGROUND_HISTORY);
  bg_sub_ptr_->setVarThreshold(BACKGROUND_VAR_THRESHOLD);
  bg_sub_ptr_->setDetectShadows(BACKGROUND_DETECT_SHADOWS);


  if (gpu_enabled_)
  {
    // image_g_ = cv::cuda::GpuMat(image_size_,image_type_,image_data_ptr_);

    // bg_sub_ptr_g_ = cv::cuda::createBackgroundSubtractorMOG2();
    // bg_sub_ptr_g_->setHistory(BACKGROUND_HISTORY);
    // bg_sub_ptr_g_->setVarThreshold(BACKGROUND_VAR_THRESHOLD);
    // bg_sub_ptr_g_->setDetectShadows(BACKGROUND_DETECT_SHADOWS);

    // cudaMallocManaged((void**)&background_data_ptr_,image_data_size_);
    // background_ = cv::Mat(image_size_,image_type_,background_data_ptr_);
    // background_g_ = cv::cuda::GpuMat(image_size_,image_type_,background_data_ptr_);

    // cudaMallocManaged((void**)&foreground_data_ptr_,image_data_size_);
    // foreground_ = cv::Mat(image_size_,image_type_,foreground_data_ptr_);
    // foreground_g_ = cv::cuda::GpuMat(image_size_,image_type_,foreground_data_ptr_);

    // cudaMallocManaged((void**)&foreground_mask_data_ptr_,image_data_size_);
    // foreground_mask_ = cv::Mat(image_size_,image_type_,foreground_mask_data_ptr_);
    // foreground_mask_g_ = cv::cuda::GpuMat(image_size_,image_type_,foreground_mask_data_ptr_);

    // cudaMallocManaged((void**)&threshold_data_ptr_,image_data_size_);
    // threshold_ = cv::Mat(image_size_,image_type_,threshold_data_ptr_);
    // threshold_g_ = cv::cuda::GpuMat(image_size_,image_type_,threshold_data_ptr_);

  }
}

void ImageProcessor::update(cv::Mat image)
{
  if (show_ && !windows_)
  {
    createWindows();
  }
  updateFrameRateMeasurement();
  cv::Point tracked_point = cv::Point(0,0);
  switch (mode_)
  {
    case BLOB:
    {
      updateBackground(image);

      findBlobLocation(image,tracked_point);
      break;
    }
    case MOUSE:
    {
      findClickedLocation(image,tracked_point);
      break;
    }
  }

  tracked_image_point_ = tracked_point;

  displayImage(image);

  ++image_count_;
}

void ImageProcessor::getTrackedImagePoint(cv::Point & tracked_image_point)
{
  tracked_image_point = tracked_image_point_;
}

// private

void ImageProcessor::createWindows()
{
  destroyWindows();

  cv::namedWindow("Image",cv::WINDOW_NORMAL);

  switch (mode_)
  {
    case BLOB:
    {
      cv::namedWindow("Background",cv::WINDOW_NORMAL);
      cv::namedWindow("Foreground",cv::WINDOW_NORMAL);
      cv::namedWindow("Threshold",cv::WINDOW_NORMAL);
      char TrackbarName[50];
      sprintf(TrackbarName, "threshold_value");
      cv::createTrackbar(TrackbarName,
                         "Threshold",
                         &threshold_value_,
                         MAX_PIXEL_VALUE,
                         trackbarThresholdHandler);
      break;
    }
    case MOUSE:
    {
      cv::setMouseCallback("Image",mouseClickHandler);
      break;
    }
  }
  windows_ = true;
}

void ImageProcessor::destroyWindows()
{
  cv::destroyAllWindows();
  windows_ = false;
}

void ImageProcessor::updateFrameRateMeasurement()
{
  if ((image_count_ % FRAME_RATE_FRAME_COUNT) == 0)
  {
    int64 frame_tick_count = cv::getTickCount();
    int64 tick_delta = (frame_tick_count - frame_tick_count_prev_);
    double frame_rate = cv::getTickFrequency()/(double)tick_delta;
    frame_rate *= FRAME_RATE_FRAME_COUNT;
    // frame_rate_ = (FRAME_RATE_ALPHA*frame_rate) + (1.0 - FRAME_RATE_ALPHA)*frame_rate_;
    frame_rate_ = frame_rate;
    frame_tick_count_prev_ = frame_tick_count;
    std::cout << "frame_rate: " << frame_rate_ << std::endl;
  }
}

void ImageProcessor::updateBackground(cv::Mat image)
{
  if ((image_count_ % BACKGROUND_DIVISOR) == 0)
  {
    if (gpu_enabled_)
    {
      // bg_sub_ptr_g_->apply(image_g_,foreground_mask_g_,BACKGROUND_LEARNING_RATE);
      // bg_sub_ptr_g_->getBackgroundImage(background_g_);
    }
    else
    {
      bg_sub_ptr_->apply(image,foreground_mask_,BACKGROUND_LEARNING_RATE);
      bg_sub_ptr_->getBackgroundImage(background_);
    }
    // std::cout << "image.data: " << (long)image.data << std::endl;
    // std::cout << "image_g_.data: " << (long)image_g_.data << std::endl;
  }
}

double ImageProcessor::getFrameRate()
{
  return frame_rate_;
}

void ImageProcessor::findBlobLocation(cv::Mat image, cv::Point & location)
{
  if (gpu_enabled_)
  {
    // cv::cuda::subtract(background_g_,image_g_,foreground_g_);
    // cv::threshold(foreground_,threshold_,threshold_value_,MAX_PIXEL_VALUE,cv::THRESH_BINARY);
    // cv::cuda::threshold(foreground_g_,threshold_g_,threshold_value_,MAX_PIXEL_VALUE,cv::THRESH_BINARY);

    // std::vector<cv::Point> locations;
    // cv::cuda::findNonZero(threshold_g_,locations);
  }
  else
  {
    cv::subtract(background_,image,foreground_);
    cv::threshold(foreground_,threshold_,threshold_value_,MAX_PIXEL_VALUE,cv::THRESH_BINARY);

    std::vector<cv::Point> locations;
    cv::findNonZero(threshold_,locations);

    // Choose one
    // if (locations.size() > 0)
    // {
    //   cv::Point2f sum(0,0);
    //   for (size_t i=0; i<locations.size(); ++i)
    //   {
    //     sum = sum + cv::Point2f(locations[i]);
    //   }
    //   cv::Point2f mean;
    //   mean.x = sum.x/locations.size();
    //   mean.y = sum.y/locations.size();
    //   location = mean;
    // }
  }
}

void ImageProcessor::findClickedLocation(cv::Mat image, cv::Point & location)
{
}

void ImageProcessor::displayImage(cv::Mat image)
{
  if (!show_ || !windows_)
  {
    return;
  }
  // Update display
  if ((image_count_ % DISPLAY_DIVISOR) == 0)
  {
    cv::cvtColor(image,display_image_,CV_GRAY2BGR);

    cv::circle(display_image_,
               tracked_image_point_,
               DISPLAY_MARKER_RADIUS,
               red_,
               DISPLAY_MARKER_THICKNESS);

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

    showImageInWindow("Image",display_image_);

    switch (mode_)
    {
      case BLOB:
      {
        showImageInWindow("Background",background_);
        showImageInWindow("Foreground",foreground_);
        showImageInWindow("Threshold",threshold_);
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

void ImageProcessor::showImageInWindow(const cv::String & winname, cv::Mat mat)
{
  if ((mat.cols != 0) && (mat.rows != 0))
  {
    cv::imshow(winname,mat);
  }
}

void ImageProcessor::trackbarThresholdHandler(int value, void * userdata)
{
}

void ImageProcessor::mouseClickHandler(int event, int x, int y, int flags, void * userdata)
{
  if(event != cv::EVENT_LBUTTONDOWN)
  {
    return;
  }
  tracked_image_point_.x = x;
  tracked_image_point_.y = y;

  std::cout << "Clicked point x: " << x << ", y: " << y << std::endl;
}
