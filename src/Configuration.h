// ----------------------------------------------------------------------------
// Configuration.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_
#include <iostream>
#include <opencv2/core.hpp>
#include <boost/filesystem.hpp>


class Configuration
{
public:
  Configuration();

  void setConfigurationRepositoryPath(cv::String path);

  bool checkCalibrationPath();
  bool readHomographyImageToStage(cv::Mat & homography_image_to_stage);

private:
  static boost::filesystem::path configuration_repository_path_;
  static boost::filesystem::path calibration_path_;

  static bool checkConfigurationRepositoryPath(boost::filesystem::path path);
  static void setConfigurationRepositoryPath(boost::filesystem::path path);

};

#endif
