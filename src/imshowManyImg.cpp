#include "manyImages.h"
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
	cv::Mat img_1 = cv::imread("lena.jpg");
	cv::Mat img_2 = cv::imread("mosaic.bmp");

	cv::Mat dst;
	std::vector<cv::Mat> manyImg;
	manyImg.push_back(img_1);
	manyImg.push_back(img_2);

	ManyImages(manyImg, dst, 2);
	cv::imshow("ManyImgWindow", dst);
	cv::imwrite("mergeImg.png", dst);
	cv::waitKey(0);;
	return 0;
}