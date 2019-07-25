#pragma execution_character_set("utf-8")
#include "CameraTest.h"
#include <QDebug>
#include <QlineEdit>
#include "CImageManager.h"
#include "cconfigmanager.h"
#include <QMessageBox>
extern void ShowMessageBox1(QMessageBox::Icon type, QString title, QString content);
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
	ui.label_2->setText("��ʼȡͼ");
	//CamCaptureThread::Instance()->StartCapture(ui.lineEdit->text().toInt());
}
void CameraTest::pushButton2_onClick() {	
	CImageManager::Instance()->StopThread();
	ui.label_2->setText("ֹͣȡͼ");
	//CamCaptureThread::Instance()->StopCapture();
}
//R
void CameraTest::on_btnConfig1_clicked() {
    CConfigManager* config = CConfigManager::getInstance();
    bool bl = CImageManager::Instance()->ChangedCamConfig(config->csvRight, config->calibxmlRight);
    if (!bl) {
        ShowMessageBox1(QMessageBox::Critical, "����", "�л���������ô���!");
    }
}
//L
void CameraTest::on_btnConfig2_clicked() {
    CConfigManager* config = CConfigManager::getInstance();
    bool bl = CImageManager::Instance()->ChangedCamConfig(config->csvLeft, config->calibxmlLeft);
    if (!bl) {
        ShowMessageBox1(QMessageBox::Critical, "����", "�л���������ô���!");
    }
}
//void  CameraTest::onThreadStarted() {
//	ui.label_2->setText("�߳̿�ʼ");
//	ui.plainTextEdit->clear();
//}
//void  CameraTest::onThreadFinished() {
//	ui.label_2->setText("�߳�ֹͣ");
//}
int CaptureImageNum = 0;
int DecodeImageNum = 0;
void  CameraTest::onCaptureImage() {
	ui.plainTextEdit->appendPlainText(QString("Cam��ȡ�˵�%1��ͼ").arg(++CaptureImageNum));
}
void  CameraTest::onDecodeImage() {
	ui.plainTextEdit->appendPlainText(QString("�����˵�%1��ͼ").arg(++DecodeImageNum));
}