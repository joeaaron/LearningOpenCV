// The "Square Detector" program.
// It loads several images sequentially and tries to find squares in
// each image

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <math.h>
#include <string.h>

using namespace cv;
using namespace std;

RNG rng(12345);

int main(int /*argc*/, char** /*argv*/)
{
	Mat src = imread("pic7.jpg", 1);
	//输入图像
	Mat img;
	//灰度值归一化
	Mat bgr;
	//HSV图像
	Mat hsv;
	img = src.clone();
	blur(img, img, Size(3, 3));
	//彩色图像的灰度值归一化
	img.convertTo(bgr, CV_32FC3, 1.0 / 255, 0);
	//颜色空间转换
	cvtColor(bgr, hsv, COLOR_BGR2HSV);

	vector<Mat> mv;
	split(hsv, mv);//分为3个通道  
	Mat s = mv[1];

	threshold(s, s, 0.5, 255, THRESH_BINARY);
	s.convertTo(s, CV_8U, 1, 0);
 	vector<vector<Point> > contours, contours2;
	vector<Vec4i> hierarchy;
	findContours(s, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));
	// Print contours' length轮廓的个数
	cout << "Contours: " << contours.size() << endl;
	vector<vector<Point>>::const_iterator itContours = contours.begin();
	for (; itContours != contours.end(); ++itContours) {

		cout << "Size: " << itContours->size() << endl;//每个轮廓包含的点数
	}

	// draw black contours on white image
	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
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

	Mat _drawing = Mat::zeros(src.size(), CV_8UC3);
	drawContours(_drawing, polyContours, maxArea, Scalar(0, 0, 255/*rand() & 255, rand() & 255, rand() & 255*/), 2);
	
	vector<Point>  hull;
	convexHull(polyContours[maxArea], hull, true);    //检测该轮廓的凸包
	
	for (int i = 0; i < hull.size(); ++i){
		circle(_drawing, polyContours[maxArea][i], 10, Scalar(rand() & 255, rand() & 255, rand() & 255), 3);
	}
	addWeighted(_drawing, 0.5, src, 0.5, 0, src);

	for (int i = 0; i < hull.size(); i++)
		cout << hull.at(i) << endl;

	waitKey();
	return 0;
}
