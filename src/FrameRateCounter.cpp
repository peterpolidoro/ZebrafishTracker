//=============================================================================
// Copyright © 2008 Point Grey Research, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with Point Grey Research, Inc. (PGR).
//
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: FrameRateCounter.cpp,v 1.11 2009-08-26 18:30:35 soowei Exp $
//=============================================================================

#include "FrameRateCounter.h"

FrameRateCounter::FrameRateCounter( unsigned long queueLength )
{
	timer_.start();

	double seconds = timer_.elapsed().wall;

	for( unsigned int i = 0; i < queueLength; i++ )
	{
		frame_time_.push_back( seconds );
	}

	timer_.start();
}

FrameRateCounter::~FrameRateCounter()
{
}


double FrameRateCounter::GetFrameRate()
{
	const unsigned int size = frame_time_.size();

	boost::lock_guard<boost::mutex> lock(mutex_);

	const double start = frame_time_[0];
	const double end = frame_time_[size-1];
	const double diff = end - start;

	double frameRate = 1.0e9 / (diff / static_cast<double>(size-1));

	return frameRate;
}


void FrameRateCounter::SetFrameRate( double /*frameRate*/ )
{
	// Nothing to do here
}


void FrameRateCounter::Reset()
{
	const double seconds = timer_.elapsed().wall;

	boost::lock_guard<boost::mutex> lock(mutex_);

	const unsigned int queueSize = frame_time_.size();
	frame_time_.clear();

	for( unsigned int i = 0; i < queueSize; i++ )
	{
		frame_time_.push_back( seconds );
	}
}

void FrameRateCounter::NewFrame()
{
	const double seconds = timer_.elapsed().wall;

	boost::lock_guard<boost::mutex> lock(mutex_);

	frame_time_.pop_front();
	frame_time_.push_back( seconds );
}
