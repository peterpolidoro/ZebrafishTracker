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
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <boost/filesystem.hpp>


class Calibrator
{
public:
  Calibrator();

  void setConfigurationRepositoryPath(boost::filesystem::path path);
  void recalibrate();
  bool getHomographyImageToStage(cv::Mat & homography_image_to_stage);

private:
  boost::filesystem::path configuration_repository_path_;
  bool recalibrate_;

  bool calibrate(const boost::filesystem::path calibration_path);
};

#endif
