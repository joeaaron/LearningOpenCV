#include<iostream>    
#include<opencv2/opencv.hpp>    
#include<vector>    

using namespace std;
using namespace cv;

int g_CannyThred = 180, g_CannyP = 0, g_CannySize = 0, g_HoughThred = 105, g_HoughThick = 9;
int g_Blue = 255, g_Green = 255, g_Red = 0;
int g_nWay = 0;
int g_nHoughLineMax = 10, g_nHoughLineMin = 50;

int g_nDp = 0;
int g_nMinDist = 5;
int g_nMinRadius = 0, g_nMaxRadius = 0;

int main()
{
	//��·��Ϊϵͳ�����ͼƬλ��·��  
	Mat srcImage = imread("btb3.bmp");

	/*Mat srcImage = imread("C:\\Users\\Yao%20Yao\\AppData\\Local\\Temp\\1.jpg");*/
	imshow("��ԭͼ��", srcImage);

	Mat grayImage;
	cvtColor(srcImage, grayImage, CV_BGR2GRAY);
	GaussianBlur(grayImage, grayImage, Size(9, 9), 2, 2);

	Mat cannyImage;
	vector<Vec3f> circles;

	namedWindow("�����������ڡ�", 0);
	createTrackbar("dp", "�����������ڡ�", &g_nDp, 100, 0);
	createTrackbar("MinDist", "�����������ڡ�", &g_nMinDist, 100, 0);
	createTrackbar("CannyThred", "�����������ڡ�", &g_CannyThred, 300, 0);
	createTrackbar("HoughThred", "�����������ڡ�", &g_HoughThred, 255, 0);
	createTrackbar("Blue", "�����������ڡ�", &g_Blue, 255, 0);
	createTrackbar("Green", "�����������ڡ�", &g_Green, 255, 0);
	createTrackbar("Red", "�����������ڡ�", &g_Red, 255, 0);
	createTrackbar("Bgr/Gray", "�����������ڡ�", &g_nWay, 1, 0);
	createTrackbar("HoughThred", "�����������ڡ�", &g_HoughThred, 200, 0);
	createTrackbar("MinRadius", "�����������ڡ�", &g_nMinRadius, 100, 0);
	createTrackbar("MaxRadius", "�����������ڡ�", &g_nMaxRadius, 100, 0);
	createTrackbar("HoughThick", "�����������ڡ�", &g_HoughThick, 100, 0);

	char key;
	Mat dstImage;
	while (1)
	{
		HoughCircles(grayImage, circles, CV_HOUGH_GRADIENT, (double)((g_nDp + 1.5)), (double)g_nMinDist + 1
			, (double)g_CannyThred + 1, g_HoughThred + 1, g_nMinRadius, g_nMaxRadius);

		/*HoughCircles(grayImage, circles, CV_HOUGH_GRADIENT, 1.5, 10, 200, 100, 0, 0);*/

		//��ʾ�߶�    
		for (size_t i = 0; i < circles.size(); i++)
		{
			if (g_nWay)
				grayImage.copyTo(dstImage);
			else
				srcImage.copyTo(dstImage);

			circle(dstImage, Point(cvRound(circles[i][0]), cvRound(circles[i][1])), cvRound(circles[i][2])
				, Scalar(g_Blue, g_Green, g_Red), g_HoughThick);
		}

		imshow("�������", dstImage);

		key = waitKey(1);
		if (key == 27)
			break;
	}

	return 0;
}
