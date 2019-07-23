/**
	Grabbing-Image Class Demo File.

	- Notes:
	Before run this domo, please make sure the Ranger3Studio can connect to camera without any error.
	This file should be called by R3API_Demo_OneR3.cpp or R3API_Demo_TwoR3.cpp
	The ImageGrabber.h is needed.
	OpenCV is not needed.

	- This file:
	It is the head file of ImageGabber, contains the declaration.
	This class is desigend as an image grabbing process in an independent thread.
	One Ranger3 should correspond to one ImageGabber object.
	R3API_Demo.cpp does not contain this file.

	- For 1 Ranger3 demo, please run R3API_Demo_OneR3.cpp(recommended) in the same folder.
	- For 2 Ranger3 demo, please run R3API_Demo_TwoR3.cpp(recommended) in the same folder.

	// Copyright 2016-2018 SICK AG. All rights reserved.
	// SICK GCN Vision Lab.
*/

#pragma once

#include <Ranger3.h>

#include <memory>
#include <string>
#include <vector>


/**
	This class shows how to manage threads of Ranger3.
	It is just a sample, you should modify it in your case.
*/
class ImageGabber final
{
public:
	ImageGabber();
	ImageGabber(std::shared_ptr<R3S> & _R3S);
	ImageGabber(std::shared_ptr<R3S> & _R3S, const std::string _macAddress);

	~ImageGabber();

	bool init(const double _scaleY, const std::string & para_csv_path, const std::string & calib_xml_path, const ranger3calibration::RectificationMethod method = ranger3calibration::RectificationMethod::Top, const uint64_t timeOut=1000);

	void run();

	bool isGrabbed();
	Sample::ImgTable getImgTable();

	void requireStop();
	bool isStop();
	bool close();

private:
	std::unique_ptr<R3> m_pCam;
	std::list<Sample::ImgTable> m_listImgTable;

	bool is_start;
	bool is_require_stop;
	bool is_stop;
	bool is_grabbed;

	std::mutex MutexImageData;
	std::mutex MutexIsGrabbed;

	std::string macAddress;

	uint64_t m_timeOut;

};
