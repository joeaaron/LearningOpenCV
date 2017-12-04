#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
	cv::Mat srcImg = cv::imread("badPoint.jpg");
	cv::Mat grayImg,thresholdImg;
	cvtColor(srcImg, grayImg, CV_BGR2GRAY);
	cv::threshold(grayImg, thresholdImg, 200, 255, cv::THRESH_BINARY);
	
	for (int i = 0; i < grayImg.rows; ++i)
	{
		uchar *p = grayImg.ptr<uchar>(i);
		for (int j = 0; j < grayImg.cols; ++j)
			if (p[j] > 155 && p[j] < 200)
			{
				std::cout << i << " "<< j << std::endl;
				//cv::circle(srcImg, cv::Point(i, j), 1, cv::Scalar(0, 0, 255), 1);
			}
	}

	return 0;
}