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

  config_.frame_rate = 163;
  config_.auto_exposure = false;
  config_.exposure = 0.8;
  config_.auto_shutter = false;
  config_.shutter_speed = 0.006;
  config_.auto_gain = false;
  config_.gain = 27;
  config_.brightness = 0;
  config_.auto_sharpness = false;
  config_.sharpness = 1024;
  config_.auto_saturation = false;
  config_.saturation = 0;
  config_.gamma = 1.3;

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

bool Camera::setDesiredCameraIndex(const size_t camera_index)
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
  success = configure();
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

// private
bool Camera::error()
{
  bool error = (error_ != FlyCapture2::PGRERROR_OK);
  if (error)
  {
    printError();}
  return error;
}

void Camera::printError()
{
  error_.PrintErrorTrace();
}

bool Camera::configure()
{
  bool success = true;
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

  // Set frame rate
  success &= setProperty(FlyCapture2::FRAME_RATE, false, config_.frame_rate);

  // Set exposure
  success &= setProperty(FlyCapture2::AUTO_EXPOSURE, config_.auto_exposure, config_.exposure);

  // // Set sharpness
  // success &= setProperty(FlyCapture2::SHARPNESS, config_.auto_sharpness, config_.sharpness);

  // Set saturation
  success &= setProperty(FlyCapture2::SATURATION, config_.auto_saturation, config_.saturation);

  // Set shutter time
  double shutter = 1000.0 * config_.shutter_speed; // Needs to be in milliseconds
  success &= setProperty(FlyCapture2::SHUTTER, config_.auto_shutter, shutter);
  config_.shutter_speed = shutter / 1000.0; // Needs to be in seconds

  // Set gain
  success &= setProperty(FlyCapture2::GAIN, config_.auto_gain, config_.gain);

  // Set brightness
  success &= setProperty(FlyCapture2::BRIGHTNESS, false, config_.brightness);

  // Set gamma
  success &= setProperty(FlyCapture2::GAMMA, false, config_.gamma);

 return success;
}

bool Camera::setProperty(const FlyCapture2::PropertyType &type,
                         const bool &auto_set,
                         unsigned int &value_a,
                         unsigned int &value_b)
{
  // return true if we can set values as desired.
  bool success = true;

  FlyCapture2::PropertyInfo property_info;
  property_info.type = type;
  error_ = camera_.GetPropertyInfo(&property_info);
  if (error())
  {
    return false;
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
      success &= false;
    }
    else if(value_a > property_info.max)
    {
      value_a = property_info.max;
      success &= false;
    }
    if(value_b < property_info.min)
    {
      value_b = property_info.min;
      success &= false;
    }
    else if(value_b > property_info.max)
    {
      value_b = property_info.max;
      success &= false;
    }
    property.valueA = value_a;
    property.valueB = value_b;
    error_ = camera_.SetProperty(&property);
    if (error())
    {
      return false;
    }

    // Read back setting to confirm
    error_ = camera_.GetProperty(&property);
    if (error())
    {
      return false;
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

  return success;
}

bool Camera::setProperty(const FlyCapture2::PropertyType &type,
                         const bool &auto_set,
                         double &value)
{
  // return true if we can set values as desired.
  bool success = true;

  FlyCapture2::PropertyInfo property_info;
  property_info.type = type;
  error_ = camera_.GetPropertyInfo(&property_info);
  if (error())
  {
    return false;
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
      success &= false;
    }
    else if(value > property_info.absMax)
    {
      value = property_info.absMax;
      success &= false;
    }
    property.absValue = value;
    error_ = camera_.SetProperty(&property);
    if (error())
    {
      return false;
    }

    // Read back setting to confirm
    error_ = camera_.GetProperty(&property);
    if (error())
    {
      return false;
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
  return success;
}
