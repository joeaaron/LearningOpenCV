#define _USE_MATH_DEFINES
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <cmath>
#include <string>
#include <numeric>      // std::accumulate
#include <tuple>
#include <stack>

using namespace cv;
using namespace std;

typedef struct _Feather
{
	int label;              // 连通域的label值
	int area;               // 连通域的面积
	Rect boundingbox;       // 连通域的外接矩形框
} Feather;

/*
Input:
src: 待检测连通域的二值化图像
Output:
dst: 标记后的图像
featherList: 连通域特征的清单
return：
连通域数量。
*/
int bwLabel(Mat & src, Mat & dst, vector<Feather> & featherList)
{
	int rows = src.rows;
	int cols = src.cols;

	int labelValue = 0;
	Point seed, neighbor;
	stack<Point> pointStack;    // 堆栈

	int area = 0;               // 用于计算连通域的面积
	int leftBoundary = 0;       // 连通域的左边界，即外接最小矩形的左边框，横坐标值，依此类推
	int rightBoundary = 0;
	int topBoundary = 0;
	int bottomBoundary = 0;
	Rect box;                   // 外接矩形框
	Feather feather;

	featherList.clear();    // 清除数组

	dst.release();
	dst = src.clone();
	for (int i = 0; i < rows; i++)
	{
		uchar *pRow = dst.ptr<uchar>(i);
		for (int j = 0; j < cols; j++)
		{
			if (pRow[j] == 255)
			{
				area = 0;
				labelValue++;           // labelValue最大为254，最小为1.
				seed = Point(j, i);     // Point（横坐标，纵坐标）
				dst.at<uchar>(seed) = labelValue;
				pointStack.push(seed);

				area++;
				leftBoundary = seed.x;
				rightBoundary = seed.x;
				topBoundary = seed.y;
				bottomBoundary = seed.y;

				while (!pointStack.empty())
				{
					neighbor = Point(seed.x + 1, seed.y);
					if ((seed.x != (cols - 1)) && (dst.at<uchar>(neighbor) == 255))
					{
						dst.at<uchar>(neighbor) = labelValue;
						pointStack.push(neighbor);

						area++;
						if (rightBoundary < neighbor.x)
							rightBoundary = neighbor.x;
					}

					neighbor = Point(seed.x, seed.y + 1);
					if ((seed.y != (rows - 1)) && (dst.at<uchar>(neighbor) == 255))
					{
						dst.at<uchar>(neighbor) = labelValue;
						pointStack.push(neighbor);

						area++;
						if (bottomBoundary < neighbor.y)
							bottomBoundary = neighbor.y;

					}

					neighbor = Point(seed.x - 1, seed.y);
					if ((seed.x != 0) && (dst.at<uchar>(neighbor) == 255))
					{
						dst.at<uchar>(neighbor) = labelValue;
						pointStack.push(neighbor);

						area++;
						if (leftBoundary > neighbor.x)
							leftBoundary = neighbor.x;
					}

					neighbor = Point(seed.x, seed.y - 1);
					if ((seed.y != 0) && (dst.at<uchar>(neighbor) == 255))
					{
						dst.at<uchar>(neighbor) = labelValue;
						pointStack.push(neighbor);

						area++;
						if (topBoundary > neighbor.y)
							topBoundary = neighbor.y;
					}

					seed = pointStack.top();
					pointStack.pop();
				}
				box = Rect(leftBoundary, topBoundary, rightBoundary - leftBoundary, bottomBoundary - topBoundary);
				rectangle(src, box, 255);
				feather.area = area;
				feather.boundingbox = box;
				feather.label = labelValue;
				featherList.push_back(feather);
			}
		}
	}
	return labelValue;
}

Vec2d linearParameters(Vec4i line){
	Mat a = (Mat_<double>(2, 2) <<
		line[0], 1,
		line[2], 1);
	Mat y = (Mat_<double>(2, 1) <<
		line[1],
		line[3]);
	Vec2d mc; solve(a, y, mc);
	return mc;
}

Vec4i extendedLine(Vec4i line, double d){
	// oriented left-t-right
	Vec4d _line = line[2] - line[0] < 0 ? Vec4d(line[2], line[3], line[0], line[1]) : Vec4d(line[0], line[1], line[2], line[3]);
	double m = linearParameters(_line)[0];
	// solution of pythagorean theorem and m = yd/xd
	double xd = sqrt(d * d / (m * m + 1));
	double yd = xd * m;
	return Vec4d(_line[0] - xd, _line[1] - yd, _line[2] + xd, _line[3] + yd);
}

