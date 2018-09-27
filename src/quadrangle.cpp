// The "Square Detector" program.
// It loads several images sequentially and tries to find squares in
// each image

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <math.h>
#include <string>

using namespace cv;
using namespace std;

#define debug 0

bool x_sort(const cv::Point2f & m1, const cv::Point2f & m2)
{
	return m1.x < m2.x;
}

void CalcDstSize(const std::vector<cv::Point>& corners,int& h1, int& h2, int& w1, int& w2)
{
	if ((abs(corners[0].y - corners[3].y) < 300) && (abs(corners[1].y - corners[2].y) < 300)
		&& corners[0].x !=0 && corners[1].x != 0)
	{
		h1 = sqrt((corners[0].x - corners[3].x)*(corners[0].x - corners[3].x) + (corners[0].y - corners[3].y)*(corners[0].y - corners[3].y));
		h2 = sqrt((corners[1].x - corners[2].x)*(corners[1].x - corners[2].x) + (corners[1].y - corners[2].y)*(corners[1].y - corners[2].y));

		w1 = sqrt((corners[0].x - corners[1].x)*(corners[0].x - corners[1].x) + (corners[0].y - corners[1].y)*(corners[0].y - corners[1].y));
		w2 = sqrt((corners[2].x - corners[3].x)*(corners[2].x - corners[3].x) + (corners[2].y - corners[3].y)*(corners[2].y - corners[3].y));

	}
	
}


int main(int /*argc*/, char** /*argv*/)
{

	VideoCapture cap("1.mp4");
	if (!cap.isOpened()) // check if we succeeded
		return -1;
	namedWindow("src");
	for (;;)
	{
		Mat frame;
#if debug
		frame = imread("pic7.jpg");
#else
		cap >> frame; 
#endif
		if (frame.empty())
		{
			continue;
		}
		Mat img, bgr, hsv;

		img = frame.clone();

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

		////获取自定义核
		//Mat element = getStructuringElement(MORPH_RECT, Size(11, 11));
		////dilate(s, s, element);
		//morphologyEx(s, s, MORPH_CLOSE, element);
		vector<vector<Point> > contours, contours2;
		vector<Vec4i> hierarchy;
		findContours(s, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));
		// Print contours' length轮廓的个数
		//cout << "Contours: " << contours.size() << endl;
		vector<vector<Point>>::const_iterator itContours = contours.begin();
#if debug
		for (; itContours != contours.end(); ++itContours) {

			cout << "Size: " << itContours->size() << endl;//每个轮廓包含的点数
		}
#endif
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

		//2018-08-30
		///LSD line detection & find the cross points!

		vector<Point>  hull;
		convexHull(polyContours[maxArea], hull, true);    //检测该轮廓的凸包


		if (hull.size() != 4)
			continue;
		else
		{
			sort(hull.begin(), hull.end(), x_sort);  //注意先按x的大小给4个点排序
			int h1 = 0;
			int h2 = 0;
			int w1 = 0;
			int w2 = 0;
			CalcDstSize(hull, h1, h2, w1, w2);
					  
			if (h1 > 200 && h1 < 2000 
				&& h2 > 200 && h2 < 2000 
				&& w1 > 200 && w1 < 2000 
				&& w2 > 200 && w2 < 2000)
			{
				for (int i = 0; i < hull.size(); ++i){
					circle(_drawing, polyContours[maxArea][i], 10, Scalar(rand() & 255, rand() & 255, rand() & 255), 3);
				}

				addWeighted(_drawing, 0.5, img, 0.5, 0, img);

				imshow("result", img);
				if (waitKey(1) >= 0)
					break;
			}
		
			
		}
		imshow("ori", frame);
		if (waitKey(1) >= 0)
			break;
		
	}

	
	return 0;
}
