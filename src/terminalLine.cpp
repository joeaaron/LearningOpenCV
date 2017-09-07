#include <opencv2/opencv.hpp>
#include <iostream>

const int ROWVALUE = 40;
const int COLVALUE = 120;
const int COUNTNUM = 50;

cv::Point FindLeftPoint(int rows, int cols, cv::Mat srcGray)
{
	int num = 0;
	cv::Mat img;
	srcGray.copyTo(img);
	std::vector<cv::Point> ptSet;
	cv::Point ptLeft;
	///第1列
	for (int i = 0; i < rows; i++)
	{
		int rowPixValue = img.at<uchar>(i, 0);

		if (rowPixValue < ROWVALUE)
			num = num + 1;
		else
			ptSet.push_back(cv::Point(0, i));
		//std::cout << pixelValue << std::endl;
	}

	///
	if (num > COUNTNUM)
	{
		ptLeft = ptSet[0];
		cv::circle(img, ptLeft, 5, cv::Scalar(0, 0, 255));
	}

	else
	{
		///第1行
		ptSet.clear();
		int newStart = 0;
		for (int j = 0; j < cols; j++)
		{
			int colPixValue = img.at<uchar>(0, j);
			if (colPixValue > COLVALUE)
			{
				newStart = j;
				break;
			}
		}
		for (newStart; newStart < cols; newStart++)
		{
			int colPixValue = img.at<uchar>(0, newStart);
			if (colPixValue < COLVALUE)
			{
				ptSet.push_back(cv::Point(newStart, 0));
				break;
			}

		}
		ptLeft = ptSet[0];
		cv::circle(img, ptLeft, 5, cv::Scalar(0, 0, 255));
	}
	return ptLeft;
}

cv::Point FindRightPoint(int rows, int cols, cv::Mat srcGray)
{
	int num = 0;
	cv::Mat img;
	srcGray.copyTo(img);
	cv::Point ptRight;
	std::vector<cv::Point> ptSet;
	///最后一列
	for (int i = 0; i < rows; i++)
	{
		int rowPixValue = img.at<uchar>(i, cols - 1);

		if (rowPixValue > COLVALUE)
		{
			ptSet.push_back(cv::Point(cols - 1, i));
			break;
		}
			
		else
			num = num + 1;
	}
	if (0 != num )
	{
		ptRight = ptSet[0];
		cv::circle(img, ptRight, 5, cv::Scalar(0, 0, 255));
	}
	return ptRight;
}

int main()
{
	cv::Mat srcImg = cv::imread("fro.bmp");
	//check if the image is read successfully
	if (srcImg.empty())
	{
		std::cout << "Can not load image." << std::endl;
		return -1;
	}
	cv::Mat srcGray;
	cv::cvtColor(srcImg, srcGray, cv::COLOR_BGR2GRAY);

	int rows = srcImg.rows;
	int cols = srcImg.cols;

	cv::Point lPt;
	cv::Point rPt;

	lPt = FindLeftPoint(rows, cols, srcGray);
	rPt = FindRightPoint(rows, cols, srcGray);
	
	cv::Scalar color(0, 0, 255);
	cv::line(srcImg, lPt, rPt, color, 1);
	return 0;
}