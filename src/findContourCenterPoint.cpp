#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"


using namespace cv;
using namespace std;


RNG rng12345(12345);
int main()
{
	Mat matSrc = imread("shapes_and_colors.jpg", 0);
	GaussianBlur(matSrc, matSrc, Size(5, 5), 0);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//阈值
	threshold(matSrc, matSrc, 60, 255, THRESH_BINARY);
	//寻找轮廓
	findContours(matSrc.clone(), contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
	/// 计算矩
	vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], false);
	}
	///  计算中心矩:
	vector<Point2f> mc(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}
	/// 绘制轮廓
	Mat drawing = Mat::zeros(matSrc.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng12345.uniform(0, 255), rng12345.uniform(0, 255), rng12345.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		circle(drawing, mc[i], 4, color, -1, 8, 0);
	}
	waitKey();
	return 0;

};