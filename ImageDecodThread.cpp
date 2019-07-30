#pragma execution_character_set("utf-8")
#include "ImageDecodThread.h"
#include "Ranger3.h"
#include <opencv2/opencv.hpp>
#include <QDateTime>
#include <QDebug>
#include <HalconCpp.h>
#include <cstdio>
using namespace HalconCpp;

extern Sample::ImgTable ImgTableDequeue();
extern void CircleProcess(cv::Mat image);
//extern bool CircleProcess(int type, cv::Mat img, cv::Point3i& point);

HObject convertToHObject(std::vector<float>& range, int width, int height)
{
	/*HObject dst = HObject();
	if (ImgT.range.empty())
		return HObject();
	HObject obgdst = HObject();
	int height = ImgT.rows;
	int width = ImgT.cols;
	float* data = new float[width*height];
	qDebug() << "&ImgT.range[0]:" << &ImgT.range[0];
	for (int i = 0; i < ImgT.rows; i++)
	{
		memcpy(data + width*i, &ImgT.range[0] + width*i, width*4);
	}
	GenImage1(&dst, 'real', width, height, (Hlong)data);
	std::ofstream of_2("test_range_to_h.txt");
	of_2.write((char*)(data), sizeof(float)*width*height);
	of_2.close();
	delete[] data;
	data = NULL;
	return dst;*/
	//方法二：
	HObject dst = HObject();
	float *HImage = new float[width * height];
#pragma omp parallel for
	for (int i = 0; i < height; i++)
	{
#pragma omp parallel for
		for (int j = 0; j < width; j++)
		{
			if (std::isnan(range[i * width + j]))
			{
				range[i * width + j] = -1;
				HImage[i * width + j] = range[i * width + j];
			}
			else
			{
				
				HImage[i * width + j] = range[i * width + j];
			}
			
			
		}
	}
	GenImage1(&dst, "real", width, height, (Hlong)HImage);
	delete[] HImage;
	HImage = NULL;
	//WriteImage(dst, "tiff", 0, "haha0727");
	return dst;
	
}

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
// OpenCV is needed in this function
static cv::Mat convertToMatI(Sample::ImgTable & ImgT)
{
	cv::Mat dst(ImgT.rows, ImgT.cols, CV_8UC1);
	for (size_t j = 0; j<dst.rows; ++j)
	{
		uchar * pd = dst.ptr<uchar>(j);
		for (size_t i = 0; i < dst.cols; ++i)
		{
			pd[i] = ImgT.reflectance[j*dst.cols + i];
		}
	}
	return dst;
}
ImageDecodThread::ImageDecodThread(QObject *parent)
	: QThread(parent)
{
}

ImageDecodThread::~ImageDecodThread()
{
	StopDecode();
}
ImageDecodThread* ImageDecodThread::Instance() {
	static ImageDecodThread pDecod;
	return &pDecod;
}
void ImageDecodThread::run() {
	//char fileName[1024];
	int count = 1;
	while (!IsStopThread) {
		Sample::ImgTable imgTable = ImgTableDequeue();
		if (imgTable.isEmpty()) {
			msleep(500);
		}
		else {
			//cv::Mat t_range = convertToMatR(imgTable);
			int width = imgTable.cols;		//image width
			int height = imgTable.rows;		//image height
			HObject Hob_Image = convertToHObject(imgTable.range, width, height);
			//QString file = QString("D:/0727/test/%1.tiff").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz"));
			QString file = QString("D:/0727/test/%1.tiff").arg(count++);
			qDebug() << file;
			const char* f = file.toLocal8Bit();
			qDebug() << f;

			WriteImage(Hob_Image, "tiff", 0, file.toStdString().c_str());
		

			qDebug() << "解码结束";
			//std::cout << "解码结束" << std::endl;
			emit DecodeImage();
		}
	}
}
/*开始图像处理*/
void ImageDecodThread::StartDecode() {
	if (isRunning()) {
		return;
	}
	IsStopThread = false;
	start();
}
/*停止图像处理*/
void ImageDecodThread::StopDecode() {
	IsStopThread = true;
	wait();
}
