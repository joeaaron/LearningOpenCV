#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>

cv::Point2f center(0, 0);


bool sort_corners(std::vector<cv::Point2f>& corners)
{
	std::vector<cv::Point2f> top, bot;
	cv::Point2f tmp_pt;
	std::vector<cv::Point2f> olddata = corners;

	if (corners.size() != 4)
	{
		return false;
	}

	for (size_t i = 0; i < corners.size(); i++)
	{
		for (size_t j = i + 1; j < corners.size(); j++)
		{
			if (corners[i].y < corners[j].y)
			{
				tmp_pt = corners[i];
				corners[i] = corners[j];
				corners[j] = tmp_pt;
			}
		}
	}
	top.push_back(corners[0]);
	top.push_back(corners[1]);
	bot.push_back(corners[2]);
	bot.push_back(corners[3]);
	if (top.size() == 2 && bot.size() == 2) {
		corners.clear();
		cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
		cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
		cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
		cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];
		corners.push_back(tl);
		corners.push_back(tr);
		corners.push_back(br);
		corners.push_back(bl);
		return true;
	}
	else
	{
		corners = olddata;
		return false;
	}
}

cv::Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b)
{
	int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
	int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];

	if (float d = ((float)(x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4)))
	{
		cv::Point2f pt;
		pt.x = ((x1*y2 - y1*x2) * (x3 - x4) - (x1 - x2) * (x3*y4 - y3*x4)) / d;
		pt.y = ((x1*y2 - y1*x2) * (y3 - y4) - (y1 - y2) * (x3*y4 - y3*x4)) / d;
		return pt;
	}
	else
		return cv::Point2f(-1, -1);
}

