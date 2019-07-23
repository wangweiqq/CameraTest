#pragma once

#include <QtWidgets/QDialog>
#include "ui_CameraTest.h"

class CameraTest : public QDialog
{
	Q_OBJECT
public:
	CameraTest(QWidget *parent = Q_NULLPTR);
public slots:
	void pushButton_onClick();
	void pushButton2_onClick();
	/*void onThreadStarted();
	void onThreadFinished();*/
	void onCaptureImage();
	void onDecodeImage();
private:
	Ui::CameraTestClass ui;
};
