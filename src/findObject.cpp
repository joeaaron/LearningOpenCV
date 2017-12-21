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
	//轮廓  
	vector<vector<Point>> contours;
	findContours(hdst, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	for (size_t i = 0; i < contours.size(); i++)
	{
		//拟合的点至少为6  
		size_t count = contours[i].size();
		if (count < 6)
			continue;

		//椭圆拟合  
		RotatedRect box = fitEllipse(contours[i]);

		//如果长宽比大于30，则排除，不做拟合  
		if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 30)
			continue;

		//画出追踪出的轮廓  
		drawContours(edst, contours, (int)i, Scalar::all(255), 1, 8);

		//画出拟合的椭圆  
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
	/*利用 Hough 变换在二值图像中找到直线
	CvSeq* cvHoughLines2( CvArr* image, void* line_storage, int method,
	double rho, double theta, int threshold,
	double param1=0, double param2=0 );
	line_storage :检测到的线段存储仓.
	Hough 变换变量，是下面变量的其中之一：
	CV_HOUGH_STANDARD - 传统或标准 Hough 变换. 每一个线段由两个浮点数 (ρ, θ) 表示，
	其中 ρ 是直线与原点 (0,0) 之间的距离，θ 线段与 x-轴之间的夹角。因此，矩阵类
	型必须是 CV_32FC2 type.
	CV_HOUGH_PROBABILISTIC - 概率 Hough 变换(如果图像包含一些长的线性分割，则效率更高).
	它返回线段分割而不是整个线段。每个分割用起点和终点来表示，所以矩阵（或创建的序列）
	类型是 CV_32SC4.
	CV_HOUGH_MULTI_SCALE - 传统 Hough 变换的多尺度变种。
	线段的编码方式与 CV_HOUGH_STANDARD 的一致。
	rho 与象素相关单位的距离精度。
	theta 弧度测量的角度精度。*/
	HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);

	for (size_t i = 0; i < lines.size(); i++)//将求得的线条画出来
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

	//检测椭圆
	ellipseDetect(srcImg, edstImg);

	//检测边
	lineDetect(srcImg, ldstImg);

	imshow("result_1", edstImg);
	imwrite("result1.bmp", edstImg);
	imshow("result_2", ldstImg);
	imwrite("result2.bmp", ldstImg);

	waitKey();
	return 0;
}