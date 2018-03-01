// ----------------------------------------------------------------------------
// Configuration.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Configuration.h"


boost::filesystem::path Configuration::configuration_repository_path_;
boost::filesystem::path Configuration::calibration_path_;

// public
Configuration::Configuration()
{
}

void Configuration::setConfigurationRepositoryPath(cv::String path)
{
  setConfigurationRepositoryPath(boost::filesystem::path(path));
}

bool Configuration::checkCalibrationPath()
{
  try
  {
    if (boost::filesystem::exists(calibration_path_))
    {
      return true;
    }
    else
    {
      std::cerr << std::endl << "zebrafish_tracker_calibration_path: " << calibration_path_ << " does not exist!" << std::endl;
    }
  }
  catch (const boost::filesystem::filesystem_error& ex)
  {
    std::cout << std::endl << ex.what() << std::endl;
  }
  return false;
}

bool Configuration::readHomographyImageToStage(cv::Mat & homography_image_to_stage)
{
  const bool got_calibration = true;

  if (!checkCalibrationPath())
  {
    return !got_calibration;
  }

  cv::FileStorage calibration_fs(calibration_path_.string(), cv::FileStorage::READ);
  calibration_fs["homography_image_to_stage"] >> homography_image_to_stage;
  calibration_fs.release();

  if ((homography_image_to_stage.rows != 3) || (homography_image_to_stage.cols != 3))
  {
    std::cout << "homography_image_to_stage is not a 3x3 matrix." << std::endl;
    return !got_calibration;
  }
  else
  {
    std::cout << std::endl << "homography_image_to_stage = " << std::endl << homography_image_to_stage << std::endl;
  }

  return got_calibration;
}

// private
bool Configuration::checkConfigurationRepositoryPath(boost::filesystem::path path)
{
  try
  {
    if (boost::filesystem::exists(path))
    {
      std::cout << std::endl << "zebrafish_tracker_configuration_path = " << path << std::endl;
      return true;
    }
    else
    {
      std::cerr << std::endl << "zebrafish_tracker_configuration_path: " << path << " does not exist!" << std::endl;
    }
  }
  catch (const boost::filesystem::filesystem_error& ex)
  {
    std::cout << std::endl << ex.what() << std::endl;
  }
  return false;
}

void Configuration::setConfigurationRepositoryPath(boost::filesystem::path path)
{
  if (checkConfigurationRepositoryPath(path))
  {
    configuration_repository_path_ = path;

    calibration_path_ = configuration_repository_path_;
    calibration_path_ /= "calibration/calibration.yml";
  }
}
