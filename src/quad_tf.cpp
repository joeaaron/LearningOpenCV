/********************************************************************
created:	2018/09/04
created:	4:9:2018   12:52
filename: 	D:\soucecode\LearningOpenCV\src\quad_tf.cpp
file path:	D:\soucecode\LearningOpenCV\src
file base:	quad_tf
file ext:	cpp
author:		aaron

purpose:
*********************************************************************/
#define _USE_MATH_DEFINES
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <cmath>
#include <string>
#include <numeric>      // std::accumulate
#include <tuple>

using namespace cv;
using namespace std;

#define debug 0

struct sortY {
	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.y < pt2.y); }
} mySortY;

struct sortX {
	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.x < pt2.x); }
} mySortX;

struct str{
	bool operator() (Point2f a, Point2f b){
		if (a.y != b.y)
			return a.y < b.y;
		return a.x <= b.x;
	}
} comp;


bool x_sort(const cv::Point2f & m1, const cv::Point2f & m2)
{
	return m1.x < m2.x;
}
// Returns the z-component of the cross product of a and b
inline double CrossProductZ(const Point2f &a, const Point2f &b) {
	return a.x * b.y - a.y * b.x;
}

// Orientation is positive if abc is counterclockwise, negative if clockwise.
// (It is actually twice the area of triangle abc, calculated using the
// Shoelace formula: http://en.wikipedia.org/wiki/Shoelace_formula .)
inline double Orientation(const Point2f &a, const Point2f &b, const Point2f &c) {
	return CrossProductZ(a, b) + CrossProductZ(b, c) + CrossProductZ(c, a);
}