std::vector<Point2i> boundingRectangleContour(Vec4i line, float d){
	// finds coordinates of perpendicular lines with length d in both line points
	// https://math.stackexchange.com/a/2043065/183923

	Vec2f mc = linearParameters(line);
	float m = mc[0];
	float factor = sqrtf(
		(d * d) / (1 + (1 / (m * m)))
		);

	float x3, y3, x4, y4, x5, y5, x6, y6;
	// special case(vertical perpendicular line) when -1/m -> -infinity
	if (m == 0){
		x3 = line[0]; y3 = line[1] + d;
		x4 = line[0]; y4 = line[1] - d;
		x5 = line[2]; y5 = line[3] + d;
		x6 = line[2]; y6 = line[3] - d;
	}
	else {
		// slope of perpendicular lines
		float m_per = -1 / m;

		// y1 = m_per * x1 + c_per
		float c_per1 = line[1] - m_per * line[0];
		float c_per2 = line[3] - m_per * line[2];

		// coordinates of perpendicular lines
		x3 = line[0] + factor; y3 = m_per * x3 + c_per1;
		x4 = line[0] - factor; y4 = m_per * x4 + c_per1;
		x5 = line[2] + factor; y5 = m_per * x5 + c_per2;
		x6 = line[2] - factor; y6 = m_per * x6 + c_per2;
	}

	return std::vector<Point2i> {
		Point2i(x3, y3),
			Point2i(x4, y4),
			Point2i(x6, y6),
			Point2i(x5, y5)
	};
}

bool extendedBoundingRectangleLineEquivalence(const Vec4i& _l1, const Vec4i& _l2, float extensionLengthFraction, float maxAngleDiff, float boundingRectangleThickness){

	Vec4i l1(_l1), l2(_l2);
	// extend lines by percentage of line width
	float len1 = sqrtf((l1[2] - l1[0])*(l1[2] - l1[0]) + (l1[3] - l1[1])*(l1[3] - l1[1]));
	float len2 = sqrtf((l2[2] - l2[0])*(l2[2] - l2[0]) + (l2[3] - l2[1])*(l2[3] - l2[1]));
	Vec4i el1 = extendedLine(l1, len1 * extensionLengthFraction);
	Vec4i el2 = extendedLine(l2, len2 * extensionLengthFraction);

	// reject the lines that have wide difference in angles
	float a1 = atan(linearParameters(el1)[0]);
	float a2 = atan(linearParameters(el2)[0]);
	if (fabs(a1 - a2) > maxAngleDiff * M_PI / 180.0){
		return false;
	}

	// calculate window around extended line
	// at least one point needs to inside extended bounding rectangle of other line,
	std::vector<Point2i> lineBoundingContour = boundingRectangleContour(el1, boundingRectangleThickness / 2);
	return
		pointPolygonTest(lineBoundingContour, cv::Point(el2[0], el2[1]), false) == 1 ||
		pointPolygonTest(lineBoundingContour, cv::Point(el2[2], el2[3]), false) == 1;
}

