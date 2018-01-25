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

  image_processor_.setMode(ImageProcessor::BLOB);
}

bool ZebrafishTracker::processCommandLineArgs(int argc, char * argv[])
{
  enum optionIndex { HELP, MOUSE };
  const option::Descriptor usage[] =
    {
      {HELP, 0,"", "help", option::Arg::None, "  --help  \tPrint usage and exit." },
      {MOUSE, 0,"", "mouse", option::Arg::None, "  --mouse  \tTrack mouse click location instead of blob." },
      {0, 0, 0, 0, 0, 0}
    };
  argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
  option::Stats  stats(usage, argc, argv);
  std::vector<option::Option> options(stats.options_max);
  std::vector<option::Option> buffer(stats.buffer_max);
  option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

  if (parse.error() || options[HELP])
  {
    option::printUsage(std::cout, usage);
    return !SUCCESS;
  }

  if (options[MOUSE])
  {
    image_processor_.setMode(ImageProcessor::MOUSE);
    std::cout << "Mouse mode!" << std::endl;
  }

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
    std::cout << std::endl << ex.what() << std::endl;
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
    coordinate_converter_.setHomographyImageToStage(homography_image_to_stage_);
  }

  return got_calibration;
}

bool ZebrafishTracker::connectHardware()
{
  bool success;
  success = connectStageController();
  if (!success)
  {
    std::cerr << std::endl << "Unable to connect stage controller." << std::endl;
    return !SUCCESS;
  }

  success = connectCamera();
  if (!success)
  {
    std::cerr << std::endl << "Unable to connect camera." << std::endl;
    return !SUCCESS;
  }
}

bool ZebrafishTracker::disconnectHardware()
{
  bool success;
  success = disconnectCamera();
  if (!success)
  {
    std::cerr << std::endl << "Unable to disconnect camera." << std::endl;
    return !SUCCESS;
  }

  success = disconnectStageController();
  if (!success)
  {
    std::cerr << std::endl << "Unable to disconnect stage controller." << std::endl;
    return !SUCCESS;
  }
}

void ZebrafishTracker::run()
{
  std::cout << std::endl << "Running! Press ctrl-c to stop." << std::endl << std::endl;

  cv::Mat image;
  cv::Point tracked_image_point;
  cv::Point stage_target_position;
  bool success;
  while(enabled_)
  {
    success = camera_.grabImage(image);
    if (success)
    {
      success = image_processor_.updateTrackedImagePoint(image,&tracked_image_point);
    }
    if (success)
    {
      success = coordinate_converter_.convertImagePointToStagePoint(tracked_image_point,stage_target_position);
    }
    if (success)
    {
      success = stage_controller_.moveStageTo(stage_target_position.x,stage_target_position.y);
    }
  }
}

// private
bool ZebrafishTracker::connectCamera()
{
  camera_.printLibraryInfo();

  size_t camera_count = camera_.count();
  std::cout << std::endl << "Number of cameras detected: " << camera_count << std::endl;

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

bool ZebrafishTracker::disconnectCamera()
{
  std::cout << std::endl << "Stopping camera capture." << std::endl;

  bool success;
  success = camera_.stop();
  if (!success)
  {
    return !SUCCESS;
  }

  std::cout << std::endl << "Disconnecting camera." << std::endl;

  success = camera_.disconnect();
  if (!success)
  {
    return !SUCCESS;
  }
}

bool ZebrafishTracker::connectStageController()
{
  std::cout << std::endl << "Connecting stage controller." << std::endl;

  bool success;
  success = stage_controller_.connect();
  return success;
}

bool ZebrafishTracker::disconnectStageController()
{
  std::cout << std::endl << "Disconnecting stage controller." << std::endl;

  bool success;
  success = stage_controller_.disconnect();
  return success;
}
