// ----------------------------------------------------------------------------
// StageController.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _STAGE_CONTROLLER_H_
#define _STAGE_CONTROLLER_H_
#include <boost/asio.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <sstream>


class StageController
{
public:
  StageController();
  ~StageController();

  bool connect();
  bool disconnect();

  bool homeStage();
  bool stageHomed();
  bool moveStageTo(const long x, const long y);

private:
  const static std::string END_OF_LINE_STRING;
  const static long BAUD = 115200;
  const static size_t CHARACTER_SIZE = 8;

  boost::asio::io_service io_service_;
  boost::asio::serial_port serial_port_;

  bool isOpen();
	int writeRequest(const char * string);
	int writeRequest(const std::string & string);
  std::string readResponse();
  bool readBoolResponse();
};

#endif
