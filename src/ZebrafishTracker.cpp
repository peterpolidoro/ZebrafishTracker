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


volatile sig_atomic_t run_global = 1;

void interruptSignalFunction(int sig)
{
  run_global = 0;
}

bool getCalibrationData(cv::Mat & homography_image_to_stage);

int main(int argc, char * argv[])
{
  std::cout << std::endl;

  std::stringstream usage_ss;
  usage_ss << "Usage: " << argv[0] << std::endl;

  // std::stringstream usage_ss;
  // usage_ss << "Usage: " << argv[0] << " ZEBRAFISH_TRACKER_CALIBRATION_PATH" << std::endl;

  // if (argc < 2)
  // {
  //   std::cerr << usage_ss.str();
  //   return 1;
  // }

  // boost::filesystem::path calibration_path(argv[1]);

  cv::Mat homography_image_to_stage;
  bool got_calibration = getCalibrationData(homography_image_to_stage);

  if (!got_calibration)
  {
    std::cerr << "unable to read homography_image_to_stage in calibration.yml" << std::endl;
    return 1;
  }

  signal(SIGINT,interruptSignalFunction);

  Camera camera;

  camera.printLibraryInfo();

  size_t camera_count = camera.count();
  std::cout << "Number of cameras detected: " << camera_count << std::endl;

  if (camera_count != 1)
  {
    return -1;
  }

  size_t camera_index = 0;
  bool success = camera.setDesiredCameraIndex(camera_index);
  if (!success)
  {
    return -1;
  }

  success = camera.connect();
  if (!success)
  {
    return -1;
  }

  camera.printCameraInfo();

  success = camera.start();
  if (!success)
  {
    return -1;
  }

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

bool getCalibrationData(cv::Mat & homography_image_to_stage)
{
  boost::filesystem::path calibration_path("../ZebrafishTrackerCalibration/calibration/calibration.yml");

  try
  {
    if (boost::filesystem::exists(calibration_path))
    {
      std::cout << "zebrafish_tracker_calibration_path = " << calibration_path << std::endl;
    }
    else
    {
      std::cerr << "ZEBRAFISH_TRACKER_CALIBRATION_PATH: " << calibration_path << " does not exist!" << std::endl;
      return 1;
    }
  }
  catch (const boost::filesystem::filesystem_error& ex)
  {
    std::cout << ex.what() << std::endl;
  }

  cv::FileStorage calibration_fs(calibration_path.string(), cv::FileStorage::READ);
  calibration_fs["homography_image_to_stage"] >> homography_image_to_stage;
  calibration_fs.release();

  bool got_calibration = true;
  if ((homography_image_to_stage.rows != 3) || (homography_image_to_stage.cols != 3))
  {
    got_calibration = false;
  }

  if (got_calibration)
  {
    std::cout << std::endl << "homography_image_to_stage = " << std::endl << homography_image_to_stage << std::endl;
  }

  return got_calibration;
}

// LibSerial::SerialStream dev;
// dev.Open("/dev/ttyACM0");
// dev.SetBaudRate(LibSerial::SerialStreamBuf::BAUD_115200);
// dev << "playTone 4000 ALL\n";

// std::cout << "Press Enter to exit..." << std::endl;
// std::cin.ignore();

// dev << "stop\n";
// dev.Close();

// int RunSingleCamera(FlyCapture2::PGRGuid guid)
// {
// // // // Create a unique filename

// //     // std::ostringstream filename;
// //     // filename << "FlyCapture2Test-" << camInfo.serialNumber << "-"
// //     //          << imageCnt << ".pgm";

// //     // // Save the image. If a file format is not passed in, then the file
// //     // // extension is parsed to attempt to determine the file format.
// //     // error = rgbImage.Save(filename.str().c_str());
// //     // if (error != PGRERROR_OK)
// //     // {
// //     //     PrintError(error);
// //     //     return -1;
// //     // }
//   // }

//   return 0;
// }

// Since this application saves images in the current folder
// we must ensure that we have permission to write to this folder.
// If we do not have permission, fail right away.
// FILE *tempFile = fopen("test.txt", "w+");
// if (tempFile == NULL)
// {
//   std::cout << "Failed to create file in current folder.  Please check "
//     "permissions."
//        << std::endl;
//   return -1;
// }
// fclose(tempFile);
// remove("test.txt");

// void processImage(cv::Mat frame)
// {
//show the current frame and the fg masks
// imshow("Frame", frame);

// imshow("FG Mask MOG 2", fg_mask_mog_2);

//update the background model
// mog_2_ptr->apply(frame, fg_mask_mog_2);

// if (fg_mask_sum_initialized)
// {
//   add(fg_mask_sum,fg_mask_mog_2,fg_mask_sum,noArray(),CV_64F);
// }
// else
// {
//   fg_mask_sum = fg_mask_mog_2;
//   fg_mask_sum.convertTo(fg_mask_sum,CV_64F);
//   fg_mask_sum_initialized = true;
// }
//get the frame number and write it on the current frame
// stringstream ss;
// rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
//           cv::Scalar(255,255,255), -1);
// ss << capture.get(CAP_PROP_POS_FRAMES);
// string frameNumberString = ss.str();
// putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
//         FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
//show the current frame and the fg masks
// imshow("Frame", frame);
// imshow("FG Mask MOG 2", fg_mask_mog_2);
// normalize(fg_mask_sum,fg_mask_sum_normalized,0,255,NORM_MINMAX,CV_8U);
// applyColorMap(fg_mask_sum_normalized,fg_mask_sum_colored,COLORMAP_JET);

//get the input from the keyboard
// keyboard = waitKey(1);
// }
