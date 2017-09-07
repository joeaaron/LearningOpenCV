#include <opencv2/opencv.hpp>
#include "manyImages.h"

void ManyImages(vector<Mat> Images, Mat& dst, int imgRows)
{
	int Num = Images.size();
	cv::Mat Window(300 * ((Num - 1) / imgRows + 1), 300 * imgRows, CV_8UC3, Scalar(0, 0, 0));
	cv::Mat stdImg;      //standard imgs
	cv::Mat imgROI;      //img placement area

	cv::Size stdSize = cv::Size(300, 300);
	int xBegin = 0;
	int yBegin = 0;

	for (int i = 0; i < Num; i++)
	{
		///starting coordinate
		xBegin = (i % imgRows) * stdSize.width;    
		yBegin = (i / imgRows) * stdSize.height;

		resize(Images[i], stdImg, stdSize, 0, 0, INTER_LINEAR);   //set img as std
		imgROI = Window(Rect(xBegin, yBegin, stdSize.width, stdSize.height));
		stdImg.copyTo(imgROI);
	}
	dst = Window;
}