/**
	API DLL File.

	- Notes:
	Before run this domo, please make sure the Ranger3Studio can connect to camera without any error.
	This file should be included in SampleApiDll.

	- This file:
	To generate DLL.
	Class DeviceConnection: Encapsulate the device info. and settings.
	Class ImgTable: The data struct of image.
	Class Ranger3Shared: Shared part of Ranger3s.
	Class Ranger3: Each Ranger3 corresponding to a Ranger3 object.

	- For a simple demo, please run R3API_Demo.cpp in the SampleApiDllTestProject folder.
	- For 1 Ranger3 demo, please run R3API_Demo_OneR3.cpp in the SampleApiDllTestProject folder.
	- For 2 Ranger3 demo, please run R3API_Demo_TwoR3.cpp in the SampleApiDllTestProject folder.

	// Copyright 2016-2018 SICK AG. All rights reserved.
	// SICK GCN Vision Lab.
*/

#pragma once
#pragma warning(disable:4251)


#include "BufferInfoVLB.h"
#include "ranger3runtimecalibration-0.4.0/Ranger3RuntimeCalibration/CalibrationFilter.h"
#include "ranger3runtimecalibration-0.4.0/Ranger3RuntimeCalibration/RectificationFilter.h"
#include "ChunkAdapter.h"
#include "Consumer.h"
#include "GenIRanger.h"
#include "SampleUtils.h"

#include <string>
#include <memory>
#include <set>
#include <map>
#include <thread>
#include <mutex>


#define PARA_IN					//!< Input parameter of a function.
#define PARA_OUT				//!< Output parameter of a function.
#define PARA_IN_OUT				//!< Input and output parameter of a function.



#ifdef GENE_DLL
#   define EXPORT_TO_DLL   __declspec(dllexport)
#else
#   define EXPORT_TO_DLL   __declspec(dllimport)
#endif  


namespace Sample
{

// Copy from MultiCamera.cpp
//

/** Represents the Area of interest on the sensor, in pixel
	coordinates. This information is required to be able to perform
	proper calibration
*/
struct Aoi
{
	Aoi(uint16_t offsetX, uint16_t offsetY, uint16_t width, uint16_t height)
		: mOffsetX(offsetX)
		, mOffsetY(offsetY)
		, mWidth(width)
		, mHeight(height)
	{}

	uint16_t mOffsetX;
	uint16_t mOffsetY;
	uint16_t mWidth;
	uint16_t mHeight;
};


class DeviceConnection
{
public:
	DeviceConnection(GenTLApi* tl,	GenTL::DEV_HANDLE deviceHandle,	GenTL::DS_HANDLE dataStreamHandle,	std::string deviceName)
		: mTl(tl), mAcquisitionRunning(false), mDeviceHandle(deviceHandle), mDataStreamHandle(dataStreamHandle), mDeviceName(deviceName), mAoi(0, 0, 0, 0)
	{
		// Create log file for saving buffer information
		//std::string logDir = "acquisition_log-" + mDeviceName;
		//CreateDirectoryA(logDir.c_str(), nullptr);
		//mLog.exceptions(std::ios::failbit | std::ios::badbit);

		//std::string logFilename
		//	= logDir + "/log-" + Sample::currentDateTime() + ".txt";

		//mLogName = logFilename; // 

		//mLog.open(logFilename, std::ios_base::app);
		//mLog << "Buffer acquisition statistics Start" << std::endl
		//	<< "Acquisition num; Buffer frame id; Incomplete; Bytes filled; "
		//	<< "Buffer size" << std::endl;
	}

	void createDeviceNodeMap(Sample::Consumer& consumer);
	void createDataStreamNodeMap(Sample::Consumer& consumer);

	void initializeBuffers(size_t buffersCount,	size_t payloadSize);
	void teardownBuffers();

	void startAcquisition();
	void stopAcquisition();
	bool isAcquisitionRunning() const;

	void registerNewBufferEvent();
	void unregisterNewBufferEvent();

