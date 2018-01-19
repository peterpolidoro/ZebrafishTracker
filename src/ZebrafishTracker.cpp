// ----------------------------------------------------------------------------
// ZebrafishTracker.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "ZebrafishTracker.h"


volatile sig_atomic_t ZebrafishTracker::enabled_ = 1;

void ZebrafishTracker::interruptSignalHandler(int sig)
{
  enabled_ = 0;
}

// public
ZebrafishTracker::ZebrafishTracker()
{
  signal(SIGINT,ZebrafishTracker::interruptSignalHandler);
}

bool ZebrafishTracker::processCommandLineArgs(int argc, char * argv[])
{
  return SUCCESS;
}

bool ZebrafishTracker::importCalibrationData()
{
  boost::filesystem::path calibration_path("../ZebrafishTrackerCalibration/calibration/calibration.yml");

  try
  {
    if (boost::filesystem::exists(calibration_path))
    {
      std::cout << std::endl << "zebrafish_tracker_calibration_path = " << calibration_path << std::endl;
    }
    else
    {
      std::cerr << std::endl << "ZEBRAFISH_TRACKER_CALIBRATION_PATH: " << calibration_path << " does not exist!" << std::endl;
      return 1;
    }
  }
  catch (const boost::filesystem::filesystem_error& ex)
  {
    std::cout << ex.what() << std::endl;
  }

  cv::FileStorage calibration_fs(calibration_path.string(), cv::FileStorage::READ);
  calibration_fs["homography_image_to_stage"] >> homography_image_to_stage_;
  calibration_fs.release();

  bool got_calibration = true;
  if ((homography_image_to_stage_.rows != 3) || (homography_image_to_stage_.cols != 3))
  {
    got_calibration = false;
  }

  if (got_calibration)
  {
    std::cout << std::endl << "homography_image_to_stage = " << std::endl << homography_image_to_stage_ << std::endl;
  }

  return got_calibration;
}

bool ZebrafishTracker::connectToCamera()
{
  camera_.printLibraryInfo();

  size_t camera_count = camera_.count();
  std::cout << "Number of cameras detected: " << camera_count << std::endl;

  if (camera_count != 1)
  {
    return !SUCCESS;
  }

  size_t camera_index = 0;
  bool success = camera_.setDesiredCameraIndex(camera_index);
  if (!success)
  {
    return !SUCCESS;
  }

  success = camera_.connect();
  if (!success)
  {
    return !SUCCESS;
  }

  camera_.printCameraInfo();

  success = camera_.start();
  if (!success)
  {
    return !SUCCESS;
  }

}

// private
