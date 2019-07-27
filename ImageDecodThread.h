/*
	摄像机抓取图片处理类
*/
#pragma once

#include <QThread>
#include <omp.h>
class ImageDecodThread : public QThread
{
	Q_OBJECT
public:
	/*单件模式*/
	static ImageDecodThread* Instance();
	~ImageDecodThread();
	void run();
	/*开始图像处理*/
	void StartDecode();
	/*停止图像处理*/
	void StopDecode();
signals:
	void DecodeImage();
protected:
	ImageDecodThread(QObject *parent = NULL);
private:
	bool IsStopThread = false;//是否停止线程
};
