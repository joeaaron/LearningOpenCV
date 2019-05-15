#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>
#include <string>
#include <numeric>      // std::accumulate
#include <tuple>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

struct sortY {
	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.y < pt2.y); }
} mySortY;

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

Mat ImageThin(Mat &src, const int maxIteration, Mat& dst)
{
	Mat src_binary;
	threshold(src, src_binary, 128, 1, THRESH_BINARY);
	assert(src.type() == CV_8UC1);
	int width = src.cols;
	int height = src.rows;
	src_binary.copyTo(dst);
	int count = 0;
	while (true)
	{
		count++;
		if (maxIteration != -1 && count > maxIteration)                  //限制次数并且迭代次数达到
			break;
		vector<uchar *>mFlag;           //用于标记需要删除的点
		//对点标记  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记  
				//  p9 p2 p3  
				//  p8 p1 p4  
				//  p7 p6 p5  
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);
				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p6 == 0 && p4 * p6 * p8 == 0)
					{
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空  
		}

		//对点标记  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记  
				//  p9 p2 p3  
				//  p8 p1 p4  
				//  p7 p6 p5  
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0)
					{
						//标记  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空  
		}
		//对点标记  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记  
				//  p9 p2 p3  
				//  p8 p1 p4  
				//  p7 p6 p5  
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0)
					{
						//标记  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空  
		}

	}
	return dst;
}

void GetQuadCamTf(Mat img, vector<Point2f> crossPoints)
{
	
	//两条对角线交点的X坐标
	double cross_x = (crossPoints[0].x + crossPoints[1].x) / 2;
	double cross_y = (crossPoints[0].y + crossPoints[1].y) / 2;

	double center_x = crossPoints[0].x;
	double center_y = crossPoints[0].y;
	
	//quad coordinate
	Scalar lineColor = Scalar(0, 0, 255);
	DrawArrow(img, Point(cross_x, cross_y), Point(center_x, center_y), 25, 30, lineColor, 1, CV_AA);
	lineColor = Scalar(0, 255, 0);
	DrawArrow(img, Point(cross_x, cross_y), Point(cross_x, cross_y - 200), 25, 30, lineColor, 1, CV_AA);
	//L
	lineColor = Scalar(255, 0, 0);
	DrawArrow(img, Point(cross_x, cross_y), Point(img.cols / 2, img.rows / 2), 25, 30, lineColor, 1, CV_AA);
	double L = sqrt(pow(cross_x - img.cols / 2, 2) + pow(cross_y - img.rows / 2, 2));

	Point2f pt0(cross_x, cross_y);
	Point2f pt1(center_x, center_y);
	Point2f pt2(img.cols / 2, img.rows / 2);
	Point2f pt3(cross_x, cross_y - 200);
	

}

int main()
{
	Mat frame;
	Mat result, tmp;
	Mat Y, Cr, Cb;
	vector<Mat> channels;
	bool stop = false;

	frame = imread("line1.png");
	frame.copyTo(tmp);					 //backup
	/*转换颜色空间并分割颜色通道*/
	cvtColor(tmp, tmp, CV_BGR2YCrCb);
	split(tmp, channels);
	Y = channels.at(0);
	Cr = channels.at(1);
	Cb = channels.at(2);

	result.create(frame.rows, frame.cols, CV_8UC1);

	/*遍历图像，将符合阈值范围的像素设置为255，其余为0*/
	for (int j = 1; j < Y.rows - 1; j++)
	{
		uchar* currentCr = Cr.ptr< uchar>(j);
		uchar* currentCb = Cb.ptr< uchar>(j);
		uchar* current = result.ptr< uchar>(j);
		for (int i = 1; i < Y.cols - 1; i++)
		{
			if ((currentCr[i] > 135) && (currentCr[i] < 175) && (currentCb[i] > 60) && (currentCb[i] < 118))
				current[i] = 255;
			else
				current[i] = 0;
		}
	}

	Mat element = getStructuringElement(MORPH_RECT, Size(11, 11));
	morphologyEx(result, result, MORPH_CLOSE, element);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(result, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	Mat drawing = Mat::zeros(frame.size(), CV_8UC1);

	for (size_t i = 0; i < contours.size(); i++)
	{
		// Calculate the area of each contour
		double area = contourArea(contours[i]);
		// Ignore contours that are too small or too large
		if (area < 1e2 || 1e5 < area) continue;

		if (hierarchy[i][3] != -1)//表示其为某一个轮廓的内嵌轮廓
		{
			if (hierarchy[hierarchy[i][3]][3] == -1)//表示其为最外层轮廓，上面检测到的是其线条的内部
			{
				drawContours(drawing, contours, i, Scalar(0), -1);
			}
			else
			{
				drawContours(drawing, contours, i, Scalar(255), -1);
			}
		}
		else
		{
			drawContours(drawing, contours, i, Scalar(255), -1);
		}
	}

	//thin the contours
	Mat dst;
	//ThinImage(drawing, dst);
	ImageThin(drawing, -1, dst);
	dst = dst * 255;

	vector<vector<Point> > _contours;
	vector<Vec4i> _hierarchy;
	findContours(dst, _contours, _hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	vector<Point2f> points;
	Mat _drawing = Mat::zeros(frame.size(), CV_8UC1);

	for (size_t j = 0; j < _contours.size(); ++j)
	{
		// Calculate the area of each contour
		int size = _contours[j].size();
		double angle;
		// Ignore contours that are too small or too large
		if (size < 70 || 200 < size) continue;

		// Draw each contour only for visualisation purposes
		drawContours(_drawing, _contours, j, Scalar(255), 2, 8, hierarchy, 0);
	}

	Mat detectedLinesImg = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
	Mat reducedLinesImg = detectedLinesImg.clone();

	vector<Vec4i> lines;//储存着检测到的直线的参数对 (X1, Y1, X2, Y2) 的容器，也就是线段的两个端点
	HoughLinesP(_drawing, lines, 1, CV_PI / 180, 50, 50, 10);

	//Ptr<LineSegmentDetector> detector = createLineSegmentDetector(LSD_REFINE_NONE);
	//std::vector<Vec4i> lines; detector->detect(_drawing, lines);

	// remove small lines
	std::vector<Vec4i> linesWithoutSmall;
	std::copy_if(lines.begin(), lines.end(), std::back_inserter(linesWithoutSmall), [](Vec4f line){
		float length = sqrtf((line[2] - line[0]) * (line[2] - line[0])
			+ (line[3] - line[1]) * (line[3] - line[1]));
		return length > 30;
	});


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
		Point pos = Point((reduced[0] + reduced[2]) / 2, (reduced[1] + reduced[3]) / 2);
		circle(frame, pos, 3, CV_RGB(255, 0, 255), 2);
		points.push_back(pos);
	}


	sort(points.begin(), points.end(), mySortY);

	GetQuadCamTf(frame, points);

	imshow("frame", frame);
	imshow("result", result);


	if (waitKey(30) >= 0)
		stop = true;

	cv::waitKey();
	return 0;
}
