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

#include <boost/filesystem.hpp>

#include <boost/asio.hpp>
#include "StageController.h"

#include "ZebrafishTracker.h"


int main(int argc, char * argv[])
{
  ZebrafishTracker zebrafish_tracker;

  bool success;

  success = zebrafish_tracker.processCommandLineArgs(argc,argv);
  if (!success)
  {
    std::cerr << std::endl << "Unable to process command line arguments." << std::endl << std::endl;
    return -1;
  }

  success = zebrafish_tracker.importCalibrationData();
  if (!success)
  {
    std::cerr << std::endl << "Unable to import calibration data." << std::endl << std::endl;
    return -1;
  }

  success = zebrafish_tracker.connectHardware();
  if (!success)
  {
    std::cerr << std::endl << "Unable to connect all hardware." << std::endl << std::endl;
    return -1;
  }

  // blocks until user presses ctrl-c
  zebrafish_tracker.run();

  success = zebrafish_tracker.disconnectHardware();
  if (!success)
  {
    std::cerr << std::endl << "Unable to disconnect all hardware." << std::endl << std::endl;
    return -1;
  }

  std::cout << std::endl << "Goodbye!" << std::endl << std::endl;

  return 0;
}
