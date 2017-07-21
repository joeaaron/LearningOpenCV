#include <opencv2/opencv.hpp>
#include <vector>

std::vector<cv::Point> getPoints(cv::Mat &image, int value)
{
	int nl = image.rows;      //number of lines
	int nc = image.cols * image.channels();

	std::vector<cv::Point> points;
	for (int j = 0; j < nl; j++)
	{
		uchar* data = image.ptr<uchar>(j);
		for (int i = 0; i < nc; i++)
		{
			if (data[i] == value)
			{
				points.push_back(cv::Point(i, j));
			}
		}
	}
	return points;

}

void drawLine(cv::Mat& image, double theta, double rho, cv::Scalar color)
{
	if (theta < CV_PI / 4. || theta > 3.* CV_PI / 4.)// ~vertical line
	{
		cv::Point pt1(rho / cos(theta), 0);
		cv::Point pt2((rho - image.rows * sin(theta)) / cos(theta), image.rows);
		cv::line(image, pt1, pt2, cv::Scalar(255), 1);
	}
	else
	{
		cv::Point pt1(0, rho / sin(theta));
		cv::Point pt2(image.cols, (rho - image.cols * cos(theta)) / sin(theta));
		cv::line(image, pt1, pt2, color, 1);
	}
}


int main(int argc, char* argv[])
{
	cv::Mat image = imread("x2.bmp", cv::IMREAD_GRAYSCALE);
	if (image.empty())
		return -1;

	cv::threshold(image, image, 120, 255, cv::THRESH_BINARY);
	std::vector<cv::Point> points = getPoints(image, 255);
	cv::Vec4f line;
	cv::fitLine(points,
		line,
		CV_DIST_HUBER,
		0,
		0.01,
		0.01);

	double cos_theta = line[0];
	double sin_theta = line[1];
	double x0 = line[2], y0 = line[3];

	double phi = atan2(sin_theta, cos_theta) + CV_PI / 2.0;
	double rho = y0 * cos_theta - x0 * sin_theta;

	std::cout << "phi = " << phi / CV_PI * 180 << std::endl;
	std::cout << "rho = " << rho << std::endl;

	drawLine(image, phi, rho, cv::Scalar(0, 0, 255));
	double k = sin_theta / cos_theta;

	double b = y0 - k * x0;

	double x = 0;
	double y = k * x + b;
	std::cout << k << std::endl;
	std::cout << b << std::endl; 

	imshow("", image);
	return 0;
}