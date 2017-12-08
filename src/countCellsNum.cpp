#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>

cv::Mat searchConcaveRegion(std::vector<std::vector<cv::Point> >hull, cv::Mat &src);
std::vector<cv::Point2f> searchConcavePoint(cv::Mat &src);

void main()
{
	cv::Mat img = cv::imread("cell.jpg", 1);
	cv::Mat gray, gray_back;

	cv::cvtColor(img, gray, CV_BGR2GRAY);
	cv::threshold(gray, gray, 0, 255, CV_THRESH_OTSU);
	gray_back = gray.clone();

	//��ȡ����
	std::vector<std::vector<cv::Point>>contours;
	findContours(gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	drawContours(img, contours, -1, cv::Scalar(0, 0, 255), 2);

	cv::Mat aotu = searchConcaveRegion(contours, gray_back);
	std::vector<cv::Point2f> pt;
	pt = searchConcavePoint(aotu);

	line(img, pt[pt.size() - 2], pt[pt.size() - 1], cv::Scalar(255, 0, 0), 2, 8, 0);

	cv::imshow("�и���", img);
	cv::waitKey(0);
}

/**
* @brief searchConcaveRegion Ѱ�Ұ�����
* @param hull  ͹���㼯
* @param src  ԭͼ�񣨶�ֵͼ��
* @return ���� ͼ������
*/
cv::Mat searchConcaveRegion(std::vector<std::vector<cv::Point> >contours, cv::Mat &src)
{
	if (src.empty())
		return cv::Mat();

	//����ÿ��������Ѱ����͹��  
	std::vector<	std::vector<cv::Point>>hull(contours.size());
	for (unsigned int i = 0; i < contours.size(); ++i)
	{
		convexHull(cv::Mat(contours[i]), hull[i], false);
	}

	//������������͹��  
	cv::Mat drawing = cv::Mat::zeros(src.size(), CV_8UC1);
	for (unsigned int i = 0; i < contours.size(); ++i)
	{
		drawContours(drawing, hull, i, cv::Scalar(255), -1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
		drawContours(drawing, contours, i, cv::Scalar(0), -1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
	}

	cv::medianBlur(drawing, drawing, 3);

	cv::imshow("������", drawing);

	return drawing;
}

/**
* @brief searchConcavePoint
* @param src ������ͼ��
* @return ����ƥ��õİ���ԣ�2����
*/
std::vector<cv::Point2f> searchConcavePoint(cv::Mat &src)
{
	std::vector<cv::Point2f> ConcavePoint;
	//����Ѱ��    
	std::vector<std::vector<cv::Point> > contour;//�����洢����    
	std::vector<cv::Vec4i> hierarchys;
	findContours(src, contour, hierarchys,
		CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));  //Ѱ������    

	//����������2Ҫ�˳�    
	if (contour.size()<2)
		return ConcavePoint;

	//�������������С����    
	std::sort(contour.begin(), contour.end(), [](const std::vector<cv::Point> &s1,
		const std::vector<cv::Point> &s2){
		double a1 = contourArea(s1);
		double a2 = contourArea(s2);
		return a1>a2;
	});

	int minDistance = 100000000;//��̾���    
	for (int i = 0; i<contour[0].size(); ++i)
		for (int j = 0; j<contour[1].size(); ++j)
		{
			//ŷ�Ͼ���    
			int d = std::sqrt(std::pow((contour[0][i].x - contour[1][j].x), 2) +
				std::pow((contour[0][i].y - contour[1][j].y), 2));
			if (minDistance>d)
			{
				minDistance = d;
				ConcavePoint.push_back(contour[0][i]);
				ConcavePoint.push_back(contour[1][j]);
			}
		}
	std::cout << "ConcavePoint0:" << ConcavePoint[ConcavePoint.size() - 2].x << "," << ConcavePoint[ConcavePoint.size() - 2].y << std::endl;
	std::cout << "ConcavePoint1:" << ConcavePoint[ConcavePoint.size() - 1].x << "," << ConcavePoint[ConcavePoint.size() - 1].y << std::endl;

	return ConcavePoint;
}