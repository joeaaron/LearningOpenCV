#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

int main()
{
	cv::Mat src = cv::imread("seal_1.png");
	cv::Mat srcGray;
	cvtColor(src, srcGray, CV_BGR2GRAY);

	if (src.empty())
	{
		std::cout << "failed to open image!\n";
		return -1;
	}

	//全局二值化
	int threshold = 90;
	cv::Mat binary;
	cv::threshold(srcGray, binary, threshold, 255, CV_THRESH_BINARY);

	std::vector<cv::Mat> channels;
	split(src, channels);
	cv::Mat red = channels[2];
	cv::Mat blue = channels[0];
	cv::Mat green = channels[1];

	cv::Mat redBinary;
	cv::threshold(red, redBinary, threshold, 255, CV_THRESH_BINARY);

	cv::imshow("src", src);
	cv::imshow("gray", srcGray);
	cv::imshow("binary", binary);
	cv::imshow("red channel", red);
	cv::imshow("blue channel", blue);
	cv::imshow("green channel", green);
	cv::imshow("red+binary", redBinary);

	cv::waitKey();


}