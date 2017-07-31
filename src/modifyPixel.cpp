#include <opencv2/opencv.hpp>
#include <iostream>

void Invert(cv::Mat& img, const uchar* const lookup)
{
	int rows = img.rows;
	int cols = img.cols * img.channels();
	for (int i = 0; i < rows; i++)
	{
		uchar* p = img.ptr<uchar>(i);
		for (int j = 0; j < cols; j++)
			p[j] = lookup[p[j]];
	}
}

int main()
{
	cv::Mat src = cv::imread("robot.jpg");
	if (!src.data)
	{
		std::cout << "error! The img is not built!" << std::endl;
		return -1;
	}
	//为了演示效果，将图片转为灰度图
	cv::Mat img1 = src;
	cv::imshow("First", img1);
	//建立查找表
#if 0
	uchar lookup[256];
	for (int i = 0; i < 256; i++)
		lookup[i] = 255 - i;
	//调用自定义图像取反函数
	invert(img1, lookup);
#endif
	cv::Mat lookupTable(1, 256, CV_8U);
	uchar* p = lookupTable.data;
	for (int i = 0; i < 256; i++)
		p[i] = 255 - i;
	//通过LUT函数实现图像取反
	LUT(img1, lookupTable, img1);
	cv::imshow("Second",img1);
	cv::waitKey();
	return 0;
}