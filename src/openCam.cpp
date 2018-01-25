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

	VideoWriter write;
	std::string outFile = "D://fcq.avi";

	bool stop(false);
	cv::Mat frame;				//������Ŷ�ȡ����Ƶ����
	cv::Mat dst;
	namedWindow("capVideo");
	int i = 1;
	while (!stop)
	{
		if (!capture.read(frame))
			break;
		imshow("capVideo", frame);

		//���֡��
		double r = capture.get(CV_CAP_PROP_FPS);
		std::cout << "֡�� = " << r;

		//����֡��
		//capture.set(CV_CAP_PROP_FPS, 30);

		//���֡�Ŀ���
		int w = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
		int h = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
		Size S(w, h);

		//����Ƶ�ļ���׼��д��
		write.open(outFile, -1, r, S, true);
		//д���ļ�
		write.write(frame);
		//Esc��ֹͣ
		char c = cvWaitKey(33);
		if (13 == c)
		{
			sprintf(filename, "%s%d%s", "x", i++, ".bmp");		//�����ͼƬ�������԰ѱ���·��д��filename�У�
			imwrite(filename, frame);
		}
	
		
		if (27 == c)
			break;
	}

	capture.release();
	write.release();
	destroyAllWindows();
	return 0;
}