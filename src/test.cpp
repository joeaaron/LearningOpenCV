
#include<iostream>
#include<opencv2/opencv.hpp>
#include<vector>

using namespace cv;
using namespace std;

int main()
{
	VideoCapture cap("1.mp4");
	
	if (!cap.isOpened()) // check if we succeeded
		return -1;

	for (;;)
	{
		Mat frame;
		cap >> frame;
		/*Mat srcImage = imread("pic7.jpg");
		imshow("【原图】", srcImage);*/
		Mat img, bgr, hsv;
		img = frame.clone();
		//因为强角点检测函数的输入图像是一个单通道的图像，所以，先对原图像进行图像空间的转换
		blur(img, img, Size(3, 3));
		//彩色图像的灰度值归一化
		img.convertTo(bgr, CV_32FC3, 1.0 / 255, 0);
		//颜色空间转换
		cvtColor(bgr, hsv, COLOR_BGR2HSV);

		vector<Mat> mv;
		split(hsv, mv);//分为3个通道  
		Mat s = mv[1];

		threshold(s, s, 0.3, 255, THRESH_BINARY);
		s.convertTo(s, CV_8U, 1, 0);
		vector<vector<Point> > contours, contours2;
		vector<Vec4i> hierarchy;
		findContours(s, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));

		// draw black contours on white image
		Mat drawing = Mat::zeros(img.size(), CV_8UC3);
		drawContours(drawing, contours,
			-1, // draw all contours
			Scalar(255), // in black
			2); // with a thickness of 2

		vector<vector<Point>> polyContours(contours.size());
		int maxArea = 0;
		for (int index = 0; index < contours.size(); index++){
			if (contourArea(contours[index]) > contourArea(contours[maxArea]))
				maxArea = index;
			approxPolyDP(contours[index], polyContours[index], 10, true);
		}

		Mat _drawing = Mat::zeros(img.size(), CV_8UC3);
		drawContours(_drawing, polyContours, maxArea, Scalar(0, 0, 255/*rand() & 255, rand() & 255, rand() & 255*/), 2);
		cvtColor(_drawing, _drawing, CV_BGRA2GRAY);
		//开始进行强角点检测
		//先配置需要的函数参数
		vector<Point2f> dstPoint2f;
		goodFeaturesToTrack(_drawing, dstPoint2f, 4, 0.3, 10, Mat(), 3);

		//遍历每个点，进行绘制，便于显示
		Mat dstImage;
		frame.copyTo(dstImage);
		for (int i = 0; i < (int)dstPoint2f.size(); i++)
		{
			circle(dstImage, dstPoint2f[i], 3, Scalar(theRNG().uniform(0, 255), theRNG().uniform(0, 255), theRNG().uniform(0, 255))
				, 2, 8);
		}

		imshow("【检测到的角点图】", dstImage);

		waitKey(1);
	}
	return 0;
}