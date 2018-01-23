// ----------------------------------------------------------------------------
// StageController.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _STAGE_CONTROLLER_H_
#define _STAGE_CONTROLLER_H_
#include <iostream>
#include <boost/filesystem.hpp>
#include <sstream>

#include "TimeoutSerial.h"


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
  const static std::string DEVICE_NAME;
  const static long BAUD = 115200;
  const static std::string END_OF_LINE_STRING;
  const static long TIMEOUT = 1;

  TimeoutSerial serial_;

  bool isOpen();
	void writeRequest(const char * request);
	void writeRequest(const std::string & request);
  std::string readResponse();
  bool readBoolResponse();
};

#endif
