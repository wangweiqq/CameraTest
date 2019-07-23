#pragma execution_character_set("utf-8")
#include<opencv2/opencv.hpp>
#include<iostream>
#include<HalconCpp.h>
#include<qdebug.h>

using namespace std;
using namespace cv;
using namespace HalconCpp;
HObject Mat2HObject(const cv::Mat &image);
// Main procedure
void CircleProcess(cv::Mat image)
{
	/*
    // 定义圆心坐标
    vector<double> rowCir;
    vector<double> columnCir;
    vector<double> radiusCir;

    // Local iconic variables
    HObject  ho_Image, ho_ImageZoom, ho_ImageMedian;
    HObject  ho_Regions, ho_ConnectedRegions, ho_SelectedRegions;
    HObject  ho_RegionFillUp, ho_RegionDifference, ho_ConnectedRegions1;
    HObject  ho_SelectedRegions1, ho_ObjectSelected, ho_Contours;
    HObject  ho_Circle1, ho_Circle2, ho_Circle3;

    // Local control variables
    HTuple  hv_Width, hv_Height, hv_WindowHandle;
    HTuple  hv_Width1, hv_Height1, hv_WindowHandle1, hv_RowCir;
    HTuple  hv_ColumnCir, hv_RadiusCir, hv_Number, hv_i, hv_Row;
    HTuple  hv_Column, hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder;

    // ReadImage(&ho_Image, "rangeImg.jpg");

    //cv::Mat image = cv::imread("rangeImg.jpg");
    ho_Image = Mat2HObject(image);
    GetImageSize(ho_Image, &hv_Width, &hv_Height);
	double width = hv_Width.D();
	double height = hv_Height.D();
	ZoomImageSize(ho_Image, &ho_ImageZoom, hv_Width*1.08, hv_Height, "constant");
    GetImageSize(ho_ImageZoom, &hv_Width1, &hv_Height1);
    MedianImage(ho_ImageZoom, &ho_ImageMedian, "circle", 9, "mirrored");
    Threshold(ho_ImageMedian, &ho_Regions, 76.894, 79.565);
    Connection(ho_Regions, &ho_ConnectedRegions);
    SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 500000, 7000000);
    FillUp(ho_SelectedRegions, &ho_RegionFillUp);
    Difference(ho_RegionFillUp, ho_SelectedRegions, &ho_RegionDifference);
    Connection(ho_RegionDifference, &ho_ConnectedRegions1);
    SelectShape(ho_ConnectedRegions1, &ho_SelectedRegions1, "area", "and", 300000,
                400000);
    hv_RowCir = HTuple();
    hv_ColumnCir = HTuple();
    hv_RadiusCir = HTuple();
    CountObj(ho_SelectedRegions1, &hv_Number);

    {
        HTuple end_val22 = hv_Number;
        HTuple step_val22 = 1;
        for (hv_i = 1; hv_i.Continue(end_val22, step_val22); hv_i += step_val22)
        {
            SelectObj(ho_SelectedRegions1, &ho_ObjectSelected, hv_i);
            GenContourRegionXld(ho_ObjectSelected, &ho_Contours, "border");
            FitCircleContourXld(ho_Contours, "algebraic", -1, 0, 0, 3, 2, &hv_Row, &hv_Column,
                                &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
            hv_RowCir[hv_i] = hv_Row;
            hv_ColumnCir[hv_i] = hv_Column;
            hv_RadiusCir[hv_i] = hv_Radius;

            rowCir.push_back(hv_Row.D());
            columnCir.push_back(hv_Column.D());
            radiusCir.push_back(hv_Radius.D());
        }
    }


    // 根据获得的坐标将圆画出来

    GenCircle(&ho_Circle1, HTuple(hv_RowCir[1]), HTuple(hv_ColumnCir[1]), HTuple(hv_RadiusCir[1]));
    GenCircle(&ho_Circle2, HTuple(hv_RowCir[2]), HTuple(hv_ColumnCir[2]), HTuple(hv_RadiusCir[2]));
    GenCircle(&ho_Circle3, HTuple(hv_RowCir[3]), HTuple(hv_ColumnCir[3]), HTuple(hv_RadiusCir[3]));

    // 显示图片
    HWindow w(0, 0, hv_Width1/8, hv_Height1/8);
    DispObj(ho_ImageZoom, w);
    SetDraw(w, "margin");
    SetLineWidth(w, 3);
    SetColor(w, "green");
    DispObj(ho_Circle1, w);
    DispObj(ho_Circle2, w);
    DispObj(ho_Circle3, w);
    w.Click();

	*/
	HObject ho_Image;
	ho_Image = Mat2HObject(image);
	//WriteImage(ho_Image, "tiff", 0, "testHalcon");
}

