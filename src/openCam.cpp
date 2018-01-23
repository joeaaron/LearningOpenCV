#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <iostream>

using namespace cv;
char filename[100];			//声明一个字符型数组，用来存放图片命名

int main()
{
	VideoCapture capture(0);
	if (!capture.isOpened())     //判断是否打开摄像头
		return 1;
	bool stop(false);
	cv::Mat frame;        //用来存放读取的视频序列
	cv::Mat dst;
	namedWindow("capVideo");
	int i = 1;
	while (!stop)
	{
		if (!capture.read(frame))
			break;
		imshow("capVideo", frame);
		//Esc键停止
		char c = cvWaitKey(33);
		if (13 == c)
		{
			sprintf(filename, "%s%d%s", "x", i++, ".bmp");//保存的图片名，可以把保存路径写在filename中；
			imwrite(filename, frame);
		}
	
		
		if (27 == c)
			break;
	}

	capture.release();

}