	std::vector<GenTL::BUFFER_HANDLE>& getBufferHandles()
	{
		return mBufferHandles;
	}

	std::vector<uint8_t*>& getBufferData()
	{
		return mBufferData;
	}

private:
	std::vector<GenTL::BUFFER_HANDLE> mBufferHandles;
	std::vector<uint8_t*> mBufferData;
	GenTLApi* mTl; 
	std::unique_ptr<Sample::GenTLPort> mDevicePort;
	std::unique_ptr<Sample::GenTLPort> mDataStreamPort;
	bool mAcquisitionRunning;

public:
	GenTL::DEV_HANDLE mDeviceHandle;
	GenTL::DS_HANDLE mDataStreamHandle;
	GenTL::EVENT_HANDLE mNewBufferEventHandle;
	std::string mDeviceName;
	std::ofstream mLog;
	Aoi mAoi; // might be useless
	GenApi::CNodeMapRef mDeviceNodeMap;		//! < Node map for reading/setting device parameters
	GenApi::CNodeMapRef mDataStreamNodeMap; //! < Node map for data stream module to boost receiver thread priority
	std::string mLogName;					//! < log file name.

};

class EXPORT_TO_DLL ImgTable
{
public:
	ImgTable();

	ImgTable(float* _prange, float* _pref, size_t _c, size_t _r,
		double _offsetX, double _offsetY, double _offsetZ,
		double _scaleX, double _scaleY, double _scaleZ);

	ImgTable(uint8_t * _prange, uint8_t * _pref, size_t _c, size_t _r, 
		double _offsetX, double _offsetY, double _offsetZ, 
		double _scaleX, double _scaleY, double _scaleZ);

	~ImgTable();

	bool isEmpty();

	std::vector<float> range;		//! < The data for the range. Each point in the array represents that points z-coordinate. This is usually height after calibration and rectification 
	std::vector<float> reflectance; //! < Contains the rectified reflectance.

	size_t cols; //! < width
	size_t rows; //! < height

	double offsetX;
	double offsetY; //! < always be 0, Not used
	double offsetZ; //! < Not used

	double scaleX;
	double scaleY; //! < should be set by ueser.
	double scaleZ; //! < Not used
};

namespace API
{

enum CAM_STATUS
{
	All_OK,
	OPEN_CAM_ERROR,
	SCAN_DEVICE_ERROR,
	CSV_PATH_ERROR,
	XML_PATH_ERROR,
	SET_CAM_ERROR,
	CLO_CAM_ERROR, //! < close camera error
	GET_IMG_ERROR,
	TIME_OUT
};

/**
	Range3Shared is shared by all Range3s. \n \n

	When system initializing, the Ranger3Shared object should be initialized too. \n
	The Ranger3Shared() will call scanDevice() to open all the devices connected. \n
	All devices will be closed by ~Ranger3Shared(). \n

*/
typedef std::map<std::string, std::shared_ptr<DeviceConnection>> deviceList;
class EXPORT_TO_DLL Ranger3Shared final
{
public:

	/** 
		CTI file should be in the same folder with exe.
	*/
	Ranger3Shared();
	~Ranger3Shared();

	std::shared_ptr<Sample::Consumer>& getConsumer();
	deviceList & getConDevList(); // To get the ptr map of devices. 

protected:
	CAM_STATUS scanDevice();
	void setProcessPriority(void);

private:
	std::shared_ptr<Sample::Consumer> m_pconsumer;
	deviceList m_connectedDevices;
	std::mutex consumer_lock;
	std::mutex conDevList_lock;
	std::set<GenTL::IF_HANDLE> m_openInterfaces;
	int m_nDevices;		// ! < the number of devices. 

};


/**
	Each Ranger3 device corresponding to a Ranger3 object. \n \n

	If only one camera, Ranger3() can op
*/
class EXPORT_TO_DLL Ranger3 final
{
public:
	/** @brief Ranger3() \n

		Ranger3() is only used for initializing a ptr. All the variables are empty, and need to be set later. \n
		Ranger3(RS) is used when one Ranger3 is connected. \n
		Ranger3(RS, mac) is used when more than one Ranger3s are connected. mac is used for selecting Ranger3. \n
	*/
	Ranger3();
	Ranger3(PARA_IN const std::shared_ptr<Ranger3Shared>& RS);
	Ranger3(
		PARA_IN const std::shared_ptr<Ranger3Shared> & RS,
		PARA_IN const std::string & mac);

