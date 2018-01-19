// ----------------------------------------------------------------------------
// Main.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <signal.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/features2d.hpp>

#include <FlyCapture2.h>
#include "Camera.h"

#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>
#include "FrameRateCounter.h"

#include "BlobTracker.h"

#include <SerialStream.h>

#include <boost/filesystem.hpp>

#include "ZebrafishTracker.h"


int main(int argc, char * argv[])
{
  ZebrafishTracker zebrafish_tracker;

  bool success;

  success = zebrafish_tracker.processCommandLineArgs(argc,argv);
  if (!success)
  {
    std::cerr << std::endl << "Unable to process command line arguments." << std::endl;
    return 1;
  }

  success = zebrafish_tracker.importCalibrationData();
  if (!success)
  {
    std::cerr << std::endl << "Unable to import calibration data." << std::endl;
    return 1;
  }

  success = zebrafish_tracker.connectToCamera();
  if (!success)
  {
    std::cerr << std::endl << "Unable to connect to camera." << std::endl;
    return 1;
  }

  return 0;
}
