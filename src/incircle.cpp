#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

typedef vector<Point> VP;
VP FindBigestContour(Mat src){
	int imax = 0; //代表最大轮廓的序号
	int imaxcontour = -1; //代表最大轮廓的大小
	std::vector<std::vector<cv::Point>>contours;
	findContours(src, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); i++){
		int itmp = contourArea(contours[i]);//这里采用的是轮廓大小
		if (imaxcontour < itmp){
			imax = i;
			imaxcontour = itmp;
		}
	}
	return contours[imax];
}

int main(int argc, char* argv[])
{
	Mat src = imread("incircle.png");
	if (src.empty())
	{
		return -1;
	}
	Mat temp;
	cvtColor(src, temp, COLOR_BGR2GRAY);
	threshold(temp, temp, 100, 255, THRESH_OTSU);
	imshow("src", temp);
	//寻找最大轮廓
	VP VPResult = FindBigestContour(temp);
	//寻找最大内切圆
	int dist = 0;
	int maxdist = 0;
	Point center;
	for (int i = 0; i < src.cols; i++)
	{
		for (int j = 0; j < src.rows; j++)
		{
			dist = pointPolygonTest(VPResult, cv::Point(i, j), true);
			if (dist > maxdist)
			{
				maxdist = dist;
				center = cv::Point(i, j);
			}
		}
	}
	//绘制结果
	circle(src, center, maxdist, Scalar(0, 0, 255));
	imshow("dst", src);
	waitKey();
}