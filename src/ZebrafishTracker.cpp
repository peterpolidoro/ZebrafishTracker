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

  image_processor_.setMode(ImageProcessor::BLOB);

  stage_homed_ = false;
  stage_homing_ = false;
  paralyzed_ = false;
  blind_ = false;
  recalibrate_ = false;
}

void ZebrafishTracker::processCommandLineArgs(int argc, char * argv[])
{
  const cv::String keys =
    "{help h usage ?  |                                   | Print usage and exit.                              }"
    "{c configuration |  ../ZebrafishTrackerConfiguration | Configuration repository path.                     }"
    "{d debug         |                                   | Print debug info.                                  }"
    "{m mouse         |                                   | Track mouse click location instead of blob.        }"
    "{p paralyze      |                                   | Do not communicate with stage so it does not move. }"
    "{b blind         |                                   | Do not communicate with camera.                    }"
    "{r recalibrate   |                                   | Recalibrate with chessboard before running.        }"
    ;

  cv::CommandLineParser parser(argc,argv,keys);

  if (!parser.check())
  {
    parser.printErrors();
    throw std::runtime_error("Command line parser error.");
  }

  if (parser.has("help"))
  {
    parser.printMessage();
    return;
  }

  cv::String configuration_path = parser.get<cv::String>("configuration");
  configuration_.setConfigurationRepositoryPath(configuration_path);

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
}

void ZebrafishTracker::connectHardware()
{
  connectStageController();
  connectCamera();
}

void ZebrafishTracker::disconnectHardware()
{
  disconnectCamera();
  disconnectStageController();
}

void ZebrafishTracker::allocateMemory()
{
  int cuda_device_count = cv::cuda::getCudaEnabledDeviceCount();

  gpu_enabled_ = (cuda_device_count > 0);
  std::cout << std::endl << "gpu enabled: " << gpu_enabled_ << std::endl;

  if (blind_)
  {
    return;
  }

  if (gpu_enabled_)
  {
    camera_.enableGpu();
    image_processor_.enableGpu();
  }
  camera_.allocateMemory();
  image_processor_.allocateMemory();
}

void ZebrafishTracker::findCalibration()
{
  if (recalibrate_)
  {
    camera_.setRecalibrationShutterSpeed();
    camera_.reconfigure();
    calibration_.recalibrate();
    camera_.setNormalShutterSpeed();
    camera_.reconfigure();
  }

  coordinate_converter_.updateHomographyImageToStage();
}

void ZebrafishTracker::run()
{
  std::cout << std::endl << "Running! Press ctrl-c to stop." << std::endl << std::endl;

  cv::Mat image;
  cv::Point tracked_image_point;
  cv::Point stage_target_position;
  while(run_enabled_ && !blind_)
  {
    camera_.grabImage(image);
    image_processor_.updateTrackedImagePoint(image);
    image_processor_.getTrackedImagePoint(tracked_image_point);
    coordinate_converter_.convertImagePointToStagePoint(tracked_image_point,stage_target_position);
    if (!paralyzed_)
    {
      if (stage_homed_)
      {
        stage_controller_.moveStageTo(stage_target_position.x,stage_target_position.y);
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
void ZebrafishTracker::connectCamera()
{
  if (blind_)
  {
    return;
  }

  camera_.printLibraryInfo();

  size_t camera_count = camera_.count();
  std::cout << std::endl << "Number of cameras detected: " << camera_count << std::endl;

  size_t camera_index = 0;
  camera_.setDesiredCameraIndex(camera_index);

  camera_.connect();

  camera_.printCameraInfo();

  camera_.start();
}

void ZebrafishTracker::disconnectCamera()
{
  if (blind_)
  {
    return;
  }

  std::cout << std::endl << "Stopping camera capture." << std::endl;
  camera_.stop();

  std::cout << std::endl << "Disconnecting camera." << std::endl;
  camera_.disconnect();
}

void ZebrafishTracker::connectStageController()
{
  if (paralyzed_)
  {
    return;
  }

  std::cout << std::endl << "Connecting stage controller." << std::endl;
  stage_controller_.connect();
}

void ZebrafishTracker::disconnectStageController()
{
  if (paralyzed_)
  {
    return;
  }

  std::cout << std::endl << "Disconnecting stage controller." << std::endl;
  stage_controller_.disconnect();
}
