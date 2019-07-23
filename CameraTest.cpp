#pragma execution_character_set("utf-8")
#include "CameraTest.h"
#include <QDebug>
#include <QlineEdit>
#include "CImageManager.h"
CameraTest::CameraTest(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(pushButton_onClick()));
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(pushButton2_onClick()));
	connect(CImageManager::Instance(), SIGNAL(CaptureImage()), this, SLOT(onCaptureImage()));
	connect(CImageManager::Instance(), SIGNAL(DecodeImage()), this, SLOT(onDecodeImage()));
	/*connect(CamCaptureThread::Instance(), SIGNAL(started()), this, SLOT(onThreadStarted()));
	connect(CamCaptureThread::Instance(), SIGNAL(finished()), this, SLOT(onThreadFinished()));
	connect(CamCaptureThread::Instance(), SIGNAL(CaptureImage(int)), this, SLOT(onCaptureImage(int)));*/
}
void CameraTest::pushButton_onClick() {
	CImageManager::Instance()->StartThread();
	ui.label_2->setText("开始取图");
	//CamCaptureThread::Instance()->StartCapture(ui.lineEdit->text().toInt());
}
void CameraTest::pushButton2_onClick() {	
	CImageManager::Instance()->StopThread();
	ui.label_2->setText("停止取图");
	//CamCaptureThread::Instance()->StopCapture();
}
//void  CameraTest::onThreadStarted() {
//	ui.label_2->setText("线程开始");
//	ui.plainTextEdit->clear();
//}
//void  CameraTest::onThreadFinished() {
//	ui.label_2->setText("线程停止");
//}
int CaptureImageNum = 0;
int DecodeImageNum = 0;
void  CameraTest::onCaptureImage() {
	ui.plainTextEdit->appendPlainText(QString("Cam获取了第%1张图").arg(++CaptureImageNum));
}
void  CameraTest::onDecodeImage() {
	ui.plainTextEdit->appendPlainText(QString("处理了第%1张图").arg(++DecodeImageNum));
}