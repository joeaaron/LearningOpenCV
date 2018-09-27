#include <opencv2\opencv.hpp>  
using namespace cv;

//-----------------------------------【main( )函数】--------------------------------------------
//		描述：控制台应用程序的入口函数，我们的程序从这里开始
//-------------------------------------------------------------------------------------------------
int main()
{
	//【1】读入视频
	VideoCapture capture("LaneDetection.avi");
	int i = 0;
	// open the cam
	//VideoCapture capture(0);

	int count = 0;
	//【2】循环显示每一帧
	while (1)
	{
		Mat frame;//定义一个Mat变量，用于存储每一帧的图像
		capture >> frame;  //读取当前帧
		
		if (!frame.empty()) //判断当前帧是否捕捉成功 **这步很重要  
			imshow("读取视频", frame);  //显示当前帧 
		else
			std::cout << "can not ";
		if (waitKey(3) == 27)
			break;

		char file[100]; 

		if (waitKey(1) == 's')
		{
			sprintf(file, "x%d.bmp", count + 1);
			imwrite(file, frame);
		}

		count++;
			
		
	}
	return 0;
}
