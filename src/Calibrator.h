// ----------------------------------------------------------------------------
// Calibrator.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _CALIBRATOR_H_
#define _CALIBRATOR_H_
#include <iostream>
#include <opencv2/core.hpp>
#include <boost/filesystem.hpp>


class Calibrator
{
public:
  Calibrator();

  bool getHomographyImageToStage(cv::Mat & homography_image_to_stage);
  // bool convertImagePointToStagePoint(cv::Point & image_point, cv::Point & stage_point);

private:
  // const static bool SUCCESS = true;
  // cv::Mat homography_image_to_stage_;
  // bool homography_image_to_stage_set_;
};

#endif
