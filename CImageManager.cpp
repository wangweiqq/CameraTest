#pragma execution_character_set("utf-8")
#include "CImageManager.h"
#include <QQueue>
#include "Ranger3.h"
#include <QMutex>
#include <QMutexLocker> 
#include <iostream>
#include <QFileInfo>
#include <QDebug>
QQueue<Sample::ImgTable> ListImageTable;
QMutex imgMutex;
/*
	摄像机捕捉图片入队
*/
void ImgTableEnqueue(Sample::ImgTable& p) {
	QMutexLocker locker(&imgMutex);
	ListImageTable.enqueue(p);
}
/*
摄像机捕捉图片出队
*/
Sample::ImgTable ImgTableDequeue() {
	QMutexLocker locker(&imgMutex);
	if (ListImageTable.isEmpty()) {
		return Sample::ImgTable();
	}
	return ListImageTable.dequeue();
}
CImageManager* CImageManager::Instance() {
	static CImageManager manager;
	return &manager;
}
CImageManager::CImageManager(QObject *parent): QObject(parent)
{
	ListImageTable.clear();
	camThread = CamCaptureThread::Instance();
	decodeThread = ImageDecodThread::Instance();
	connect(camThread, SIGNAL(CaptureImage()), this, SIGNAL(CaptureImage()));
	connect(decodeThread, SIGNAL(DecodeImage()), this, SIGNAL(DecodeImage()));
}

CImageManager::~CImageManager()
{
	StopThread();
}
/*初始化参数*/
bool  CImageManager::Init(QString csv, QString calibxml) {
    //判断摄像机参数文件是否存在
    bool bl = QFileInfo::exists(csv);
    bool b2 = QFileInfo::exists(calibxml);
	qDebug() << csv << "," << calibxml<<","<<bl<<","<<b2;
    if(!QFileInfo::exists(csv) || !QFileInfo::exists(calibxml)){
        return false;
    }
	if (!camThread->InitCamDevice(csv, calibxml)) {
		return false;
	}
	return true;
}
void CImageManager::StartThread() {
	camThread->StartCapture();
	decodeThread->StartDecode();
}
void CImageManager::StopThread() {
	camThread->StopCapture();
	decodeThread->StopDecode();
	std::cout << "图片队列剩余" << ListImageTable.count() << "章图未处理" << std::endl;
	ListImageTable.clear();
	/*while (!ListImageTable.isEmpty()) {
		delete ImgTableDequeue();
	}*/
}
bool CImageManager::ChangedCamConfig(QString csvPath, QString calibXml) {
    return camThread->changeCamConfig(csvPath, calibXml);
}