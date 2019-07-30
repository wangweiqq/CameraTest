#pragma execution_character_set("utf-8")
#include "CameraTest.h"
#include <QtWidgets/QApplication>
#include "CImageManager.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include "cconfigmanager.h"
void ShowMessageBox1(QMessageBox::Icon type, QString title, QString content) {
	QMessageBox box(type, title, content);
	box.setStandardButtons(QMessageBox::Ok);
	box.setButtonText(QMessageBox::Ok, "确定");
	box.exec();
}
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	


	//QString csv1 = "D:\\SICK-Images\\Ranger3ConfigFile.csv";
	/*QString csv1 = "D:\\work\\CameraTest\\testcalib0722\\Cam.csv";
	QString calibxml1 = "D:\\work\\CameraTest\\testcalib0722\\CalibrationResult.xml";*/
	//QString csv1 = "D:\\SICKCameraConfig\\RightSickConfigFile.csv";
	/*QString csv1 = "D:\\SICKCameraConfig\\RightSickConfigFile_MotionPattern.csv";
	QString calibxml1 = "D:\\SICKCameraConfig\\Right-CalibrationResult.xml";*/
	/*QString csv2 = "D:\\SICKCameraConfig\\RightSickConfigFile.csv";
	QString calibxml2 = "D:\\SICKCameraConfig\\Right-CalibrationResult.xml";*/
    CConfigManager* config = CConfigManager::getInstance();
	if (!CImageManager::Instance()->Init(config->csvRight, config->calibxmlRight)) {
		ShowMessageBox1(QMessageBox::Critical, "错误", "初始化摄像机错误!");
		return -1;
	}
	CameraTest w;
	w.show();
	return a.exec();
}
