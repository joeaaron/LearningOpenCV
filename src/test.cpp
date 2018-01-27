//#include<iostream>    
//#include<opencv2/opencv.hpp>    
//#include "angle.h"
//
//int main()
//{
//	AngleProc* angleProc = AngleProc::Create();
//	double angle;
//
//	angleProc->Open(0, "out_camera_data.xml");
//	angleProc->Angle(angle);
//}

#include <iostream>  
#include <opencv2/opencv.hpp>  

using namespace std;
using namespace cv;

int main()
{

	cv::VideoCapture capl(0);
	cv::VideoCapture capr(1);

	int i = 0;

	cv::Mat src_imgl;
	cv::Mat src_imgr;

	char filename_l[15];
	char filename_r[15];
	while (capl.read(src_imgl) && capr.read(src_imgr))
	{

		cv::imshow("src_imgl", src_imgl);
		cv::imshow("src_imgr", src_imgr);

		char c = cv::waitKey(1);
		if (c == ' ') //按空格采集图像  
		{
			sprintf(filename_l, "img%d.jpg", i);
			imwrite(filename_l, src_imgl);
			sprintf(filename_r, "right%d.jpg", i++);
			imwrite(filename_r, src_imgr);
		}
		if (c == 'q' || c == 'Q') // 按q退出  
		{
			break;
		}


	}

	return 0;
}
