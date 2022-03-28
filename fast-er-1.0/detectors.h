/*

    This file is part of the FAST-ER machine learning system.
    Copyright (C) 2008  Edward Rosten and Los Alamos National Laboratory

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#ifndef DETECTORS_H
#define DETECTORS_H

#include <cvd/image.h>
#include <cvd/byte.h>
#include <vector>
#include <memory>
#include <string>

///A corner detector object which is passed a target number of corners to detect.
///@ingroup gDetect
struct DetectN
{	
	///Detect corners
	///@param i Image in which to detect corners
	///@param c Detected corners are inserted in to this container
	///@param N Number of corners to detect
	virtual void operator()(const CVD::Image<CVD::byte>& i, std::vector<CVD::ImageRef>& c, unsigned int N)const =0;
	///Destroy to object
	virtual ~DetectN(){}
};

///A corner detector object which is passed a threshold. These can be wrapped with a 
///searching algorithm to turn them in to a ::DetectN, specifically ::SearchThreshold
///@ingroup gDetect
struct DetectT
{
	///Detect corners
	///@param i Image in which to detect corners
	///@param c Detected corners are inserted in to this container
	///@param N Threshold used to detect corners
	virtual void operator()(const CVD::Image<CVD::byte>& i, std::vector<CVD::ImageRef>& c, unsigned int N)const =0;
	///Destroy to object
	virtual ~DetectT(){}
};

///Get a corner detector. The detector is read from the GVars3 database
///as the <code>detector</code> variable.
std::auto_ptr<DetectN> get_detector();

#endif
