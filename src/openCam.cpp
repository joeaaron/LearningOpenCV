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

	VideoWriter write;
	std::string outFile = "D://fcq.avi";

	bool stop(false);
	cv::Mat frame;				//用来存放读取的视频序列
	cv::Mat dst;
	namedWindow("capVideo");
	int i = 1;
	while (!stop)
	{
		if (!capture.read(frame))
			break;
		imshow("capVideo", frame);

		//获得帧率
		double r = capture.get(CV_CAP_PROP_FPS);
		std::cout << "帧率 = " << r;

		//设置帧率
		//capture.set(CV_CAP_PROP_FPS, 30);

		//获得帧的宽、高
		int w = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
		int h = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
		Size S(w, h);

		//打开视频文件，准备写入
		write.open(outFile, -1, r, S, true);
		//写入文件
		write.write(frame);
		//Esc键停止
		char c = cvWaitKey(33);
		if (13 == c)
		{
			sprintf(filename, "%s%d%s", "x", i++, ".bmp");		//保存的图片名，可以把保存路径写在filename中；
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