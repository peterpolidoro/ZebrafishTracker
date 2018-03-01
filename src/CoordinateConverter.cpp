// ----------------------------------------------------------------------------
// CoordinateConverter.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "CoordinateConverter.h"


// public
CoordinateConverter::CoordinateConverter()
{
  homography_image_to_stage_set_ = false;
}

bool CoordinateConverter::updateHomographyImageToStage()
{
  bool success = configuration_.readHomographyImageToStage(homography_image_to_stage_);
  if (!success)
  {
    return success;
  }

  homography_image_to_stage_set_ = true;
  return success;
}

bool CoordinateConverter::convertImagePointToStagePoint(cv::Point & image_point, cv::Point & stage_point)
{
  if (homography_image_to_stage_set_)
  {
    std::vector<cv::Point2f> image_points;
    image_points.push_back(image_point);
    std::vector<cv::Point2f> stage_points;
    cv::perspectiveTransform(image_points,stage_points,homography_image_to_stage_);
    stage_point = stage_points[0];
  }
  return homography_image_to_stage_set_;
}

// private
