#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <iostream>

using namespace cv;

int main()
{
	VideoCapture capture(0);
	if (!capture.isOpened())     //判断是否打开摄像头
		return 1;
	bool stop(false);
	cv::Mat frame;        //用来存放读取的视频序列
	cv::Mat dst;
	namedWindow("capVideo");
	while (!stop)
	{
		if (!capture.read(frame))
			break;
		imshow("capVideo", frame);

		//Esc键停止
		char c = cvWaitKey(33);
		if (27 == c)
			break;
	}

	capture.release();

}