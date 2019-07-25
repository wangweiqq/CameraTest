/*
	摄像头启动，捕捉图片
*/
#pragma once
#include <QThread>
#include <QObject>
#include "Ranger3.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <QMutex>
class CamCaptureThread : public QThread
{
	Q_OBJECT
public:
	CamCaptureThread(QObject *parent = NULL);
	//static CamCaptureThread* Instance();
	~CamCaptureThread();
	/*初始化摄像机*/
	bool InitCamDevice(QString csvPath, QString calibXml);
    bool changeCamConfig(QString csvPath, QString calibXml);
	void run();
	/*开始捕捉图像*/
	void StartCapture();
	/*停止捕捉图像*/
	void StopCapture();
signals:
	void CaptureImage();
protected:
private:
	//int NumImg = 0;//线程读几张图片退出
	bool IsStopThread = false;//是否停止线程
	std::shared_ptr<R3S> pR3S = nullptr;
	std::unique_ptr<R3> pR3_1 = nullptr;
	//std::unique_ptr<ImageGabber> pIG = nullptr;
	std::string para_csv_path;// = "D:\\SICK-Images\\Ranger3ConfigFile.csv";
	std::string calib_xml_path;// = "D:\\SICK-Images\\RightLazerCalibrationResult.xml";
    QMutex mutex;
};
