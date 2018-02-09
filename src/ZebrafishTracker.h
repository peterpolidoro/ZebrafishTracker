// ----------------------------------------------------------------------------
// ZebrafishTracker.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _ZEBRAFISH_TRACKER_H_
#define _ZEBRAFISH_TRACKER_H_
#include <iostream>
#include <signal.h>
#include <opencv2/core.hpp>

#include "optionparser.h"
#include "Camera.h"
#include "ImageProcessor.h"
#include "StageController.h"
#include "Calibrator.h"
#include "CoordinateConverter.h"


class ZebrafishTracker
{
public:
  ZebrafishTracker();

  bool processCommandLineArgs(int argc, char * argv[]);
  bool connectHardware();
  bool disconnectHardware();
  bool findCalibration();
  void run();

private:
  const static bool SUCCESS = true;
  cv::Mat homography_image_to_stage_;
  Camera camera_;
  ImageProcessor image_processor_;
  StageController stage_controller_;
  bool stage_homed_;
  bool stage_homing_;
  Calibrator calibrator_;
  CoordinateConverter coordinate_converter_;
  bool paralyzed_;
  bool blind_;

  volatile static sig_atomic_t run_enabled_;
  static void interruptSignalHandler(int sig);

  bool connectCamera();
  bool disconnectCamera();
  bool connectStageController();
  bool disconnectStageController();
};

#endif
