#include<iostream>    
#include<opencv2/opencv.hpp>    
#include "markDetector.h"

cv::Mat srcImg;

void GetImagePoint(vector<Marker> marker)
{
	Point2f imgPoint_;              //image point center of the board

	vector<Point2f> points = marker[0].m_points;
	double a0 = points[0].y - points[2].y;
	double b0 = points[2].x - points[0].x;
	double c0 = points[0].x * points[2].y - points[2].x * points[0].y;

	double a1 = points[1].y - points[3].y;
	double b1 = points[3].x - points[1].x;
	double c1 = points[1].x * points[3].y - points[3].x * points[1].y;
	
	double d = a0 * b1 - a1 * b0;
	imgPoint_.x = (b0 * c1 - b1 * c0) / d;
	imgPoint_.y = (a1 * c0 - a0 * c1) / d;
	circle(srcImg, imgPoint_, 3, Scalar(0, 0, 255), 2, 8);
	// show marker in image
	for (int i = 0; i < marker.size(); i++)
	{
		int sizeNum = marker[i].m_points.size();
		for (int j = 0; j < sizeNum; j++)
		{
			line(srcImg, marker[i].m_points[j], marker[i].m_points[(j + 1) % sizeNum], Scalar(0, 0, 255), 2, 8);
		}
		circle(srcImg, marker[i].m_points[0], 3, Scalar(0, 255, 255), 2, 8);//clockwise code the points
	}
	imshow("markerDetector", srcImg);
	cv::waitKey(5);

}

int main()
{
	vector<Marker> markers;
	srcImg = cv::imread("marker01.png");
	if (srcImg.empty())
		return -1;
	MarkProc* markProc = MarkProc::Create();
	markProc->ProcessFrame(srcImg, markers);
	GetImagePoint(markers);
}