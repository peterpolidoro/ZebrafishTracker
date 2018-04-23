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

#include "Configuration.h"
#include "Camera.h"
#include "ImageProcessor.h"
#include "StageController.h"
#include "Calibration.h"
#include "CoordinateConverter.h"


class ZebrafishTracker
{
public:
  ZebrafishTracker();

  bool processCommandLineArgs(int argc, char * argv[]);
  void connectHardware();
  void disconnectHardware();
  void allocateMemory();
  void findCalibration();
  void run();

private:
  Configuration configuration_;
  Camera camera_;
  ImageProcessor image_processor_;
  StageController stage_controller_;
  bool stage_homed_;
  bool stage_homing_;
  Calibration calibration_;
  CoordinateConverter coordinate_converter_;
  bool paralyzed_;
  bool blind_;
  bool recalibrate_;

  volatile static sig_atomic_t run_enabled_;
  static void interruptSignalHandler(int sig);

  void connectCamera();
  void disconnectCamera();
  void connectStageController();
  void disconnectStageController();
};

#endif
