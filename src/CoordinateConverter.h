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
#include <opencv2/core.hpp>

#include "Configuration.h"


class CoordinateConverter
{
public:
  CoordinateConverter();

  bool updateHomographyImageToStage();
  bool convertImagePointToStagePoint(cv::Point & image_point, cv::Point & stage_point);

private:
  const static bool SUCCESS = true;
  Configuration configuration_;
  cv::Mat homography_image_to_stage_;
  bool homography_image_to_stage_set_;
};

#endif
