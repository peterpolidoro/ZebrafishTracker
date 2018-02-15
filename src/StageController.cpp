// ----------------------------------------------------------------------------
// StageController.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "StageController.h"


const std::string StageController::DEVICE_NAME = std::string("/dev/ttyACM0");
const std::string StageController::END_OF_LINE_STRING = std::string("\n");

// public
StageController::StageController()
{
  debug_ = false;
}

StageController::~StageController()
{
  disconnect();
}

bool StageController::connect()
{
  boost::filesystem::path com_port_path(DEVICE_NAME);
  if (!boost::filesystem::exists(com_port_path))
  {
    std::cout << std::endl << DEVICE_NAME << " does not exist! Is the stage_controller attached?" << std::endl;
    return false;
  }
  std::cout << std::endl << DEVICE_NAME << " exists." << std::endl;

  serial_.open(DEVICE_NAME,BAUD);
  serial_.setTimeout(boost::posix_time::seconds(TIMEOUT));
  serial_.flush();

  bool is_open = isOpen();

  if (is_open)
  {
    std::cout << DEVICE_NAME << " is open." << std::endl;

    std::string response = writeRequestReadResponse("[getDeviceId]");

    const char * device_name = "zebrafish_tracker_controller";
    size_t position = response.find(device_name);

    if (position == std::string::npos)
    {
      std::cout << response << std::endl;
      std::cout << std::endl << device_name << " not found in device_id response!" << std::endl;
      std::cout << "Is the stage_controller attached to " << DEVICE_NAME << "?" << std::endl;
      return false;
    }

    std::cout << std::endl << "stage_controller device_id = " << std::endl << response << std::endl;
  }

  return is_open;
}

bool StageController::disconnect()
{
  if (isOpen())
  {
    serial_.close();
  }

  return !isOpen();
}

void StageController::setDebug(const bool debug)
{
  debug_ = debug;
}

bool StageController::homeStage()
{
  x_prev_ = 0;
  y_prev_ = 0;
  return writeRequestReadBoolResponse("[homeStage]");
}

bool StageController::stageHomed()
{
  writeRequestReadBoolResponse("[stageHomed]");
}

bool StageController::moveStageTo(const long x, const long y)
{
  double dist = sqrt(pow((x - x_prev_),2) + pow((y - y_prev_),2));
  if (dist < (DEADBAND/2))
  {
    return false;
  }
  x_prev_ = x;
  y_prev_ = y;
  std::stringstream request;
  request << "[moveStageTo [" << x << "," << y << "]]";
  return writeRequestReadBoolResponse(request.str());
}

// private

bool StageController::isOpen()
{
  return serial_.isOpen();
}

void StageController::writeRequest(const char * request)
{
  writeRequest(std::string(request));
}

void StageController::writeRequest(const std::string & request)
{
  if (request.size() == 0)
  {
    return;
  }

  const std::string string = request + END_OF_LINE_STRING;

  if (debug_)
  {
    std::cout << request << std::endl;
  }
  serial_.writeString(string);
}

std::string StageController::readResponse()
{
  std::string response;
  size_t read_attempts = 0;
  while (read_attempts++ < READ_ATTEMPTS_MAX)
  {
    try
    {
      response = serial_.readStringUntil(END_OF_LINE_STRING);
      if (debug_)
      {
        std::cout << response << std::endl;
      }
      return response;
    }
    catch (const std::exception & e)
    {
    }
  }
  std::cerr << "Error while attempting to read response. read_attempts: " << read_attempts << std::endl;
  return response;
}

bool StageController::readBoolResponse()
{
  std::string response = readResponse();

  size_t position = response.find("true");

  if (position != std::string::npos)
  {
    return true;
  }

  return false;
}

std::string StageController::writeRequestReadResponse(const std::string & request)
{
  writeRequest(request);
  boost::this_thread::sleep(boost::posix_time::milliseconds(WRITE_READ_DELAY));
  return readResponse();
}

bool StageController::writeRequestReadBoolResponse(const std::string & request)
{
  writeRequest(request);
  boost::this_thread::sleep(boost::posix_time::milliseconds(WRITE_READ_DELAY));
  return readBoolResponse();
}
