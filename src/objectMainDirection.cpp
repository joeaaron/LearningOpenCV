/*!
 * \file objectMainDirection.cpp
 *
 * \author Aaron
 * \date ʮ���� 2017
 *
 * http://www.cnblogs.com/jsxyhelu/p/7690699.html
 */
#include <iostream>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>

using namespace std;
using namespace cv;

//��ù�������Ҫ����
double getOrientation(vector<Point> &pts, Mat &img)
{
	//����pca���ݡ����������ǽ��������x��y��Ϊ����άѹ��data_pts��ȥ��
	Mat data_pts = Mat(pts.size(), 2, CV_64FC1);//ʹ��mat���������ݣ�Ҳ��Ϊ�˺���pca������Ҫ
	for (int i = 0; i < data_pts.rows; ++i)
	{
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
	}
	//ִ��PCA����
	PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);
	//�������Ҫ�������ڱ����У���Ӧ�ľ��������е㣬Ҳ��ͼ���е�
	Point pos = Point(pca_analysis.mean.at<double>(0, 0), pca_analysis.mean.at<double>(0, 1));
	//�洢��������������ֵ
	vector<Point2d> eigen_vecs(2);
	vector<double> eigen_val(2);
	for (int i = 0; i < 2; ++i)
	{
		eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0), pca_analysis.eigenvectors.at<double>(i, 1));
		eigen_val[i] = pca_analysis.eigenvalues.at<double>(i, 0);//ע�⣬����ط�ԭ����д����
	}
	//������/ͼ���е����СԲ
	circle(img, pos, 3, CV_RGB(255, 0, 255), 2);
	//�����ֱ�ߣ�����Ҫ�����ϻ���ֱ��
	line(img, pos, pos + 0.02 * Point(eigen_vecs[0].x * eigen_val[0], eigen_vecs[0].y * eigen_val[0]), CV_RGB(255, 255, 0));
	line(img, pos, pos + 0.02 * Point(eigen_vecs[1].x * eigen_val[1], eigen_vecs[1].y * eigen_val[1]), CV_RGB(0, 255, 255));
	//���ؽǶȽ��
	return atan2(eigen_vecs[0].y, eigen_vecs[0].x);
}

int main()
{
	//����ͼ��ת��Ϊ�Ҷ�
	Mat img = imread("pca1.png");
	Mat bw;
	cvtColor(img, bw, COLOR_BGR2GRAY);
	//��ֵ����
	threshold(bw, bw, 150, 255, CV_THRESH_BINARY);
	//Ѱ������
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(bw, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	//�����������ҵ�����
	for (size_t i = 0; i < contours.size(); ++i)
	{
		//����������С
		double area = contourArea(contours[i]);
		//ȥ����С���߹�����������򣨿�ѧ��������ʾ��
		if (area < 1e2 || 1e5 < area) continue;
		//��������
		drawContours(img, contours, i, CV_RGB(255, 0, 0), 2, 8, hierarchy, 0);
		//Ѱ��ÿһ�������ķ���
		getOrientation(contours[i], img);
	}
}