bool IsBadLine(int a, int b)
{
	if (a * a + b * b < 100)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool x_sort(const cv::Point2f & m1, const cv::Point2f & m2)
{
	return m1.x < m2.x;
}

//确定四个点的中心线
void sortCorners(std::vector<cv::Point2f>& corners,
	cv::Point2f center)
{
	std::vector<cv::Point2f> top, bot;
	std::vector<cv::Point2f> backup = corners;

	sort(corners.begin(), corners.end(), x_sort);  //注意先按x的大小给4个点排序

	for (int i = 0; i < corners.size(); i++)
	{
		if (corners[i].y < center.y && top.size() < 2)    //这里的小于2是为了避免三个顶点都在top的情况
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}
	corners.clear();

	if (top.size() == 2 && bot.size() == 2)
	{
		std::cout << "log" << std::endl;
		cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
		cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
		cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
		cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

		corners.push_back(tl);
		corners.push_back(tr);
		corners.push_back(br);
		corners.push_back(bl);
	}
	else
	{
		corners = backup;
	}
}

int g_dst_hight;  //最终图像的高度
int g_dst_width; //最终图像的宽度

void CalcDstSize(const std::vector<cv::Point2f>& corners)
{
	int h1 = sqrt((corners[0].x - corners[3].x)*(corners[0].x - corners[3].x) + (corners[0].y - corners[3].y)*(corners[0].y - corners[3].y));
	int h2 = sqrt((corners[1].x - corners[2].x)*(corners[1].x - corners[2].x) + (corners[1].y - corners[2].y)*(corners[1].y - corners[2].y));
	g_dst_hight = MAX(h1, h2);

	int w1 = sqrt((corners[0].x - corners[1].x)*(corners[0].x - corners[1].x) + (corners[0].y - corners[1].y)*(corners[0].y - corners[1].y));
	int w2 = sqrt((corners[2].x - corners[3].x)*(corners[2].x - corners[3].x) + (corners[2].y - corners[3].y)*(corners[2].y - corners[3].y));
	g_dst_width = MAX(w1, w2);
}

int main()
{
	cv::Mat src = cv::imread("1.png");
	cv::Mat source = src.clone();
	cv::Mat bkp = src.clone();
	cv::Mat img = src.clone();

	cvtColor(img, img, CV_BGR2GRAY);
	GaussianBlur(img, img, cv::Size(5, 5), 0, 0);

	///获取自定义核
	cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));      //MORPH_RECT矩形卷积核
	dilate(img, img, element);
	Canny(img, img, 30, 120, 3);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<std::vector<cv::Point> > f_contours;
	std::vector<cv::Point> approx2;

	findContours(img, f_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	///求出面积最大的轮廓
	int maxArea = 0;
	int index;
	for (int i = 0; i < f_contours.size(); i++)
	{
		double temArea = fabs(contourArea(f_contours[i]));
		if (temArea > maxArea)
		{
			index = i;
			maxArea = temArea;
		}
	}
	contours.push_back(f_contours[index]);

	std::vector<cv::Point> tmp = contours[0];
	for (int lineType = 1; lineType <= 3; lineType++)
	{
		cv::Mat black = img.clone();
		black.setTo(0);
		drawContours(black, contours, 0, cv::Scalar(255), lineType);  //注意线的厚度，不要选择太细的
		imshow("show contour", black);

		std::vector<cv::Vec4i> lines;
		std::vector<cv::Point2f> corners;
		std::vector<cv::Point2f> approx;

		int para = 10;
		int flag = 0;
		int round = 0;
		for (; para < 300; para++)
		{
			lines.clear();
			corners.clear();
			approx.clear();
			center = cv::Point2f(0, 0);

			cv::HoughLinesP(black, lines, 1, CV_PI / 180, para, 30, 10);
		
			//过滤距离太近的直线
			std::set<int> ErasePt;
			for (int i = 0; i < lines.size(); i++)
			{
				for (int j = i + 1; j < lines.size(); j++)
				{
					if (IsBadLine(abs(lines[i][0] - lines[j][0]), abs(lines[i][1] - lines[j][1])) && (IsBadLine(abs(lines[i][2] - lines[j][2]), abs(lines[i][3] - lines[j][3]))))
					{
						ErasePt.insert(j);//将该坏线加入集合
					}
				}
			}

			int Num = lines.size();
			while (Num != 0)
			{
				std::set<int>::iterator j = ErasePt.find(Num);
				if (j != ErasePt.end())
				{
					lines.erase(lines.begin() + Num - 1);
				}
				Num--;
			}
			if (lines.size() != 4)
			{
				continue;
			}

			//计算直线的交点，保存在图像范围内的部分
			for (int i = 0; i < lines.size(); i++)
			{
				for (int j = i + 1; j < lines.size(); j++)
				{
					cv::Point2f pt = computeIntersect(lines[i], lines[j]);
					if (pt.x >= 0 && pt.y >= 0 && pt.x <= src.cols && pt.y <= src.rows)             //保证交点在图像的范围之内
						corners.push_back(pt);
				}
			}
			if (corners.size() != 4)
			{
				continue;
			}
#if 1
			bool IsGoodPoints = true;

			//保证点与点的距离足够大以排除错误点
			for (int i = 0; i < corners.size(); i++)
			{
				for (int j = i + 1; j < corners.size(); j++)
				{
					int distance = sqrt((corners[i].x - corners[j].x)*(corners[i].x - corners[j].x) + (corners[i].y - corners[j].y)*(corners[i].y - corners[j].y));
					if (distance < 5)
					{
						IsGoodPoints = false;
					}
				}
			}

			if (!IsGoodPoints) continue;
#endif
			cv::approxPolyDP(cv::Mat(corners), approx, cv::arcLength(cv::Mat(corners), true) * 0.02, true);

			if (lines.size() == 4 && corners.size() == 4 && approx.size() == 4)
			{
				flag = 1;
				break;
			}
		}

		// Get mass center  
		for (int i = 0; i < corners.size(); i++)
			center += corners[i];
		center *= (1. / corners.size());

		if (flag)
		{
			std::cout << "we found it!" << std::endl;
			cv::circle(bkp, corners[0], 3, CV_RGB(255, 0, 0), -1);
			cv::circle(bkp, corners[1], 3, CV_RGB(0, 255, 0), -1);
			cv::circle(bkp, corners[2], 3, CV_RGB(0, 0, 255), -1);
			cv::circle(bkp, corners[3], 3, CV_RGB(255, 255, 255), -1);
			cv::circle(bkp, center, 3, CV_RGB(255, 0, 255), -1);
			cv::imshow("backup", bkp);
			std::cout << "corners size" << corners.size() << std::endl;
			// cv::waitKey();

			// bool sort_flag = sort_corners(corners);
			// if (!sort_flag) cout << "fail to sort" << endl;

			sortCorners(corners, center);
			std::cout << "corners size" << corners.size() << std::endl;
			std::cout << "tl:" << corners[0] << std::endl;
			std::cout << "tr:" << corners[1] << std::endl;
			std::cout << "br:" << corners[2] << std::endl;
			std::cout << "bl:" << corners[3] << std::endl;

			CalcDstSize(corners);

			cv::Mat quad = cv::Mat::zeros(g_dst_hight, g_dst_width, CV_8UC3);
			std::vector<cv::Point2f> quad_pts;
			quad_pts.push_back(cv::Point2f(0, 0));
			quad_pts.push_back(cv::Point2f(quad.cols, 0));
			quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));

			quad_pts.push_back(cv::Point2f(0, quad.rows));

			cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);
			cv::warpPerspective(source, quad, transmtx, quad.size());

			cv::imshow("find", bkp);
			cv::imshow("quadrilateral", quad);

			/*如果需要二值化就解掉注释把*/
			/*
			Mat local,gray;
			cvtColor(quad, gray, CV_RGB2GRAY);
			int blockSize = 25;
			int constValue = 10;
			adaptiveThreshold(gray, local, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, constValue);

			imshow("二值化", local);

			*/


			cv::waitKey();
			return 0;
		}
	}

	std::cout << "can not transform!" << std::endl;
	cv::waitKey();
}