// ----------------------------------------------------------------------------
// StageController.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "StageController.h"


// public
StageController::StageController()
{
}

bool StageController::connect()
{
  dev_.Open("/dev/ttyACM0");
  dev_.SetBaudRate(LibSerial::SerialStreamBuf::BAUD_115200);
  dev_.SetCharSize(LibSerial::SerialStreamBuf::CHAR_SIZE_8);
  dev_.SetNumOfStopBits(1) ;
  dev_.SetParity(LibSerial::SerialStreamBuf::PARITY_NONE);
  dev_.SetFlowControl(LibSerial::SerialStreamBuf::FLOW_CONTROL_NONE);

  bool is_open = dev_.IsOpen();

  if (is_open)
  {
    dev_ << "[getDeviceId]\n";

    size_t read_count = 0;
    char serial_char = 0;
    while ((serial_char != '\n') && (++read_count < 1000))
    {
      dev_ >> serial_char;
      std::cout << serial_char;
    }
    std::cout << std::endl << std::endl;
  }


 return is_open;
}

bool StageController::disconnect()
{
  dev_.Close();

  return !dev_.IsOpen();
}

// private

// dev << "playTone 4000 ALL\n";

// std::cout << "Press Enter to exit..." << std::endl;
// std::cin.ignore();

// dev << "stop\n";
