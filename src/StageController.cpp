// ----------------------------------------------------------------------------
// StageController.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "StageController.h"


const std::string StageController::END_OF_LINE_STRING = std::string("\n");

// public
StageController::StageController() :
  io_service_(),
  serial_port_(io_service_)
{
}

StageController::~StageController()
{
  disconnect();
}

bool StageController::connect()
{
  const char * com_port_name = "/dev/ttyACM0";
  boost::filesystem::path com_port_path(com_port_name);
  if (!boost::filesystem::exists(com_port_path))
  {
    std::cout << std::endl << com_port_name << " does not exist! Is the stage_controller attached?" << std::endl;
    return false;
  }

  boost::system::error_code ec;
  serial_port_.open(com_port_name,ec);
  if (ec)
  {
    std::cout << "error : serial_port_->open() failed...com_port_name="
              << com_port_name << ", e=" << ec.message().c_str() << std::endl;
    return false;
  }

  serial_port_.set_option(boost::asio::serial_port_base::baud_rate(BAUD));
  serial_port_.set_option(boost::asio::serial_port_base::character_size(CHARACTER_SIZE));
  serial_port_.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
  serial_port_.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
  serial_port_.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

  bool is_open = isOpen();

  if (is_open)
  {
    // std::string flushed = readResponse();

    writeRequest("[getDeviceId]");

    std::string response = readResponse();

    const char * device_name = "zebrafish_tracker_controller";
    size_t position = response.find(device_name);

    if (position == std::string::npos)
    {
      std::cout << "response = " << std::endl;
      std::cout << response << std::endl;
      std::cout << std::endl << device_name << " not found in device_id response!" << std::endl;
      std::cout << "Is the stage_controller attached to " << com_port_name << "?" << std::endl;
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
    serial_port_.cancel();
    serial_port_.close();
  }
  io_service_.stop();
  io_service_.reset();

  return true;
}

bool StageController::homeStage()
{
  writeRequest("[homeStage]");

  return readBoolResponse();
}

bool StageController::stageHomed()
{
  writeRequest("[stageHomed]");

  return readBoolResponse();
}

bool StageController::moveStageTo(const long x, const long y)
{
  // std::stringstream request;
  // request << "[moveStageTo [" << x << "," << y << "]";
  // std::cout << request.str() << std::endl;
  // return true;
  // writeRequest(request.str());

  // return readBoolResponse();
}

// private

bool StageController::isOpen()
{
  return serial_port_.is_open();
}

int StageController::writeRequest(const char * string)
{
  return writeRequest(std::string(string));
}

int StageController::writeRequest(const std::string & string)
{
  if (string.size() == 0)
  {
    return 0;
  }

  const std::string string_eol = string + END_OF_LINE_STRING;

  boost::system::error_code ec;

  int bytes_written = boost::asio::write(serial_port_,boost::asio::buffer(string_eol.c_str(), string_eol.size()),ec);
  if (ec)
  {
    std::cout << "error : writing to serial_port_" << ", e=" << ec.message().c_str() << std::endl;
    return false;
  }
  return bytes_written;
}

std::string StageController::readResponse()
{
  boost::system::error_code ec;

  char c;
  std::string result;
  for(;;)
  {
    try
    {
      boost::asio::read(serial_port_,boost::asio::buffer(&c,1),ec);
    }
    catch (boost::system::system_error & e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      return result;
    }
    // boost::asio::read(serial_port_,boost::asio::buffer(&c,1),ec);
    // if (ec)
    // {
    //   std::cout << "error : reading from serial_port_" << ", e=" << ec.message().c_str() << std::endl;
    //   break;
    // }
    switch(c)
    {
      case '\r':
        break;
      case '\n':
        return result;
      default:
        result+=c;
    }
  }
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
