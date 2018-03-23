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

#include <opencv2/imgproc.hpp>

#include <FlyCapture2.h>

// #if CUDA_FOUND
#include <cuda_runtime_api.h>
#include <cuda.h>
// #endif


class Camera
{
public:
  Camera();
  ~Camera();

  void printLibraryInfo();
  size_t count();
  void setDesiredCameraIndex(const size_t camera_index);
  void connect();
  void printCameraInfo();
  void start();
  void enableGpu();
  void allocateMemory();
  unsigned int getImageDataSize();
  void grabImage(cv::Mat & image);
  void stop();
  void disconnect();
  float getCameraTemperature();
  void setNormalShutterSpeed();
  void setRecalibrationShutterSpeed();
  void reconfigure();

private:
  FlyCapture2::Error error_;
  FlyCapture2::BusManager bus_mgr_;
  size_t camera_index_;
  FlyCapture2::PGRGuid guid_;
  FlyCapture2::Camera camera_;
  FlyCapture2::CameraInfo camera_info_;
  static const size_t buffer_count_ = 1;
  FlyCapture2::Image retrieved_image_;
  FlyCapture2::Image unified_image_;

  bool gpu_enabled_;
  FlyCapture2::PixelFormat pixel_format_;
  unsigned int rows_;
  unsigned int cols_;
  unsigned int stride_;
  unsigned int image_data_size_;
  unsigned char * image_data_ptr_;

  struct Config
  {
    double frame_rate;
    bool auto_exposure;
    double exposure;
    bool auto_shutter;
    double shutter_speed;
    bool auto_gain;
    double gain;
    double brightness;
    bool auto_sharpness;
    double sharpness;
    bool auto_saturation;
    double saturation;
    double gamma;
  } config_;


  bool error();
  void printError();
  void setProperty(const FlyCapture2::PropertyType &type,
                   const bool &auto_set,
                   unsigned int &value_a,
                   unsigned int &value_b);
  void setProperty(const FlyCapture2::PropertyType &type,
                   const bool &auto_set,
                   double &value);
};

#endif
