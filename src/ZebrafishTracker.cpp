// ----------------------------------------------------------------------------
// ZebrafishTracker.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "ZebrafishTracker.h"


volatile sig_atomic_t ZebrafishTracker::run_enabled_ = 1;

void ZebrafishTracker::interruptSignalHandler(int sig)
{
  run_enabled_ = 0;
}

// public
ZebrafishTracker::ZebrafishTracker()
{
  signal(SIGINT,ZebrafishTracker::interruptSignalHandler);

  configuration_repository_path_ = boost::filesystem::path("../ZebrafishTrackerConfiguration");

  calibrator_.setConfigurationRepositoryPath(configuration_repository_path_);
  image_processor_.setMode(ImageProcessor::BLOB);

  stage_homed_ = false;
  stage_homing_ = false;
  paralyzed_ = false;
  blind_ = false;
  recalibrate_ = false;
}

bool ZebrafishTracker::processCommandLineArgs(int argc, char * argv[])
{
  const cv::String keys =
    "{help h usage ? |      | Print usage and exit.                              }"
    "{d debug        |      | Print debug info.                                  }"
    "{m mouse        |      | Track mouse click location instead of blob.        }"
    "{p paralyze     |      | Do not communicate with stage so it does not move. }"
    "{b blind        |      | Do not communicate with camera.                    }"
    "{r recalibrate  |      | Recalibrate with chessboard before running.        }"
    ;

  cv::CommandLineParser parser(argc,argv,keys);

  if (!parser.check())
  {
    parser.printErrors();
    return !SUCCESS;
  }

  if (parser.has("help"))
  {
    parser.printMessage();
    return !SUCCESS;
  }

  if (parser.has("debug"))
  {
    stage_controller_.setDebug(true);
    std::cout << std::endl << "Debug mode!" << std::endl;
  }

  if (parser.has("mouse"))
  {
    image_processor_.setMode(ImageProcessor::MOUSE);
    std::cout << std::endl << "Mouse mode!" << std::endl;
  }

  if (parser.has("paralyze"))
  {
    paralyzed_ = true;
    std::cout << std::endl << "Paralyzed!" << std::endl;
  }

  if (parser.has("blind"))
  {
    blind_ = true;
    std::cout << std::endl << "Blind!" << std::endl;
  }

  if (parser.has("recalibrate"))
  {
    recalibrate_ = true;
    std::cout << std::endl << "Recalibrate!" << std::endl;
  }

  return SUCCESS;
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

  return success;
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

  return success;
}

bool ZebrafishTracker::findCalibration()
{
  if (recalibrate_)
  {
    camera_.setRecalibrationShutterSpeed();
    camera_.reconfigure();
    calibrator_.recalibrate();
  }

  bool got_calibration = calibrator_.getHomographyImageToStage(homography_image_to_stage_);

  if ((homography_image_to_stage_.rows != 3) || (homography_image_to_stage_.cols != 3))
  {
    got_calibration = false;
    std::cout << "wrong size????" << std::endl;
  }

  if (got_calibration)
  {
    std::cout << std::endl << "homography_image_to_stage = " << std::endl << homography_image_to_stage_ << std::endl;
    coordinate_converter_.setHomographyImageToStage(homography_image_to_stage_);
  }

  if (recalibrate_)
  {
    camera_.setNormalShutterSpeed();
    camera_.reconfigure();
  }

  return got_calibration;
}

void ZebrafishTracker::run()
{
  std::cout << std::endl << "Running! Press ctrl-c to stop." << std::endl << std::endl;

  cv::Mat image;
  cv::Point tracked_image_point;
  cv::Point stage_target_position;
  bool success;
  while(run_enabled_)
  {
    if (!blind_)
    {
      success = camera_.grabImage(image);
    }
    if (success)
    {
      success = image_processor_.updateTrackedImagePoint(image);
    }
    if (success)
    {
      success = image_processor_.getTrackedImagePoint(tracked_image_point);
    }
    if (success)
    {
      success = coordinate_converter_.convertImagePointToStagePoint(tracked_image_point,stage_target_position);
    }
    if (success && !paralyzed_)
    {
      if (stage_homed_)
      {
        success = stage_controller_.moveStageTo(stage_target_position.x,stage_target_position.y);
      }
      else if (stage_homing_)
      {
        stage_homed_ = stage_controller_.stageHomed();
        stage_homing_ = !stage_homed_;
      }
      else
      {
        stage_controller_.homeStage();
        stage_homing_ = true;
      }
    }
  }
}

// private
bool ZebrafishTracker::connectCamera()
{
  if (blind_)
  {
    return true;
  }

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

  return success;
}

bool ZebrafishTracker::disconnectCamera()
{
  if (blind_)
  {
    return true;
  }

  std::cout << std::endl << "Stopping camera capture." << std::endl;

  bool success;
  success = camera_.stop();
  if (!success)
  {
    return !SUCCESS;
  }

  std::cout << std::endl << "Disconnecting camera." << std::endl;

  success = camera_.disconnect();

  return success;
}

bool ZebrafishTracker::connectStageController()
{
  if (paralyzed_)
  {
    return true;
  }

  std::cout << std::endl << "Connecting stage controller." << std::endl;

  bool success;
  success = stage_controller_.connect();

  return success;
}

bool ZebrafishTracker::disconnectStageController()
{
  if (paralyzed_)
  {
    return true;
  }

  std::cout << std::endl << "Disconnecting stage controller." << std::endl;

  bool success;
  success = stage_controller_.disconnect();

  return success;
}
