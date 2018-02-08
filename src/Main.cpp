// ----------------------------------------------------------------------------
// Main.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include <iostream>

#include "ZebrafishTracker.h"


int main(int argc, char * argv[])
{
  ZebrafishTracker zebrafish_tracker;

  bool success;

  success = zebrafish_tracker.processCommandLineArgs(argc,argv);
  if (!success)
  {
    return -1;
  }

  success = zebrafish_tracker.connectHardware();
  if (!success)
  {
    std::cerr << std::endl << "Unable to connect all hardware." << std::endl << std::endl;
    return -1;
  }

  success = zebrafish_tracker.findCalibration();
  if (!success)
  {
    std::cerr << std::endl << "Unable to find calibration." << std::endl << std::endl;
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
