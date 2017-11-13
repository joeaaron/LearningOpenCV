#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdint.h>
#include <iostream>	

int main()
{
	cv::Mat src = cv::imread("5.png");
	CV_Assert(!src.empty());

	cv::Mat srcGray;
	cvtColor(src, srcGray, cv::COLOR_BGR2GRAY);

	int currentBright = 0;
	int pt_x;
	int pt_y;

	for (int i = 0; i < src.cols; i++)
	{
		for (int j = 0; j < src.rows; j++)
		{
			uint8_t *pData = srcGray.ptr<uint8_t>(j) + i;
			if (*pData > currentBright)
			{
				currentBright = *pData;
				pt_x = i;
				pt_y = j;
			}
				
		}
	}

	std::cout << currentBright;

	cv::Mat result = src.clone();
	cv::circle(result, cv::Point(pt_x, pt_y), 2, cv::Scalar(0, 0, 255), 2);
	system("pause");
	return 0;
}