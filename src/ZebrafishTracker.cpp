//=============================================================================
// Copyright © 2017 FLIR Integrated Imaging Solutions, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: FlyCapture2Test.cpp 316528 2017-02-22 00:03:53Z alin $
//=============================================================================

#include <iostream>
#include <sstream>

#include "FlyCapture2.h"

#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>
#include "FrameRateCounter.h"


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <SerialStream.h>


void PrintBuildInfo()
{
  FlyCapture2::FC2Version fc2Version;
  FlyCapture2::Utilities::GetLibraryVersion(&fc2Version);

  std::ostringstream version;
  version << "FlyCapture2 library version: " << fc2Version.major << "."
          << fc2Version.minor << "." << fc2Version.type << "."
          << fc2Version.build;
  std::cout << version.str() << std::endl;

  std::ostringstream timeStamp;
  timeStamp << "Application build date: " << __DATE__ << " " << __TIME__;
  std::cout << timeStamp.str() << std::endl << std::endl;
}

void PrintCameraInfo(FlyCapture2::CameraInfo *pCamInfo)
{
  std::cout << std::endl;
  std::cout << "*** CAMERA INFORMATION ***" << std::endl;
  std::cout << "Serial number - " << pCamInfo->serialNumber << std::endl;
  std::cout << "Camera model - " << pCamInfo->modelName << std::endl;
  std::cout << "Camera vendor - " << pCamInfo->vendorName << std::endl;
  std::cout << "Sensor - " << pCamInfo->sensorInfo << std::endl;
  std::cout << "Resolution - " << pCamInfo->sensorResolution << std::endl;
  std::cout << "Firmware version - " << pCamInfo->firmwareVersion << std::endl;
  std::cout << "Firmware build time - " << pCamInfo->firmwareBuildTime << std::endl
       << std::endl;
}

void PrintError(FlyCapture2::Error error) { error.PrintErrorTrace(); }

void processImage(cv::Mat frame);

int RunSingleCamera(FlyCapture2::PGRGuid guid)
{
  const int k_numImages = 100;

  FlyCapture2::Error error;

  // Connect to a camera
  FlyCapture2::Camera cam;
  error = cam.Connect(&guid);
  if (error != FlyCapture2::PGRERROR_OK)
  {
    PrintError(error);
    return -1;
  }

  // Get the camera information
  FlyCapture2::CameraInfo camInfo;
  error = cam.GetCameraInfo(&camInfo);
  if (error != FlyCapture2::PGRERROR_OK)
  {
    PrintError(error);
    return -1;
  }

  PrintCameraInfo(&camInfo);

  // Get the camera configuration
  FlyCapture2::FC2Config config;
  error = cam.GetConfiguration(&config);
  if (error != FlyCapture2::PGRERROR_OK)
  {
    PrintError(error);
    return -1;
  }

  // Set the number of driver buffers used to 10.
  // config.numBuffers = 10;
  config.numBuffers = 300;
  config.grabMode = FlyCapture2::BUFFER_FRAMES;
  config.highPerformanceRetrieveBuffer = true;

  // Set the camera configuration
  error = cam.SetConfiguration(&config);
  if (error != FlyCapture2::PGRERROR_OK)
  {
    PrintError(error);
    return -1;
  }

  FrameRateCounter frame_rate_counter(k_numImages);
  frame_rate_counter.Reset();

  // Start capturing images
  error = cam.StartCapture();
  if (error != FlyCapture2::PGRERROR_OK)
  {
    PrintError(error);
    return -1;
  }

  FlyCapture2::Image rawImage;
  FlyCapture2::Image rgbImage;

  std::cout << "Capturing " << k_numImages << " images." << std::endl;
  for (int imageCnt = 0; imageCnt < k_numImages; imageCnt++)
  {
    // Retrieve an image
    error = cam.RetrieveBuffer(&rawImage);
    if (error != FlyCapture2::PGRERROR_OK)
    {
      PrintError(error);
      continue;
    }

    // std::cout << "Grabbed image " << imageCnt << std::endl;
    frame_rate_counter.NewFrame();

    // Convert the raw image
    // error = rawImage.Convert(PIXEL_FORMAT_MONO8, &rgbImage);
    error = rawImage.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage);
    if (error != FlyCapture2::PGRERROR_OK)
    {
        PrintError(error);
        return -1;
    }

    // convert to OpenCV Mat
    unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();
    cv::Mat image = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(),rowBytes);

    // convert to greyscale
    cv::Mat greyMat;
    cv::cvtColor(image, greyMat, CV_BGR2GRAY);

// // Create a unique filename

    // std::ostringstream filename;
    // filename << "FlyCapture2Test-" << camInfo.serialNumber << "-"
    //          << imageCnt << ".pgm";

    // // Save the image. If a file format is not passed in, then the file
    // // extension is parsed to attempt to determine the file format.
    // error = rgbImage.Save(filename.str().c_str());
    // if (error != PGRERROR_OK)
    // {
    //     PrintError(error);
    //     return -1;
    // }
  }

  // Stop capturing images
  error = cam.StopCapture();
  if (error != FlyCapture2::PGRERROR_OK)
  {
    PrintError(error);
    return -1;
  }

  // Disconnect the camera
  error = cam.Disconnect();
  if (error != FlyCapture2::PGRERROR_OK)
  {
    PrintError(error);
    return -1;
  }

  std::cout << "Frame rate: " << frame_rate_counter.GetFrameRate() << std::endl;

  return 0;
}

int main(int /*argc*/, char ** /*argv*/)
{
  PrintBuildInfo();

  FlyCapture2::Error error;

  // Since this application saves images in the current folder
  // we must ensure that we have permission to write to this folder.
  // If we do not have permission, fail right away.
  FILE *tempFile = fopen("test.txt", "w+");
  if (tempFile == NULL)
  {
    std::cout << "Failed to create file in current folder.  Please check "
      "permissions."
         << std::endl;
    return -1;
  }
  fclose(tempFile);
  remove("test.txt");

  FlyCapture2::BusManager busMgr;
  unsigned int numCameras;
  error = busMgr.GetNumOfCameras(&numCameras);
  if (error != FlyCapture2::PGRERROR_OK)
  {
    PrintError(error);
    return -1;
  }

  std::cout << "Number of cameras detected: " << numCameras << std::endl;

  for (unsigned int i = 0; i < numCameras; i++)
  {
    FlyCapture2::PGRGuid guid;
    error = busMgr.GetCameraFromIndex(i, &guid);
    if (error != FlyCapture2::PGRERROR_OK)
    {
      PrintError(error);
      return -1;
    }

    RunSingleCamera(guid);
  }

  LibSerial::SerialStream dev;
  dev.Open("/dev/ttyACM0");
  dev.SetBaudRate(LibSerial::SerialStreamBuf::BAUD_115200);
  dev << "playTone 4000 ALL\n";

  std::cout << "Press Enter to exit..." << std::endl;
  std::cin.ignore();

  dev << "stop\n";
  dev.Close();

  return 0;
}

void processImage(cv::Mat frame)
{
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
}
