// ----------------------------------------------------------------------------
// Calibration.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include "Configuration.h"


class Calibration
{
public:
  Calibration();

  bool recalibrate();

private:
  Configuration configuration_;
};

#endif