void Sort4PointsClockwise(vector<Point2f> points)
{
	Point2f& a = points[0];
	Point2f& b = points[1];
	Point2f& c = points[2];
	Point2f& d = points[3];

	if (Orientation(a, b, c) < 0.0) {
		// Triangle abc is already clockwise.  Where does d fit?
		if (Orientation(a, c, d) < 0.0) {
			return;           // Cool!
		}
		else if (Orientation(a, b, d) < 0.0) {
			std::swap(d, c);
		}
		else {
			std::swap(a, d);
		}
	}
	else if (Orientation(a, c, d) < 0.0) {
		// Triangle abc is counterclockwise, i.e. acb is clockwise.
		// Also, acd is clockwise.
		if (Orientation(a, b, d) < 0.0) {
			std::swap(b, c);
		}
		else {
			std::swap(a, b);
		}
	}
	else {
		// Triangle abc is counterclockwise, and acd is counterclockwise.
		// Therefore, abcd is counterclockwise.
		std::swap(a, c);
	}
}
//************************************
// Method:    CalcDstSize
// FullName:  CalcDstSize
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const std::vector<cv::Point2f> & corners
// Parameter: int & h1
// Parameter: int & h2
// Parameter: int & w1
// Parameter: int & w2
//************************************
void CalcDstSize(const std::vector<cv::Point2f>& corners,int& h1, int& h2, int& w1, int& w2)
{
	h1 = sqrt((corners[0].x - corners[3].x)*(corners[0].x - corners[3].x) + (corners[0].y - corners[3].y)*(corners[0].y - corners[3].y));
	h2 = sqrt((corners[1].x - corners[2].x)*(corners[1].x - corners[2].x) + (corners[1].y - corners[2].y)*(corners[1].y - corners[2].y));

	w1 = sqrt((corners[0].x - corners[1].x)*(corners[0].x - corners[1].x) + (corners[0].y - corners[1].y)*(corners[0].y - corners[1].y));
	w2 = sqrt((corners[2].x - corners[3].x)*(corners[2].x - corners[3].x) + (corners[2].y - corners[3].y)*(corners[2].y - corners[3].y));
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

/*函数功能：求两条直线交点*/
/*输入：两条Vec4i类型直线*/
/*返回：Point2f类型的点*/
//https://www.jianshu.com/p/efa68c43d0ca
Point2f getCrossPoint(Vec4i LineA, Vec4i LineB)
{
	double ka, kb;
	ka = (double)(LineA[3] - LineA[1]) / (double)(LineA[2] - LineA[0]); //求出LineA斜率
	kb = (double)(LineB[3] - LineB[1]) / (double)(LineB[2] - LineB[0]); //求出LineB斜率

	Point2f crossPoint;
	crossPoint.x = (ka*LineA[0] - LineA[1] - kb*LineB[0] + LineB[1]) / (ka - kb);
	crossPoint.y = (ka*kb*(LineA[0] - LineB[0]) + ka*LineB[1] - kb*LineA[1]) / (ka - kb);
	return crossPoint;
}

void DrawArrow(cv::Mat& img, cv::Point pStart, cv::Point pEnd, int len, int alpha,
	cv::Scalar& color, int thickness, int lineType)
{
	const double PI = 3.1415926;
	Point arrow;
	//计算 θ 角（最简单的一种情况在下面图示中已经展示，关键在于 atan2 函数，详情见下面）
	double angle = atan2((double)(pStart.y - pEnd.y), (double)(pStart.x - pEnd.x));
	line(img, pStart, pEnd, color, thickness, lineType);
	//计算箭角边的另一端的端点位置（上面的还是下面的要看箭头的指向，也就是pStart和pEnd的位置）
	arrow.x = pEnd.x + len * cos(angle + PI * alpha / 180);
	arrow.y = pEnd.y + len * sin(angle + PI * alpha / 180);
	line(img, pEnd, arrow, color, thickness, lineType);
	arrow.x = pEnd.x + len * cos(angle - PI * alpha / 180);
	arrow.y = pEnd.y + len * sin(angle - PI * alpha / 180);
	line(img, pEnd, arrow, color, thickness, lineType);
}


void GetQuadCamTf(Mat img, vector<Point2f> crossPoints)
{
	//获得四个点的坐标
	vector<double> coordinates_x;
	vector<double> coordinates_y;
	for (int i = 0; i < crossPoints.size(); i++)
	{
		coordinates_x.push_back(crossPoints[i].x);
		coordinates_y.push_back(crossPoints[i].y);
	}

	//两条对角线的系数和偏移
	double k1 = (coordinates_y[3] - coordinates_y[0]) / (coordinates_x[3] - coordinates_x[0]);
	double b1 = (coordinates_x[3] * coordinates_y[0] - coordinates_x[0] * coordinates_y[3]) / (coordinates_x[3] - coordinates_x[0]);
	double k2 = (coordinates_y[2] - coordinates_y[1]) / (coordinates_x[2] - coordinates_x[1]);
	double b2 = (coordinates_x[2] * coordinates_y[1] - coordinates_x[1] * coordinates_y[2]) / (coordinates_x[2] - coordinates_x[1]);

	//两条对角线交点的X坐标
	double cross_x = -(b1 - b2) / (k1 - k2);
	double cross_y = (k1*b2 - k2 *b1) / (k1 - k2);

	double center_x = (coordinates_x[0] + coordinates_x[1]) / 2;
	double center_y = (coordinates_y[0] + coordinates_y[1]) / 2;
	if (cross_x >= 0 && cross_y >= 0 && cross_x <= img.cols && cross_y <= img.rows)     //保证交点在图像的范围之内
	{
		//quad coordinate
		Scalar lineColor = Scalar(0, 0, 255);
		DrawArrow(img, Point(cross_x, cross_y), Point(center_x, center_y), 25, 30, lineColor, 2, CV_AA);
		DrawArrow(img, Point(cross_x, cross_y), Point(cross_x, cross_y - 200), 25, 30, lineColor, 2, CV_AA);
		//L
		lineColor = Scalar(255, 0, 0);
		DrawArrow(img, Point(cross_x, cross_y), Point(img.cols / 2, img.rows / 2), 25, 30, lineColor, 2, CV_AA);
		double L = sqrt(pow(cross_x - img.cols / 2, 2) + pow(cross_y - img.rows / 2, 2));

		Point2f pt0(cross_x, cross_y);
		Point2f pt1(center_x, center_y);
		Point2f pt2(img.cols / 2, img.rows / 2);
		Point2f pt3(cross_x, cross_y - 200);
	}
		


}

int main(int /*argc*/, char** /*argv*/)
{

	VideoCapture cap("1.mp4");
	VideoWriter writer("VideoTest.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(960, 544));
	if (!cap.isOpened()) // check if we succeeded
		return -1;

	for (;;)
	{
		Mat frame;
#if debug
		frame = imread("test.png");
#else
		cap >> frame; 
#endif
		if (frame.empty())
		{
			continue;
		}
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

		threshold(s, s, 0.3, 255, THRESH_BINARY);
		s.convertTo(s, CV_8U, 1, 0);

		////获取自定义核
		//Mat element = getStructuringElement(MORPH_RECT, Size(11, 11));
		////dilate(s, s, element);
		//morphologyEx(s, s, MORPH_CLOSE, element);
		vector<vector<Point> > contours, contours2;
		vector<Vec4i> hierarchy;
		findContours(s, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));
		// Print contours' length轮廓的个数
		//cout << "Contours: " << contours.size() << endl;
#if debug
		vector<vector<Point>>::const_iterator itContours = contours.begin();
		for (; itContours != contours.end(); ++itContours) {

			cout << "Size: " << itContours->size() << endl;//每个轮廓包含的点数
		}
#endif
		// draw black contours on white image
		Mat drawing = Mat::zeros(img.size(), CV_8UC3);
		drawContours(drawing, contours,
			-1, // draw all contours
			Scalar(255), // in black
			2); // with a thickness of 2


		vector<vector<Point>> polyContours(contours.size());
		int maxArea = 0;
		for (int index = 0; index < contours.size(); index++){
			if (contourArea(contours[index]) > contourArea(contours[maxArea]))
				maxArea = index;
			approxPolyDP(contours[index], polyContours[index], 10, true);
		}

		Mat _drawing = Mat::zeros(img.size(), CV_8UC3);
		drawContours(_drawing, polyContours, maxArea, Scalar(0, 0, 255/*rand() & 255, rand() & 255, rand() & 255*/), 2);

		//2018-08-30
		///LSD line detection & find the cross points!
		Mat detectedLinesImg = Mat::zeros(img.rows, img.cols, CV_8UC3);
		Mat reducedLinesImg = detectedLinesImg.clone();

		cvtColor(_drawing, _drawing, CV_BGRA2GRAY);
		Ptr<LineSegmentDetector> detector = createLineSegmentDetector(LSD_REFINE_NONE);
		std::vector<Vec4i> lines; detector->detect(_drawing, lines);

		// remove small lines
		std::vector<Vec4i> linesWithoutSmall;
		std::copy_if(lines.begin(), lines.end(), std::back_inserter(linesWithoutSmall), [](Vec4f line){
			float length = sqrtf((line[2] - line[0]) * (line[2] - line[0])
				+ (line[3] - line[1]) * (line[3] - line[1]));
			return length > 30;
		});
#if debug
		std::cout << "Detected: " << linesWithoutSmall.size() << std::endl;
#endif
		// partition via our partitioning function
		std::vector<int> labels;
		int equilavenceClassesCount = cv::partition(linesWithoutSmall, labels, [](const Vec4i l1, const Vec4i l2){
			return extendedBoundingRectangleLineEquivalence(
				l1, l2,
				// line extension length - as fraction of original line width
				0.2,
				// maximum allowed angle difference for lines to be considered in same equivalence class
				2.0,
				// thickness of bounding rectangle around each line		
				10);
		});
#if debug
		std::cout << "Equivalence classes: " << equilavenceClassesCount << std::endl;

		// grab a random colour for each equivalence class
		RNG rng(215526);
		std::vector<Scalar> colors(equilavenceClassesCount);
		for (int i = 0; i < equilavenceClassesCount; i++){
			colors[i] = Scalar(rng.uniform(30, 255), rng.uniform(30, 255), rng.uniform(30, 255));;
		}

		// draw original detected lines
		for (int i = 0; i < linesWithoutSmall.size(); i++){
			Vec4i& detectedLine = linesWithoutSmall[i];
			line(detectedLinesImg,
				cv::Point(detectedLine[0], detectedLine[1]),
				cv::Point(detectedLine[2], detectedLine[3]), colors[labels[i]], 2);
		}
#endif
		// build point clouds out of each equivalence classes
		std::vector<std::vector<Point2i>> pointClouds(equilavenceClassesCount);
		for (int i = 0; i < linesWithoutSmall.size(); i++){
			Vec4i& detectedLine = linesWithoutSmall[i];
			pointClouds[labels[i]].push_back(Point2i(detectedLine[0], detectedLine[1]));
			pointClouds[labels[i]].push_back(Point2i(detectedLine[2], detectedLine[3]));
		}

		// fit line to each equivalence class point cloud
		std::vector<Vec4i> reducedLines = std::accumulate(pointClouds.begin(), pointClouds.end(), std::vector<Vec4i>{}, [](std::vector<Vec4i> target, const std::vector<Point2i>& _pointCloud){
			std::vector<Point2i> pointCloud = _pointCloud;

			//lineParams: [vx,vy, x0,y0]: (normalized vector, point on our contour)
			// (x,y) = (x0,y0) + t*(vx,vy), t -> (-inf; inf)
			Vec4f lineParams;
			fitLine(pointCloud, lineParams, CV_DIST_L2, 0, 0.01, 0.01);

			// derive the bounding xs of point cloud
			std::vector<Point2i>::iterator minXP, maxXP;
			std::tie(minXP, maxXP) = std::minmax_element(pointCloud.begin(), pointCloud.end(), [](const Point2i& p1, const Point2i& p2){ return p1.x < p2.x; });

			// derive y coords of fitted line
			float m = lineParams[1] / lineParams[0];
			int y1 = ((minXP->x - lineParams[2]) * m) + lineParams[3];
			int y2 = ((maxXP->x - lineParams[2]) * m) + lineParams[3];

			target.push_back(Vec4i(minXP->x, y1, maxXP->x, y2));
			return target;
		});

		for (Vec4i reduced : reducedLines){
			line(reducedLinesImg, Point(reduced[0], reduced[1]), Point(reduced[2], reduced[3]), Scalar(255, 255, 255), 2);
		}

		if (reducedLines.size() != 4)
			continue;
		else
		{
			vector<Point2f> crossPoint;
			//cvtColor(reducedLinesImg, reducedLinesImg, CV_BGRA2GRAY);
			//goodFeaturesToTrack(reducedLinesImg, crossPoint, 4, 0.3, 10, Mat(), 3);

			////遍历每个点，进行绘制，便于显示
			//Mat dstImage;
			//frame.copyTo(dstImage);
			//for (int i = 0; i < (int)crossPoint.size(); i++)
			//{
			//	circle(reducedLinesImg, crossPoint[i], 3, Scalar(theRNG().uniform(0, 255), theRNG().uniform(0, 255), theRNG().uniform(0, 255))
			//		, 2, 8);
			//}

			//计算直线的交点，保存在图像范围内的部分
			for (int i = 0; i < reducedLines.size(); i++)
			{
				for (int j = i + 1; j < reducedLines.size(); j++)
				{
					cv::Point2f pt = getCrossPoint(reducedLines[i], reducedLines[j]);
					if (pt.x >= 0 && pt.y >= 0 && pt.x <= reducedLinesImg.cols && pt.y <= reducedLinesImg.rows)             //保证交点在图像的范围之内
						crossPoint.push_back(pt);
				}
			}
			for (int i = 0; i < crossPoint.size(); i++){
				circle(reducedLinesImg, crossPoint[i], 10, Scalar(rand() & 255, rand() & 255, rand() & 255), 3);
			}

			if (crossPoint.size() != 4)
			{
				continue;
			}

			bool IsGoodPoints = true;
			sort(crossPoint.begin(), crossPoint.end(), comp);         //topLeft, topRight, bottomLeft, bottomRight
			// Get mass center  
			Point2f center(0, 0);
			for (int i = 0; i < crossPoint.size(); i++)
				center += crossPoint[i];
			center *= (1. / crossPoint.size());

			std::vector<cv::Point2f> top, bot;
			for (int i = 0; i < crossPoint.size(); i++)
			{
				if (crossPoint[i].y < center.y && top.size() < 2)    //这里的小于2是为了避免三个顶点都在top的情况
					top.push_back(crossPoint[i]);
				else
					bot.push_back(crossPoint[i]);
			}
	
			if (top.size() == 2 && bot.size() == 2)
			{
				//保证点与点的距离足够大以排除错误点
				for (int i = 0; i < crossPoint.size(); i++)
				{
					for (int j = i + 1; j < crossPoint.size(); j++)
					{
						int distance = sqrt((crossPoint[i].x - crossPoint[j].x)*(crossPoint[i].x - crossPoint[j].x) + (crossPoint[i].y - crossPoint[j].y)*(crossPoint[i].y - crossPoint[j].y));
						if (distance < 50 || distance > 2000)
						{
							IsGoodPoints = false;
						}
					}
				}

				if (!IsGoodPoints) continue;

				else
				{
					for (int i = 0; i < reducedLines.size(); i++){
						circle(img, crossPoint[i], 10, Scalar(rand() & 255, rand() & 255, rand() & 255), 3);
					}
				
					GetQuadCamTf(img, crossPoint);
					imshow("result", img);
					if (waitKey(1) >= 0)
						break;
				}
			}
		}

		imshow("ori", frame);
		if (waitKey(1) >= 0)
			break;
		
	}

	
	return 0;
}
