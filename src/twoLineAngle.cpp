#include <opencv2/opencv.hpp>
#include <iostream>

///以pt1为基准
float getAngleOfTwoVector(cv::Point2f &pt1, cv::Point2f &pt2, cv::Point2f &c)
{
	float theta = atan2(pt1.x - c.x, pt1.y - c.y) - atan2(pt2.x - c.x, pt2.y - c.y);
	if (theta > CV_PI)
		theta -= 2 * CV_PI;
	if (theta < -CV_PI)
		theta += 2 * CV_PI;

	theta = theta * 180.0 / CV_PI;
	return theta;
}

void main()
{
	cv::Point2f c(0, 0);
	cv::Point2f pt1(0, -1);
	cv::Point2f pt2(-1, 0);

	float theta = getAngleOfTwoVector(pt1, pt2, c);

	std::cout << "theta" << theta << std::endl;
	system("pause");
	return;
}