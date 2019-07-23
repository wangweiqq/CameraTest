// Copyright 2016-2017 SICK AG. All rights reserved.

/**

* @file       ImageData.h
* @brief      This file contains the class definition of ImageData.
* @details

* @author     SICK GCN Vision Lab
* @date       2018.10.11
* @version	  2.0
* @par Copyright (c):	ALL RIGHTS RESERVED.
*						SICK China Co., Ltd.
* @par History:
*   version 1.0 : 2018.6.25 for TrispectorStreamer \n
*   version 2.0 : 2018.10.11 for range3 \n

*/

#pragma once



#if 1
//#define LOAD_DLL
#ifndef LOAD_DLL 
#   define EXPORT_TO_DLL   __declspec(dllexport)
#else
#   define EXPORT_TO_DLL   __declspec(dllimport)
#endif   
#endif // 0


//#define EXPORT_TO_DLL

#include <vector>

namespace Sample
{

struct ImageData
{
	ImageData()
	{
		// Image buffer
		vecReflectance = std::vector<uint8_t>();
		vecRange = std::vector<uint16_t>();
		vecSensor = std::vector<uint8_t>();

		// Image info
		scaleX = 0.0;
		scaleY = 0.0;
		scaleZ = 0.0;

		offsetX = 0.0;
		offsetY = 0.0;
		offsetZ = 0.0;

		minZ = 0.0;
		maxZ = 0.0;
		frameId = 0;
		cols = 0;
		rows = 0;
	}

	~ImageData()
	{
		std::vector<uint8_t>().swap(vecReflectance);
		std::vector<uint16_t>().swap(vecRange);
	}

	ImageData & operator=( ImageData & ID)
	{
		this->vecReflectance	= ID.vecReflectance;
		this->vecRange			= ID.vecRange 	;  
		this->vecSensor			= ID.vecSensor;

		this->scaleX 			= ID.scaleX 	;
		this->scaleY  			= ID.scaleY  	;  
		this->scaleZ  			= ID.scaleZ  	;  
		this->offsetY 			= ID.offsetY 	;  
		this->offsetZ 			= ID.offsetZ 	;  
		this->offsetX 			= ID.offsetX 	;  
		this->minZ 				= ID.minZ 		;  
		this->maxZ 				= ID.maxZ 		;  
		this->frameId  			= ID.frameId  	;  
		this->cols				= ID.cols		;  
		this->rows				= ID.rows		;  

		return *this;
	}


	// Image buffer
	std::vector<uint8_t> vecReflectance;	//!< Intensity data, call Reflectance in range3.
	std::vector<uint16_t> vecRange;			//!< Range data.
	std::vector<uint8_t> vecSensor;			//!< Sensor image.


	float scaleX;							//!< the scale of X coordinate axis, used for convert pixel into millimeter.
	float scaleY;							//!< the scale of Y coordinate axis, used for convert pixel into millimeter.
	float scaleZ;							//!< the scale of Z coordinate axis, used for convert pixel into millimeter.

	float offsetX;							//!< the offset of X coordinate axis, used for convert pixel into millimeter.
	float offsetY;							//!< the offset of X coordinate axis, used for convert pixel into millimeter.
	float offsetZ;							//!< the offset of X coordinate axis, used for convert pixel into millimeter.

