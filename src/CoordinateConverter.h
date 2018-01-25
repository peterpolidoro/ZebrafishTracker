// ----------------------------------------------------------------------------
// CoordinateConverter.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _COORDINATE_CONVERTER_H_
#define _COORDINATE_CONVERTER_H_
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>


class CoordinateConverter
{
public:
  CoordinateConverter();

  void setHomographyImageToStage(cv::Mat homography_image_to_stage);
  bool convertImagePointToStagePoint(cv::Point & image_point, cv::Point & stage_point);

private:
  const static bool SUCCESS = true;
  cv::Mat homography_image_to_stage_;
  bool homography_image_to_stage_set_;
};

#endif
