#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
using namespace cv;
using namespace std;
//�ҵ�����ȡ���������ĵ�
//����ȡ������С�����εı߽���ÿ���ܳ���������ȡһ��������꣬������ȡ�ĸ����ƽ�����꣨��ΪС�����εĴ������ģ�
Point Center_cal(vector<vector<Point> > contours, int i)
{
	int centerx = 0, centery = 0, n = contours[i].size();
	centerx = (contours[i][n / 4].x + contours[i][n * 2 / 4].x + contours[i][3 * n / 4].x + contours[i][n - 1].x) / 4;
	centery = (contours[i][n / 4].y + contours[i][n * 2 / 4].y + contours[i][3 * n / 4].y + contours[i][n - 1].y) / 4;
	Point point1 = Point(centerx, centery);
	return point1;
}

int main(int argc, char** argv[])
{
	Mat src = imread("qr.png", 1);
	resize(src, src, Size(800, 600));//��׼��С
	Mat src_gray;
	Mat src_all = src.clone();
	Mat threshold_output;
	vector<vector<Point> > contours, contours2;
	vector<Vec4i> hierarchy;
	//Ԥ����
	cvtColor(src, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3)); //ģ����ȥ��ë��
	threshold(src_gray, threshold_output, 100, 255, THRESH_OTSU);
	//Ѱ������ 
	//��һ������������ͼ�� 2ֵ����
	//�ڶ����������ڴ�洢����FindContours�ҵ��������ŵ��ڴ����档
	//�����������ǲ㼶��**[Next, Previous, First_Child, Parent]** ��vector
	//���ĸ����������ͣ��������ṹ
	//����������ǽڵ����ģʽ��������ȫ��Ѱ��
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));
	//����ɸѡ
	int c = 0, ic = 0, area = 0;
	int parentIdx = -1;
	for (int i = 0; i < contours.size(); i++)
	{
		//hierarchy[i][2] != -1 ��ʾ���������������
		if (hierarchy[i][2] != -1 && ic == 0)
		{
			parentIdx = i;
			ic++;
		}
		else if (hierarchy[i][2] != -1)
		{
			ic++;
		}
		//���������0
		else if (hierarchy[i][2] == -1)
		{
			ic = 0;
			parentIdx = -1;
		}
		//�ҵ���λ����Ϣ
		if (ic >= 2)
		{
			contours2.push_back(contours[parentIdx]);
			ic = 0;
			parentIdx = -1;
		}
	}
	//��䶨λ��
	for (int i = 0; i < contours2.size(); i++)
		drawContours(src_all, contours2, i, CV_RGB(0, 255, 0), -1);
	//���Ӷ�λ��
	Point point[3];
	for (int i = 0; i < contours2.size(); i++)
	{
		point[i] = Center_cal(contours2, i);
	}

	line(src_all, point[0], point[1], Scalar(0, 0, 255), 2);
	line(src_all, point[1], point[2], Scalar(0, 0, 255), 2);
	line(src_all, point[0], point[2], Scalar(0, 0, 255), 2);

	imshow("���", src_all);
	waitKey(0);
	return(0);
}
