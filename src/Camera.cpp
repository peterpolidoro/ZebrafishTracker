// ----------------------------------------------------------------------------
// Camera.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Camera.h"


// public
Camera::Camera()
{
  FlyCapture2::Error error_;
  FlyCapture2::BusManager bus_mgr_;
  FlyCapture2::Camera camera_;
  FlyCapture2::CameraInfo camera_info_;
  FlyCapture2::Image raw_image_;
  FlyCapture2::Image rgb_image_;
}

void Camera::printLibraryInfo()
{
  std::cout << std::endl;

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

size_t Camera::count()
{
  unsigned int camera_count;
  error_ = bus_mgr_.GetNumOfCameras(&camera_count);
  if (error())
  {
    return 0;
  }
  return camera_count;
}

bool Camera::setDesiredCamera(const size_t camera_index)
{
  bool success = true;
  camera_index_ = camera_index;
  error_ = bus_mgr_.GetCameraFromIndex(camera_index_, &guid_);
  if (error())
  {
    return !success;
  }
  return success;
}

bool Camera::connect()
{
  bool success = true;
  error_ = camera_.Connect(&guid_);
  if (error())
  {
    return !success;
  }
  error_ = camera_.GetCameraInfo(&camera_info_);
  if (error())
  {
    return !success;
  }
  FlyCapture2::FC2Config camera_config;
  error_ = camera_.GetConfiguration(&camera_config);
  if (error())
  {
    return !success;
  }
  camera_config.numBuffers = buffer_count_;
  camera_config.grabMode = FlyCapture2::BUFFER_FRAMES;
  camera_config.highPerformanceRetrieveBuffer = true;
  error_ = camera_.SetConfiguration(&camera_config);
  if (error())
  {
    return !success;
  }
  return success;
}

void Camera::printCameraInfo()
{
  std::cout << std::endl;
  std::cout << "*** CAMERA INFORMATION ***" << std::endl;
  std::cout << "Serial number - " << camera_info_.serialNumber << std::endl;
  std::cout << "Camera model - " << camera_info_.modelName << std::endl;
  std::cout << "Camera vendor - " << camera_info_.vendorName << std::endl;
  std::cout << "Sensor - " << camera_info_.sensorInfo << std::endl;
  std::cout << "Resolution - " << camera_info_.sensorResolution << std::endl;
  std::cout << "Firmware version - " << camera_info_.firmwareVersion << std::endl;
  std::cout << "Firmware build time - " << camera_info_.firmwareBuildTime << std::endl
            << std::endl;

}

bool Camera::start()
{
  bool success = true;
  error_ = camera_.StartCapture();
  if (error())
  {
    return !success;
  }
  return success;
}

bool Camera::grabImage(cv::Mat & image)
{
  bool success = true;
  error_ = camera_.RetrieveBuffer(&raw_image_);
  if (error())
  {
    return !success;
  }
  error_ = raw_image_.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgb_image_);
  if (error())
  {
    return !success;
  }
  size_t row_bytes = (double)rgb_image_.GetReceivedDataSize()/(double)rgb_image_.GetRows();
  cv::Mat rgb_cv = cv::Mat(rgb_image_.GetRows(), rgb_image_.GetCols(), CV_8UC3, rgb_image_.GetData(),row_bytes);
  cv::cvtColor(rgb_cv, image, CV_BGR2GRAY);
  return success;
}

bool Camera::stop()
{
  bool success = true;
  error_ = camera_.StopCapture();
  if (error())
  {
    return !success;
  }
  return success;
}

bool Camera::disconnect()
{
  bool success = true;
  error_ = camera_.Disconnect();
  if (error())
  {
    return !success;
  }
  return success;
}

// private
bool Camera::error()
{
  bool error = (error_ != FlyCapture2::PGRERROR_OK);
  if (error)
  {
    printError();
  }
  return error;
}

void Camera::printError()
{
  error_.PrintErrorTrace();
}
