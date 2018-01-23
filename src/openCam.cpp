#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <iostream>

using namespace cv;
char filename[100];			//����һ���ַ������飬�������ͼƬ����

int main()
{
	VideoCapture capture(0);
	if (!capture.isOpened())     //�ж��Ƿ������ͷ
		return 1;
	bool stop(false);
	cv::Mat frame;        //������Ŷ�ȡ����Ƶ����
	cv::Mat dst;
	namedWindow("capVideo");
	int i = 1;
	while (!stop)
	{
		if (!capture.read(frame))
			break;
		imshow("capVideo", frame);
		//Esc��ֹͣ
		char c = cvWaitKey(33);
		if (13 == c)
		{
			sprintf(filename, "%s%d%s", "x", i++, ".bmp");//�����ͼƬ�������԰ѱ���·��д��filename�У�
			imwrite(filename, frame);
		}
	
		
		if (27 == c)
			break;
	}

	capture.release();

}