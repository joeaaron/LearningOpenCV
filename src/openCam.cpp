#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <iostream>

using namespace cv;

int main()
{
	VideoCapture capture(0);
	if (!capture.isOpened())     //�ж��Ƿ������ͷ
		return 1;
	bool stop(false);
	cv::Mat frame;        //������Ŷ�ȡ����Ƶ����
	cv::Mat dst;
	namedWindow("capVideo");
	while (!stop)
	{
		if (!capture.read(frame))
			break;
		imshow("capVideo", frame);

		//Esc��ֹͣ
		char c = cvWaitKey(33);
		if (27 == c)
			break;
	}

	capture.release();

}