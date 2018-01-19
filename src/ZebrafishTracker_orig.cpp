// ----------------------------------------------------------------------------
// ZebrafishTracker
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


int main(int argc, char * argv[])
{
  BlobTracker blob_tracker;
  blob_tracker.setHomographyImageToStage(homography_image_to_stage);

  std::cout << "Running! Press ctrl-c to stop." << std::endl << std::endl;

  cv::Mat image;
  cv::Point blob_center;
  while (run_global)
  {
    success = camera.grabImage(image);
    if (success)
    {
      success = blob_tracker.findBlobCenter(image,blob_center);
    }
  }

  std::cout << std::endl << std::endl << "Stopping camera capture." << std::endl << std::endl;

  success = camera.stop();
  if (!success)
  {
    return -1;
  }

  std::cout << "Disconnecting camera." << std::endl << std::endl;

  success = camera.disconnect();
  if (!success)
  {
    return -1;
  }

  std::cout << "Goodbye!" << std::endl << std::endl;

  return 0;
}

// LibSerial::SerialStream dev;
// dev.Open("/dev/ttyACM0");
// dev.SetBaudRate(LibSerial::SerialStreamBuf::BAUD_115200);
// dev << "playTone 4000 ALL\n";

// std::cout << "Press Enter to exit..." << std::endl;
// std::cin.ignore();

// dev << "stop\n";
// dev.Close();
