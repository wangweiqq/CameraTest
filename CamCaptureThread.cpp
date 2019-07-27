#pragma execution_character_set("utf-8")
#include "CamCaptureThread.h"
#include <QDebug>
#include <exception>
#include <memory>
#include <QDateTime>
#include<HalconCpp.h>
#include <QMutexLocker>
using namespace HalconCpp; 
extern HObject Mat2HObject(const cv::Mat &image);
extern void ImgTableEnqueue(Sample::ImgTable& p);
// OpenCV is needed in this function
CamCaptureThread::CamCaptureThread( QObject *parent)
	: QThread(parent)
{
}

CamCaptureThread::~CamCaptureThread()
{
	StopCapture();
	if (pR3_1 != nullptr) {
		Ecode ec = pR3_1->closeCamera();
		if (ec == Ecode::CLO_CAM_ERROR)
		{
			printf("Error: close camara failed \n");
			//return 0;
		}
	}
}
//CamCaptureThread* CamCaptureThread::Instance() 
//{
//	static CamCaptureThread capture;
//	return &capture;
//}
bool CamCaptureThread::InitCamDevice(QString csvPath, QString calibXml) {
	para_csv_path = csvPath.toLocal8Bit();
	calib_xml_path = calibXml.toLocal8Bit();
	qDebug() << "para_csv_path：" << para_csv_path.c_str() << ",calib_xml_path=" << calib_xml_path.c_str();
	pR3S = std::make_shared<R3S>();
	if (pR3S->getConsumer() == nullptr)
	{
		printf("Error: CTI file should be in the same folder with exe. \n");
		return false;
	}

	// check 
	Sample::API::deviceList devices = pR3S->getConDevList();
	if (devices.empty())
	{
		printf("Error: scan camera error. \n");
		return false;
	}
	else
	{
		printf("find camera: \n");
		for (auto sub : devices)
		{
			printf("-    %s \n", sub.first.c_str());
		}
	}

	// --- Init one Ranger3.
	pR3_1 = std::make_unique<R3>(pR3S);
	// check
	if (pR3_1->getDeviceName() == "ERROR")
	{
		printf("Error: Init camara failed, please make sure the camera can be found in Ranger3Studio. \n");
		return false;
	}

	//  --- Open a Ranger3.
	Ecode ec = pR3_1->openCamera();
	if (ec == Ecode::OPEN_CAM_ERROR)
	{
		printf("Error: Open camara failed");
		return false;
	}
	qDebug() << "para_csv_path" << para_csv_path.c_str();
	qDebug() << "calib_xml_path = " << calib_xml_path.c_str();
	std::string para_csv_path = "D:\\SICKCameraConfig\\RightSickConfigFile.csv";
	std::string calib_xml_path = "D:\\SICKCameraConfig\\Right-CalibrationResult.xml";
	ec = pR3_1->setCamera(0.02, para_csv_path, calib_xml_path); // 0.01 is just an example.
	if (ec != Ecode::All_OK)
	{
		printf("Error: setCamera failed. para_csv_path=%s, calib_xml_path=%s \n",
			para_csv_path.c_str(), calib_xml_path.c_str());
		switch (ec)
		{
		case Ecode::SET_CAM_ERROR:
			printf("Error: SET_CAM_ERROR \n");
			break;
		case Ecode::XML_PATH_ERROR:
			printf("Error: XML_PATH_ERROR \n");
			break;
		case Ecode::CSV_PATH_ERROR:
			printf("Error: CSV_PATH_ERROR \n");
			break;
		default:
			printf("Error: UNKNOWN_ERROR \n");
			break;
		}
		return false;
	}
	return true;
}
//bool CamCaptureThread::InitCamDevice(QString csvPath, QString calibXml) {
//	
//	pR3S = std::make_shared<R3S>();
//	if (pR3S->getConsumer() == nullptr)
//	{
//		printf("Error: CTI file should be in the same folder with exe. \n");
//		return false;
//	}
//
//	// check 
//	Sample::API::deviceList devices = pR3S->getConDevList();
//	if (devices.empty())
//	{
//		printf("Error: scan camera error. \n");
//		return false;
//	}
//	else
//	{
//		printf("find camera: \n");
//		for (auto sub : devices)
//		{
//			printf("-    %s \n", sub.first.c_str());
//		}
//	}
//	return changeCamConfig(csvPath, calibXml);
//	//// --- Init one Ranger3.
//	//pR3_1 = std::make_unique<R3>(pR3S);
//	//														// check
//	//if (pR3_1->getDeviceName() == "ERROR")
//	//{
//	//	printf("Error: Init camara failed, please make sure the camera can be found in Ranger3Studio. \n");
//	//	return false;
//	//}
//    
//	//  --- Open a Ranger3.
//	/*Ecode ec = pR3_1->openCamera();
//	if (ec == Ecode::OPEN_CAM_ERROR)
//	{
//		printf("Error: Open camara failed");
//		return false;
//	}*/
//	/*std::string para_csv_path = "D:\\SICKCameraConfig\\RightSickConfigFile.csv";
//	std::string calib_xml_path = "D:\\SICKCameraConfig\\Right-CalibrationResult.xml";*/
//	//para_csv_path = "D:\\SICKCameraConfig\\RightSickConfigFile_MotionPattern.csv";//csvPath.toLocal8Bit();
//	//calib_xml_path = "D:\\SICKCameraConfig\\Right-CalibrationResult.xml";//calibXml.toLocal8Bit();
//	//qDebug() << "para_csv_path"<< para_csv_path.c_str();
//	//qDebug() << "calib_xml_path = " << calib_xml_path.c_str();
//	//ec = pR3_1->setCamera(0.02, para_csv_path, calib_xml_path); // 0.01 is just an example.
//	//if (ec != Ecode::All_OK)
//	//{
//	//	printf("Error: setCamera failed. para_csv_path=%s, calib_xml_path=%s \n",
//	//		para_csv_path.c_str(), calib_xml_path.c_str());
//	//	switch (ec)
//	//	{
//	//	case Ecode::SET_CAM_ERROR:
//	//		printf("Error: SET_CAM_ERROR \n");
//	//		break;
//	//	case Ecode::XML_PATH_ERROR:
//	//		printf("Error: XML_PATH_ERROR \n");
//	//		break;
//	//	case Ecode::CSV_PATH_ERROR:
//	//		printf("Error: CSV_PATH_ERROR \n");
//	//		break;
//	//	default:
//	//		printf("Error: UNKNOWN_ERROR \n");
//	//		break;
//	//	}
//	//	return false;
//	//}
//	//return true;
//}
//bool CamCaptureThread::changeCamConfig(QString csvPath, QString calibXml) {
//    QMutexLocker locker(&mutex);
//    para_csv_path = csvPath.toLocal8Bit();
//    calib_xml_path = calibXml.toLocal8Bit();
//    qDebug() << "para_csv_path：" << para_csv_path.c_str() << ",calib_xml_path=" << calib_xml_path.c_str();
//	if (pR3_1 != nullptr) {
//		Ecode ec = pR3_1->closeCamera();
//		if (ec == Ecode::CLO_CAM_ERROR)
//		{
//			printf("Error: close camara failed \n");
//			//return 0;
//		}
//	}
//    // --- Init one Ranger3.
//    pR3_1 = std::make_unique<R3>(pR3S);
//    // check
//    if (pR3_1->getDeviceName() == "ERROR")
//    {
//        printf("Error: Init camara failed, please make sure the camera can be found in Ranger3Studio. \n");
//        return false;
//    }
//
//    //  --- Open a Ranger3.
//    Ecode ec = pR3_1->openCamera();
//    if (ec == Ecode::OPEN_CAM_ERROR)
//    {
//        printf("Error: Open camara failed");
//        return false;
//    }
//        
//	
//    qDebug() << "para_csv_path" << para_csv_path.c_str();
//    qDebug() << "calib_xml_path = " << calib_xml_path.c_str();
//	para_csv_path = "D:\\SICKCameraConfig\\RightSickConfigFile_MotionPattern.csv";
//	calib_xml_path = "D:\\SICKCameraConfig\\Right-CalibrationResult.xml";
//    ec = pR3_1->setCamera(0.02, para_csv_path, calib_xml_path); // 0.01 is just an example.
//    if (ec != Ecode::All_OK)
//    {
//        printf("Error: setCamera failed. para_csv_path=%s, calib_xml_path=%s \n",
//            para_csv_path.c_str(), calib_xml_path.c_str());
//        switch (ec)
//        {
//        case Ecode::SET_CAM_ERROR:
//            printf("Error: SET_CAM_ERROR \n");
//            break;
//        case Ecode::XML_PATH_ERROR:
//            printf("Error: XML_PATH_ERROR \n");
//            break;
//        case Ecode::CSV_PATH_ERROR:
//            printf("Error: CSV_PATH_ERROR \n");
//            break;
//        default:
//            printf("Error: UNKNOWN_ERROR \n");
//            break;
//        }
//        return false;
//    }
//}
static cv::Mat convertToMatR(Sample::ImgTable & ImgT)
{
	cv::Mat dst(ImgT.rows, ImgT.cols, CV_32FC1);
	for (size_t j = 0; j<dst.rows; ++j)
	{
		float * pd = dst.ptr<float>(j);
		for (size_t i = 0; i < dst.cols; ++i)
		{
			pd[i] = ImgT.range[j*dst.cols + i];
		}
	}

	return dst;
}
int count = 0;
void CamCaptureThread::run() {

	while (!IsStopThread) {
        QMutexLocker locker(&mutex);
		Sample::ImgTable imgTable;
		//try {
			Ecode ec = pR3_1->getImageData(imgTable, 5000);
			// save image with opencv.
			if (ec == Ecode::TIME_OUT)
				continue;
			if (ec == Ecode::All_OK)
			{
				qDebug() << "getImageData success = "<< ++count;

				//cv::Mat t_range = convertToMatR(imgTable);
				//QString file = QString("D:/0722/%1.tiff").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz"));
				////qDebug() << file;
				//const char* f = file.toLocal8Bit();
				//qDebug() << f;
				//cv::imwrite(f, t_range);
				/*cv::FileStorage fileoutput;
				fileoutput.open(f, cv::FileStorage::WRITE);
				fileoutput << "Range" << t_range;*/
				/*HObject Himage = Mat2HObject(t_range);
				WriteImage(Himage, "tif", 0, "D:\0722\io.tiff");*/
				ImgTableEnqueue(imgTable);
				emit CaptureImage();
			}
		/*}
		catch (...) {}*/
	}
	printf("Exit Run. \n");
}
/*开始捕捉图像*/
void CamCaptureThread::StartCapture()
{
	if(isRunning()) {
		return;
	}
	/*if (num <= 0 || isRunning()) {
		return;
	}
	NumImg = num;*/
	IsStopThread = false;
	start();
}
/*停止捕捉图像*/
void CamCaptureThread::StopCapture() {
	IsStopThread = true;
	wait();
}