int main()
{
	Mat frame;
	frame = imread("line3.png");


	Mat img, bgr, hsv;

	img = frame.clone();
	//resize(img, img, cv::Size(960, 480), INTER_NEAREST);
	blur(img, img, Size(3, 3));
	//彩色图像的灰度值归一化
	img.convertTo(bgr, CV_32FC3, 1.0 / 255, 0);
	//颜色空间转换
	cvtColor(bgr, hsv, COLOR_BGR2HSV);

	vector<Mat> mv;
	split(hsv, mv);//分为3个通道  
	Mat s = mv[1];

	threshold(s, s, 0.2, 255, THRESH_BINARY);
	s.convertTo(s, CV_8U, 1, 0);
	//获取自定义核
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	//dilate(s, s, element);
	morphologyEx(s, s, MORPH_CLOSE, element);

	vector<Feather> featherList;                    // 存放连通域特征
	Mat dst;
	cout << "连通域数量： " << bwLabel(s, dst, featherList) << endl;

	// 方便观察，可以将label“放大”
	for (int i = 0; i < dst.rows; i++)
	{
		uchar *p = dst.ptr<uchar>(i);
		for (int j = 0; j < dst.cols; j++)
		{
			p[j] = 30 * p[j];
		}
	}

	cout << "标号" << "\t" << "面积" << endl;
	for (vector<Feather>::iterator it = featherList.begin(); it < featherList.end(); it++)
	{
		cout << it->label << "\t" << it->area << endl;
		rectangle(dst, it->boundingbox, 255);
	}

	imshow("src", img);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();

	
	//Mat detectedLinesImg = Mat::zeros(img.rows, img.cols, CV_8UC3);
	//Mat reducedLinesImg = detectedLinesImg.clone();
	//Mat _drawing = img.clone();

	//cvtColor(_drawing, _drawing, CV_BGRA2GRAY);
	//Ptr<LineSegmentDetector> detector = createLineSegmentDetector(LSD_REFINE_NONE);
	//std::vector<Vec4i> lines; detector->detect(_drawing, lines);

	//// remove small lines
	//std::vector<Vec4i> linesWithoutSmall;
	//std::copy_if(lines.begin(), lines.end(), std::back_inserter(linesWithoutSmall), [](Vec4f line){
	//	float length = sqrtf((line[2] - line[0]) * (line[2] - line[0])
	//		+ (line[3] - line[1]) * (line[3] - line[1]));
	//	return length > 30;
	//});

	//std::cout << "Detected: " << linesWithoutSmall.size() << std::endl;


	//// partition via our partitioning function
	//std::vector<int> labels;
	//int equilavenceClassesCount = cv::partition(linesWithoutSmall, labels, [](const Vec4i l1, const Vec4i l2){
	//	return extendedBoundingRectangleLineEquivalence(
	//		l1, l2,
	//		// line extension length - as fraction of original line width
	//		0.2,
	//		// maximum allowed angle difference for lines to be considered in same equivalence class
	//		2.0,
	//		// thickness of bounding rectangle around each line		
	//		10);
	//});

	//std::cout << "Equivalence classes: " << equilavenceClassesCount << std::endl;

	//// grab a random colour for each equivalence class
	//RNG rng(215526);
	//std::vector<Scalar> colors(equilavenceClassesCount);
	//for (int i = 0; i < equilavenceClassesCount; i++){
	//	colors[i] = Scalar(rng.uniform(30, 255), rng.uniform(30, 255), rng.uniform(30, 255));;
	//}

	//// draw original detected lines
	//for (int i = 0; i < linesWithoutSmall.size(); i++){
	//	Vec4i& detectedLine = linesWithoutSmall[i];
	//	line(detectedLinesImg,
	//		cv::Point(detectedLine[0], detectedLine[1]),
	//		cv::Point(detectedLine[2], detectedLine[3]), colors[labels[i]], 2);
	//}

	//// build point clouds out of each equivalence classes
	//std::vector<std::vector<Point2i>> pointClouds(equilavenceClassesCount);
	//for (int i = 0; i < linesWithoutSmall.size(); i++){
	//	Vec4i& detectedLine = linesWithoutSmall[i];
	//	pointClouds[labels[i]].push_back(Point2i(detectedLine[0], detectedLine[1]));
	//	pointClouds[labels[i]].push_back(Point2i(detectedLine[2], detectedLine[3]));
	//}

	//// fit line to each equivalence class point cloud
	//std::vector<Vec4i> reducedLines = std::accumulate(pointClouds.begin(), pointClouds.end(), std::vector<Vec4i>{}, [](std::vector<Vec4i> target, const std::vector<Point2i>& _pointCloud){
	//	std::vector<Point2i> pointCloud = _pointCloud;

	//	//lineParams: [vx,vy, x0,y0]: (normalized vector, point on our contour)
	//	// (x,y) = (x0,y0) + t*(vx,vy), t -> (-inf; inf)
	//	Vec4f lineParams;
	//	fitLine(pointCloud, lineParams, CV_DIST_L2, 0, 0.01, 0.01);

	//	// derive the bounding xs of point cloud
	//	std::vector<Point2i>::iterator minXP, maxXP;
	//	std::tie(minXP, maxXP) = std::minmax_element(pointCloud.begin(), pointCloud.end(), [](const Point2i& p1, const Point2i& p2){ return p1.x < p2.x; });

	//	// derive y coords of fitted line
	//	float m = lineParams[1] / lineParams[0];
	//	int y1 = ((minXP->x - lineParams[2]) * m) + lineParams[3];
	//	int y2 = ((maxXP->x - lineParams[2]) * m) + lineParams[3];

	//	target.push_back(Vec4i(minXP->x, y1, maxXP->x, y2));
	//	return target;
	//});

	//for (Vec4i reduced : reducedLines){
	//	line(reducedLinesImg, Point(reduced[0], reduced[1]), Point(reduced[2], reduced[3]), Scalar(255, 255, 255), 2);
	//}

	system("pause");
	return 0;
}
