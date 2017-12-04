// bwareaopen_test.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>

using namespace std;
using namespace cv;

//void bwareaopen(Mat &data, int n);

void bwareaopen(Mat& image, double size)
{
	image.convertTo(image, CV_8UC1);
	Mat labels, stats, centroids;
	int nLabels = connectedComponentsWithStats(image.clone(), labels, stats, centroids);
	int N = nLabels - 1;

	Mat temparea = stats.col(4);
	Mat idxx;
	findNonZero(temparea > size, idxx);
	Mat zerores = Mat::zeros(image.rows, image.cols, CV_8UC1);
	for (int j = 1; j < idxx.rows; j++)
	{
		bitwise_or(zerores, labels == idxx.at<int>(j, 1), zerores);
	}

	image = zerores;
}


//void bwareaopen(Mat &data, int n)
//{
//	Mat labels, stats, centroids;
//	connectedComponentsWithStats(data, labels, stats, centroids, 8, CV_16U);
//	int regions_count = stats.rows - 1;
//	int regions_size, regions_x1, regions_y1, regions_x2, regions_y2;
//
//	for (int i = 1; i <= regions_count; i++)
//	{
//		regions_size = stats.ptr<int>(i)[4];
//		if (regions_size < n)
//		{
//			regions_x1 = stats.ptr<int>(i)[0];
//			regions_y1 = stats.ptr<int>(i)[1];
//			regions_x2 = regions_x1 + stats.ptr<int>(i)[2];
//			regions_y2 = regions_y1 + stats.ptr<int>(i)[3];
//
//			for (int j = regions_y1; j<regions_y2; j++)
//			{
//				for (int k = regions_x1; k<regions_x2; k++)
//				{
//					if (labels.ptr<ushort>(j)[k] == i)
//						data.ptr<uchar>(j)[k] = 0;
//				}
//			}
//		}
//	}
//}

int main()
{
	Mat input = imread("1_copy.bmp", 0);
	Mat output = input.clone();
	bwareaopen(output, 1000);
	imshow("input", input);
	imshow("output", output);
	imwrite("output.bmp", output);
	waitKey(0);
    return 0;
}