/*
数据格式转换
*/

HObject Mat2HObject(const cv::Mat &image)

{
    cout << "数据转换中......" << endl;
    HObject Hobj = HObject();
    int hgt = image.rows;
    int wid = image.cols;
    int i;
	std::cout << image.type() <<","<<CV_32FC1<<","<<",step= "<<image.step<<" int"<<sizeof(unsigned int)<< std::endl;
	int ty = image.type();
    //  CV_8UC3
    if (image.type() == CV_8UC3)
    {
        vector<cv::Mat> imgchannel;
        split(image, imgchannel);
        cv::Mat imgB = imgchannel[0];
        cv::Mat imgG = imgchannel[1];
        cv::Mat imgR = imgchannel[2];
        uchar* dataR = new uchar[hgt*wid];
        uchar* dataG = new uchar[hgt*wid];
        uchar* dataB = new uchar[hgt*wid];

        for (i = 0; i<hgt; i++)

        {
            memcpy(dataR + wid*i, imgR.data + imgR.step*i, wid);
            memcpy(dataG + wid*i, imgG.data + imgG.step*i, wid);
            memcpy(dataB + wid*i, imgB.data + imgB.step*i, wid);
        }
        GenImage3(&Hobj, "byte", wid, hgt, (Hlong)dataR, (Hlong)dataG, (Hlong)dataB);
        delete[]dataR;
        delete[]dataG;
        delete[]dataB;
        dataR = NULL;
        dataG = NULL;
        dataB = NULL;

    }
    //  CV_8UCU1
    else if (image.type() == CV_8UC1)

    {
        uchar* data = new uchar[hgt*wid];
        for (i = 0; i<hgt; i++)
            memcpy(data + wid*i, image.data + image.step*i, wid);
        GenImage1(&Hobj, "byte", wid, hgt, (Hlong)data);
        delete[] data;
        data = NULL;
	}
	else if (image.type() == CV_32FC1) {
		//GenImage1(&Hobj, "byte", image.rows, image.cols, (Hlong)image.data);
		/*
		// 原始版本将32位图转成8位图
		cv::Mat temImg(image.rows, image.cols, CV_8UC1);
		image.convertTo(temImg, CV_8UC1);
		//cv::imwrite("C:/Users/Administrator/Desktop/imgFile/test.jpg", temImg);
		uchar* data = new uchar[hgt*wid];
		for (i = 0; i<hgt; i++)
			memcpy(data + wid*i, temImg.data + temImg.step*i, wid);
		GenImage1(&Hobj, "real", wid, hgt, (Hlong)data);
		delete[] data;
		data = NULL;
		*/
		float* data = new float[hgt*wid];
		for (i = 0; i < hgt; i++)
			memcpy(data + wid*i, image.data + image.step*i, wid * 4);
		GenImage1(&Hobj, "real", wid, hgt, (Hlong)data);
		delete[] data;
		data = NULL;
	}
    return Hobj;
}


//int main()
//{
//	action();

//	cv::waitKey(0);
//}
