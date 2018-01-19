// ----------------------------------------------------------------------------
// StageController.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _STAGE_CONTROLLER_H_
#define _STAGE_CONTROLLER_H_
#include <SerialStream.h>
#include <iostream>
#include <opencv2/core/core.hpp>


class StageController
{
public:
  StageController();

  bool connect();
  bool disconnect();

private:
  LibSerial::SerialStream dev_;
};

#endif
