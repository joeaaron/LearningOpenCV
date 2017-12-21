#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;
const int STANDARD = 275;

Mat ellipseDetect(cv::Mat src, cv::Mat& edst)
{
	cv::Mat hdst;
	edst = src.clone();
	cv::Mat srcImg = src.clone();
	cvtColor(srcImg, srcImg, CV_BGR2GRAY);
	Canny(srcImg, hdst, 300, 500, 3);    //>500 only circle left
	Mat cimage = Mat::zeros(srcImg.size(), CV_8UC3);
	//����  
	vector<vector<Point>> contours;
	findContours(hdst, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	for (size_t i = 0; i < contours.size(); i++)
	{
		//��ϵĵ�����Ϊ6  
		size_t count = contours[i].size();
		if (count < 6)
			continue;

		//��Բ���  
		RotatedRect box = fitEllipse(contours[i]);

		//�������ȴ���30�����ų����������  
		if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 30)
			continue;

		//����׷�ٳ�������  
		drawContours(edst, contours, (int)i, Scalar::all(255), 1, 8);

		//������ϵ���Բ  
		ellipse(edst, box, Scalar(0, 0, 255), 1, CV_AA);
	}

	imshow("detected ellipse", edst);
	return edst;
}

Mat lineDetect(cv::Mat src, cv::Mat& ldst)
{
	cv::Mat dst;
	cv::Mat srcImg = src.clone();
	ldst = src.clone();
	Canny(srcImg, dst, 30, 210, 3);
#if 0
	vector<Vec2f> lines;
	/*���� Hough �任�ڶ�ֵͼ�����ҵ�ֱ��
	CvSeq* cvHoughLines2( CvArr* image, void* line_storage, int method,
	double rho, double theta, int threshold,
	double param1=0, double param2=0 );
	line_storage :��⵽���߶δ洢��.
	Hough �任���������������������֮һ��
	CV_HOUGH_STANDARD - ��ͳ���׼ Hough �任. ÿһ���߶������������� (��, ��) ��ʾ��
	���� �� ��ֱ����ԭ�� (0,0) ֮��ľ��룬�� �߶��� x-��֮��ļнǡ���ˣ�������
	�ͱ����� CV_32FC2 type.
	CV_HOUGH_PROBABILISTIC - ���� Hough �任(���ͼ�����һЩ�������Էָ��Ч�ʸ���).
	�������߶ηָ�����������߶Ρ�ÿ���ָ��������յ�����ʾ�����Ծ��󣨻򴴽������У�
	������ CV_32SC4.
	CV_HOUGH_MULTI_SCALE - ��ͳ Hough �任�Ķ�߶ȱ��֡�
	�߶εı��뷽ʽ�� CV_HOUGH_STANDARD ��һ�¡�
	rho ��������ص�λ�ľ��뾫�ȡ�
	theta ���Ȳ����ĽǶȾ��ȡ�*/
	HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);

	for (size_t i = 0; i < lines.size(); i++)//����õ�����������
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
	}
#else
	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 10);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		if (abs(l[1] - STANDARD) < 10)
			line(ldst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}
	imshow("detected lines", ldst);
	return ldst;
#endif
}

int main(int argc, char** argv)
{
	Mat edstImg, ldstImg;
	
	const char* filename = "btb3.bmp";
	Mat srcImg = imread(filename);

	//�����Բ
	ellipseDetect(srcImg, edstImg);

	//����
	lineDetect(srcImg, ldstImg);

	imshow("result_1", edstImg);
	imwrite("result1.bmp", edstImg);
	imshow("result_2", ldstImg);
	imwrite("result2.bmp", ldstImg);

	waitKey();
	return 0;
}