/*
 图像处理
*/
#pragma once
#include <QObject>
#include "CamCaptureThread.h"
#include "ImageDecodThread.h"
class CImageManager : public QObject
{
	Q_OBJECT
public:
	static CImageManager* Instance();
	~CImageManager();
	/*初始化参数*/
	bool Init(QString csv, QString calibxml);
	/*启动线程*/
	void StartThread();
	/*停止线程*/
	void StopThread();
private:
	CImageManager(QObject *parent = nullptr);
signals:
	void DecodeImage();
	void CaptureImage();
private:
	CamCaptureThread* camThread = nullptr;
	ImageDecodThread* decodeThread = nullptr;
};
