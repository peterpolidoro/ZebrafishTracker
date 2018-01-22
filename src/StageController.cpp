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
  serial_port_.open(com_port_name);

  serial_port_.set_option(boost::asio::serial_port_base::baud_rate(BAUD));
  serial_port_.set_option(boost::asio::serial_port_base::character_size(CHARACTER_SIZE));
  serial_port_.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
  serial_port_.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
  serial_port_.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

  bool is_open = isOpen();

  if (is_open)
  {
    write("[getDeviceId]");

    std::string response = read();

    std::cout << std::endl << "stage_controller device_id = " << std::endl << response << std::endl;
  }

  return is_open;
}

bool StageController::disconnect()
{
  serial_port_.cancel();
  serial_port_.close();
  io_service_.stop();
  io_service_.reset();

  return true;
}

// private

bool StageController::isOpen()
{
  return serial_port_.is_open();
}

int StageController::write(const char * string)
{
  return write(std::string(string));
}

int StageController::write(const std::string & string)
{
  if (string.size() == 0)
  {
    return 0;
  }

  const std::string string_eol = string + END_OF_LINE_STRING;

  return boost::asio::write(serial_port_,boost::asio::buffer(string_eol.c_str(), string_eol.size()));
}

std::string StageController::read()
{
  char c;
  std::string result;
  for(;;)
  {
    boost::asio::read(serial_port_,boost::asio::buffer(&c,1));
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
