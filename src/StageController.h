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
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <math.h>

#include "TimeoutSerial.h"


class StageController
{
public:
  StageController();
  ~StageController();

  bool connect();
  bool disconnect();

  void setDebug(const bool debug);

  bool homeStage();
  bool stageHomed();
  bool moveStageTo(const long x, const long y);

private:
  const static std::string DEVICE_NAME;
  const static long BAUD = 115200;
  const static std::string END_OF_LINE_STRING;
  const static long TIMEOUT = 1;
  const static size_t READ_ATTEMPTS_MAX = 10;
  const static size_t WRITE_READ_DELAY = 5;
  const static long DEADBAND = 4000;

  TimeoutSerial serial_;
  bool debug_;
  long x_prev_;
  long y_prev_;

  bool isOpen();
  void writeRequest(const char * request);
  void writeRequest(const std::string & request);
  std::string readResponse();
  bool readBoolResponse();
  std::string writeRequestReadResponse(const std::string & request);
  bool writeRequestReadBoolResponse(const std::string & request);
};

#endif
