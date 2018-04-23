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

  try
  {
    bool help = zebrafish_tracker.processCommandLineArgs(argc,argv);
    if (help)
    {
      return EXIT_SUCCESS;
    }
  }
  catch (const std::exception & e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << std::endl << "Unable to process command line arguments." << std::endl << std::endl;
    return EXIT_FAILURE;
  }

  try
  {
    zebrafish_tracker.connectHardware();
  }
  catch (const std::exception & e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << std::endl << "Unable to connect all hardware." << std::endl << std::endl;
    return EXIT_FAILURE;
  }

  try
  {
    zebrafish_tracker.findCalibration();
  }
  catch (const std::exception & e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << std::endl << "Unable to find calibration." << std::endl << std::endl;
    return EXIT_FAILURE;
  }

  try
  {
    zebrafish_tracker.allocateMemory();
  }
  catch (const std::exception & e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << std::endl << "Unable to allocate memory." << std::endl << std::endl;
    return EXIT_FAILURE;
  }

  // blocks until user presses ctrl-c
  try
  {
    zebrafish_tracker.run();
  }
  catch (const std::exception & e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << std::endl << "Exception occurred while running." << std::endl << std::endl;
    return EXIT_FAILURE;
  }

  try
  {
    zebrafish_tracker.disconnectHardware();
  }
  catch (const std::exception & e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << std::endl << "Unable to disconnect all hardware." << std::endl << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << std::endl << "Goodbye!" << std::endl << std::endl;

  return EXIT_SUCCESS;
}
