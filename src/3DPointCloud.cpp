//#include <pcl/io/io.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/cloud_viewer.h>

int user_data;
//相机内参，根据输入改动
const double u0 = 1329.49 / 4;//由于后面resize成原图的1/4所以有些参数要缩小相同倍数
const double v0 = 954.485 / 4;
const double fx = 6872.874 / 4;
const double fy = 6872.874 / 4;
const double Tx = 174.724;
const double doffs = 293.97 / 4;

void viewerOneOff(pcl::visualization::PCLVisualizer& viewer)
{
	viewer.setBackgroundColor(0.0, 0.0, 0.0);
}

int main()
{
	pcl::PointCloud<pcl::PointXYZRGB> cloud_a;
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
	
	cv::Mat color1 = cv::imread("im0.png");
	cv::Mat depth = cv::imread("Sword1_perfect_d.png");
	
	cv::Mat color;
	resize(color1, color, cv::Size(color1.cols / 4, color1.rows / 4), 0, 0, CV_INTER_LINEAR);

	int rowNumber = color.rows;
	int colNumber = color.cols;

	cloud_a.height = rowNumber;
	cloud_a.width = colNumber;
	cloud_a.points.resize(cloud_a.width * cloud_a.height);

	for (unsigned int u = 0; u < rowNumber; ++u)
	{
		for (unsigned int v = 0; v < colNumber; ++v)
		{
			unsigned int num = u * colNumber + v;
			double Xw = 0;
			double Yw = 0;
			double Zw = 0;

			Zw = fx * Tx;
			Xw = (v + 1 - u0) * Zw / fx;
			Yw = (u + 1 - v0) * Zw / fy;

			cloud_a.points[num].b = color.at<cv::Vec3b>(u, v)[0];
			cloud_a.points[num].g = color.at<cv::Vec3b>(u, v)[1];
			cloud_a.points[num].r = color.at<cv::Vec3b>(u, v)[2];

			cloud_a.points[num].x = Xw;
			cloud_a.points[num].y = Yw;
			cloud_a.points[num].z = Zw;
		}
	}

	*cloud = cloud_a;
	pcl::visualization::CloudViewer viewer("Cloud Viewer");
	viewer.showCloud(cloud);
	viewer.runOnVisualizationThreadOnce(viewerOneOff);

	while (!viewer.wasStopped())
	{
		user_data = 9;
	}
	return 0;
}