	float minZ;								//!< the lowest value of Z range.
	float maxZ;								//!< the upper value of Z range.
	uint32_t frameId;						//!< frame ID.
	int64_t cols;							//!< the cols of an image, also called width. Origin point is on the left-top.
	int64_t rows;							//!< the rows of an image, also called height. Origin point is on the left-top.
};

	
enum ErrorCode
{
	ALL_OK = 0,							//!< ALL_OK.
	NULL_REFERENCE,						//!< NULL_REFERENCE,			
	OUT_OF_MEMORY,						//!< OUT_OF_MEMORY,			
	NO_RESOURCES_ATTACHED,				//!< NO_RESOURCES_ATTACHED,	
	STEP_DOES_NOT_EXIST,				//!< STEP_DOES_NOT_EXIST,	
	STEP_NOT_IN_LICENSE,				//!< STEP_NOT_IN_LICENSE,	
	BANK_NOT_INITIALIZED,				//!< BANK_NOT_INITIALIZED,	
	EMPTY_REGION,						//!< EMPTY_REGION,			
	BAD_DATA_FORMAT,					//!< BAD_DATA_FORMAT,		
	INCOMPATIBLE_ARGUMENTS,				//!< INCOMPATIBLE_ARGUMENTS,	
	INCOMPATIBLE_RESULTS,				//!< INCOMPATIBLE_RESULTS,	
	ICON_ERROR,							//!< ICON_ERROR,				
	BAD_GRAB_STATUS,					//!< BAD_GRAB_STATUS,		
	OVER_TRIG_OR_LOST_LINES,			//!< OVER_TRIG_OR_LOST_LINES,
	VARIABLE_NAME_EMPTY,				//!< VARIABLE_NAME_EMPTY,	
	VARIABLE_DOES_NOT_EXIST,			//!< VARIABLE_DOES_NOT_EXIST,
	VARIABLE_TYPE_MISMATCH,				//!< VARIABLE_TYPE_MISMATCH,	
	VARIABLE_LENGTH_ERROR,				//!< VARIABLE_LENGTH_ERROR,	
	ALGORITHM_ERROR,					//!< ALGORITHM_ERROR,		
	FAILED_TO_COMPRESS_FILE,			//!< FAILED_TO_COMPRESS_FILE,
	FAILED_TO_DECOMPRESS_FILE,			//!< FAILED_TO_DECOMPRESS_FILE
	XML_PARSE_ERROR,					//!< XML_PARSE_ERROR,		
	INVALID_IP_ADDRESS,					//!< INVALID_IP_ADDRESS,		
	COULD_NOT_SEND_UDP_DATA,			//!< COULD_NOT_SEND_UDP_DATA,
	ACQUISITION_TIMEOUT,				//!< ACQUISITION_TIMEOUT,	
	LOGIC_ERROR,						//!< LOGIC_ERROR,			
	DEVICE_SN_ALREADY_EXISTED			//!< DEVICE_SN_ALREADY_EXISTED
};


enum STATUS
{
	IS_OK = 1,
	DISCONNECTED,
	CONNECTED,
	STARTED,
	ACQUIRING
};

/** CAM_STATUS \n

* The status values of camera.

*/
enum CAM_STATUS
{
	// all
	OK = 1,								//!< The operation has been executed successfully.
	FAILED,								//!< Some unsolved problems occur.

	// init camera
	XML_LOAD_FAILED,					//!< Invalid xml file.
	FILE_NOT_EXIST,						//!< File does not exist.
	INIT_FAILED,						//!< System initialization failed.
	INIT_OK,							//!< System initialization OK.
	
    // connect camera
	m_pTriSCam_IS_NULL,					//!< m_pTriSCam is used before initializng.
	CONNECT_CAMERA_FAILED,				//!< Connect camera failed.
	PARAMETERS_EMPRY,					//!< Parameters loaded from xml file are empty.
	CONNECT_CAMERA_OK,					//!< Connect camera OK.

	// open camera
	OPEN_CAMERA_FAILED,					//!< Open camera failed.
	OPEN_CAMERA_OK,						//!< Open camera OK.

	// get iamge
	GRAB_CAMERA_FAILED,					//!< Get images failed.
	GRAB_CAMERA_TIME_OUT,				//!< Fail to get image from buffer in limit time, it is a warning, no error.
	GRAB_CAMERA_OK,						//!< Get images OK.

	// close camera
	STOP_CAMERA_FAILED,					//!< Stop camera failed.
	STOP_CAMERA_OK,						//!< Stop camera OK.

	DISCONNECT_CAMERA_FAILED,			//!< Disconnect cameras failed.
	DISCONNECT_CAMERA_OK,				//!< Disconnect cameras OK.

	// socket
	SOC_SWA_START_UP_FAILED,			//!< Socket init failed.
	SOC_SOCKET_INVALID,					//!< Socket handle invalid.
	SOC_CONNECT_FAILED,					//!< Connecting to server failed.
	SOC_CHENK_FAILED,					//!< Socket failed.
	SOC_DONE,							//!< Socket done successfully.

};




}

