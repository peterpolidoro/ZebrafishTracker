// ----------------------------------------------------------------------------
// Camera.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <iostream>
#include <sstream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <FlyCapture2.h>


class Camera
{
public:
  Camera();

  void printLibraryInfo();
  size_t count();
  bool setDesiredCamera(const size_t camera_index);
  bool connect();
  void printCameraInfo();
  bool start();
  bool grabImage(cv::Mat & image);
  bool stop();
  bool disconnect();
private:
  FlyCapture2::Error error_;
  FlyCapture2::BusManager bus_mgr_;
  size_t camera_index_;
  FlyCapture2::PGRGuid guid_;
  FlyCapture2::Camera camera_;
  FlyCapture2::CameraInfo camera_info_;
  static const size_t buffer_count_=10;
  FlyCapture2::Image raw_image_;
  FlyCapture2::Image rgb_image_;

  bool error();
  void printError();
};

#endif
