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

  void recalibrate();
  bool getHomographyImageToStage(cv::Mat & homography_image_to_stage);

private:
  boost::filesystem::path calibration_repository_path_;
  bool recalibrate_;
};

#endif
