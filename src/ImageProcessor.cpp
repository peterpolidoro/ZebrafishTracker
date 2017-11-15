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
  image_n_ = 0;
  cv::Point frame_rate_position_(50,500);

  // Create Window
  cv::namedWindow("Processed Images", 1);

  FrameRateCounter frame_rate_counter_(queue_length_);
  frame_rate_counter_.Reset();
}

void ImageProcessor::processImage(cv::Mat & image)
{
  frame_rate_counter_.NewFrame();
  image_n_ = (image_n_ + 1) % display_divisor_;
  if (image_n_ == 0)
  {
    std::stringstream frame_rate_ss;
    frame_rate_ss << getFrameRate();
    std::string frame_rate_string = std::string("Frame rate: ") + std::string(frame_rate_ss.str());
    // cv::putText(image,frame_rate_string,frame_rate_position_,cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,200,200),4);
    // std::cout << "Frame rate: " << getFrameRate() << std::endl;
    std::cout << frame_rate_string << std::endl;
    std::cout << std::endl;
    cv::imshow("Processed Images",image);
    cv::waitKey(1);
  }
}

double ImageProcessor::getFrameRate()
{
  return frame_rate_counter_.GetFrameRate();
}

// private