	~Ranger3();

	CAM_STATUS openCamera();

	/**
		Must keep setCamera be called in main threads.\n \n 

		@param _scaleY = (y_scaning_distance_in_mm) / (profiles_number); \n
	*/
	CAM_STATUS setCamera(
		PARA_IN const double _scaleY,
		PARA_IN const std::string & para_csv_path = "Please_input_parameters_file_path.csv",
		PARA_IN const std::string & calib_xml_path = "Please_input_calibration_file_path.xml",
		PARA_IN const ranger3calibration::RectificationMethod method = ranger3calibration::RectificationMethod::Top);

	CAM_STATUS getImageData(
		PARA_OUT ImgTable & imgTable,
		PARA_IN const uint64_t timeout = 1000);

	/** @brief closeCamera \n \n

		Stop acquition and free the buffers. \n
		The true process of closing device and interface will be in ~Ranger3Shared(), \n
	*/
	CAM_STATUS closeCamera();

	const std::string getDeviceName() const
	{
		return m_DeviceName;
	}


protected:
	void setThreadPriority(GenApi::CNodeMapRef & dataStreamNodeMap);
	void praseDataRangeOnly(GenTLApi * tl, std::vector<GenTL::BUFFER_HANDLE>& bufferHandles, const size_t bufferId, ImgTable & imgTable);
	void praseDataRangeAndReflectance(GenTLApi * tl, std::vector<GenTL::BUFFER_HANDLE>& bufferHandles, const size_t bufferId, ImgTable & imgTable);



private:
	std::shared_ptr<Ranger3Shared> m_pR3S;			//! > shared parts.
	std::shared_ptr<DeviceConnection> m_device;		//! > the device.
	std::shared_ptr<Sample::Consumer> m_consumer;	//! > used in closing camera
	std::unique_ptr<Sample::ChunkAdapter> m_chunkAdapter;

	GenTL::DEV_HANDLE m_deviceHandle;
	GenApi::CNodeMapRef m_deviceNodeMap;

	std::string m_DeviceName; //! > "SICKGigEVisionTL_DEV_" + mac + "_Ranger3"

	std::mutex setCamera_lock;
	std::mutex getImageData_lock;

	std::unique_ptr<ranger3calibration::CalibrationFilter> m_pCalFilter;
	std::unique_ptr<ranger3calibration::RectificationFilter> m_pRectFilter;
	std::unique_ptr<ranger3calibration::CalibrationData> m_pCalibrationData;
	std::unique_ptr<ranger3calibration::RectificationData> m_pRectificationData;
	std::unique_ptr<ranger3calibration::SensorTraits> m_pSensorTraits;

	std::ofstream mLog;
	bool m_bSelected;

	int64_t m_bufferWidth;
	int64_t m_bufferHeight;
	int64_t m_buffer16Size;
	uint8_t* m_pBuffer16; //! > need to delete

	int64_t m_aoiHeight;
	int64_t	m_aoiOffsetX;
	int64_t m_aoiOffsetY;
	//GenIRanger::RangeAxis m_rangeAxis; // debug to save uncalibrate image as icon

	double m_scaleY;		//! > user input
	bool m_ChunkModeActive; //! > will effect Image Grab Mode
	ranger3calibration::RectificationMethod m_RectificationMethod;
	bool m_isUsingReflectance;

};


}
}

typedef Sample::API::Ranger3Shared R3S;
typedef Sample::API::Ranger3 R3;
typedef Sample::API::CAM_STATUS Ecode;
