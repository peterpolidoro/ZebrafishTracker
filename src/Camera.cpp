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

  config_.frame_rate = 177;
  config_.auto_exposure = false;
  config_.exposure = 0.8;
  config_.auto_shutter = false;
  config_.auto_gain = false;
  config_.gain = 0;
  config_.brightness = 0;
  config_.auto_sharpness = false;
  config_.sharpness = 1024;
  config_.auto_saturation = false;
  config_.saturation = 0;
  config_.gamma = 1.3;

  setNormalShutterSpeed();

  gpu_enabled_ = false;
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

void Camera::setDesiredCameraIndex(const size_t camera_index)
{
  camera_index_ = camera_index;
  error_ = bus_mgr_.GetCameraFromIndex(camera_index_, &guid_);
  if (error())
  {
  }
}

void Camera::connect()
{
  error_ = camera_.Connect(&guid_);
  if (error())
  {
  }
  reconfigure();
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

void Camera::start()
{
  error_ = camera_.StartCapture();
  if (error())
  {
  }
}

void Camera::enableGpu()
{
  gpu_enabled_ = true;
}

void Camera::allocateMemory()
{
  FlyCapture2::Image image;
  error_ = camera_.RetrieveBuffer(&image);
  FlyCapture2::PixelFormat pixel_format = image.GetPixelFormat();
  unsigned int rows = image.GetRows();
  unsigned int cols = image.GetCols();
  unsigned int stride = image.GetStride();
  unsigned int data_size = image.GetDataSize();
  std::cout << std::endl;
  std::cout << "pixel_format == PIXEL_FORMAT_MONO8: " << (pixel_format == FlyCapture2::PIXEL_FORMAT_MONO8) << std::endl;
  std::cout << "rows: " << rows << std::endl;
  std::cout << "cols: " << cols << std::endl;
  std::cout << "stride: " << stride << std::endl;
  std::cout << "data_size: " << data_size << std::endl;

}

void Camera::grabImage(cv::Mat & image)
{
  error_ = camera_.RetrieveBuffer(&raw_image_);
  if (error())
  {
  }
  error_ = raw_image_.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgb_image_);
  if (error())
  {
  }
  size_t row_bytes = (double)rgb_image_.GetReceivedDataSize()/(double)rgb_image_.GetRows();
  cv::Mat rgb_cv = cv::Mat(rgb_image_.GetRows(), rgb_image_.GetCols(), CV_8UC3, rgb_image_.GetData(),row_bytes);
  cv::cvtColor(rgb_cv, image, CV_BGR2GRAY);
}

void Camera::stop()
{
  error_ = camera_.StopCapture();
  if (error())
  {
  }
}

void Camera::disconnect()
{
  error_ = camera_.Disconnect();
  if (error())
  {
  }
}

float Camera::getCameraTemperature()
{
  FlyCapture2::Property property;
  property.type = FlyCapture2::TEMPERATURE;
  error_ = camera_.GetProperty(&property);
  if (error())
  {
    return 0.0;
  }
  return property.valueA / 10.0f - 273.15f;  // It returns values of 10 * K
}

void Camera::setNormalShutterSpeed()
{
  config_.shutter_speed = 0.001;
}

void Camera::setRecalibrationShutterSpeed()
{
  config_.shutter_speed = 0.005;
}

void Camera::reconfigure()
{
  error_ = camera_.GetCameraInfo(&camera_info_);
  if (error())
  {
  }
  FlyCapture2::FC2Config camera_config;
  error_ = camera_.GetConfiguration(&camera_config);
  if (error())
  {
  }

  // buffer mode
  // camera_config.numBuffers = buffer_count_;
  // camera_config.grabMode = FlyCapture2::BUFFER_FRAMES;
  // camera_config.highPerformanceRetrieveBuffer = true;

  // stream mode
  camera_config.grabMode = FlyCapture2::DROP_FRAMES;

  error_ = camera_.SetConfiguration(&camera_config);
  if (error())
  {
  }

  // Set frame rate
  setProperty(FlyCapture2::FRAME_RATE, false, config_.frame_rate);

  // Set exposure
  setProperty(FlyCapture2::AUTO_EXPOSURE, config_.auto_exposure, config_.exposure);

  // // Set sharpness
  // setProperty(FlyCapture2::SHARPNESS, config_.auto_sharpness, config_.sharpness);

  // Set saturation
  setProperty(FlyCapture2::SATURATION, config_.auto_saturation, config_.saturation);

  // Set shutter time
  double shutter = 1000.0 * config_.shutter_speed; // Needs to be in milliseconds
  setProperty(FlyCapture2::SHUTTER, config_.auto_shutter, shutter);
  config_.shutter_speed = shutter / 1000.0; // Needs to be in seconds

  // Set gain
  setProperty(FlyCapture2::GAIN, config_.auto_gain, config_.gain);

  // Set brightness
  setProperty(FlyCapture2::BRIGHTNESS, false, config_.brightness);

  // Set gamma
  setProperty(FlyCapture2::GAMMA, false, config_.gamma);
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

void Camera::setProperty(const FlyCapture2::PropertyType &type,
                         const bool &auto_set,
                         unsigned int &value_a,
                         unsigned int &value_b)
{
  FlyCapture2::PropertyInfo property_info;
  property_info.type = type;
  error_ = camera_.GetPropertyInfo(&property_info);
  if (error())
  {
  }

  if(property_info.present)
  {
    FlyCapture2::Property property;
    property.type = type;
    property.autoManualMode = (auto_set && property_info.autoSupported);
    property.absControl = false;
    property.onOff = property_info.onOffSupported;

    if(value_a < property_info.min)
    {
      value_a = property_info.min;
    }
    else if(value_a > property_info.max)
    {
      value_a = property_info.max;
    }
    if(value_b < property_info.min)
    {
      value_b = property_info.min;
    }
    else if(value_b > property_info.max)
    {
      value_b = property_info.max;
    }
    property.valueA = value_a;
    property.valueB = value_b;
    error_ = camera_.SetProperty(&property);
    if (error())
    {
    }

    // Read back setting to confirm
    error_ = camera_.GetProperty(&property);
    if (error())
    {
    }
    if(!property.autoManualMode)
    {
      value_a = property.valueA;
      value_b = property.valueB;
    }
  }
  else     // Not supported
  {
    value_a = 0;
    value_b = 0;
  }
}

void Camera::setProperty(const FlyCapture2::PropertyType &type,
                         const bool &auto_set,
                         double &value)
{
  FlyCapture2::PropertyInfo property_info;
  property_info.type = type;
  error_ = camera_.GetPropertyInfo(&property_info);
  if (error())
  {
  }

  if(property_info.present)
  {
    FlyCapture2::Property property;
    property.type = type;
    property.autoManualMode = (auto_set && property_info.autoSupported);
    property.absControl = property_info.absValSupported;
    property.onOff = property_info.onOffSupported;

    if(value < property_info.absMin)
    {
      value = property_info.absMin;
    }
    else if(value > property_info.absMax)
    {
      value = property_info.absMax;
    }
    property.absValue = value;
    error_ = camera_.SetProperty(&property);
    if (error())
    {
    }

    // Read back setting to confirm
    error_ = camera_.GetProperty(&property);
    if (error())
    {
    }
    if(!property.autoManualMode)
    {
      value = property.absValue;
    }
  }
  else     // Not supported
  {
    value = 0.0;
  }
}
