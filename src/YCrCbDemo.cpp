#include <iostream>
#include <vector>
#include <stack>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace cv;

typedef struct _Feather
{
	int label;              // ��ͨ���labelֵ
	int area;               // ��ͨ������
	Rect boundingbox;       // ��ͨ�����Ӿ��ο�
} Feather;

struct sortY {
	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.y < pt2.y); }
} mySortY;

/*
Input:
src: �������ͨ��Ķ�ֵ��ͼ��
Output:
dst: ��Ǻ��ͼ��
featherList: ��ͨ���������嵥
return��
��ͨ��������
*/
int bwLabel(Mat & src, Mat & dst, vector<Feather> & featherList)
{
	int rows = src.rows;
	int cols = src.cols;

	int labelValue = 0;
	Point seed, neighbor;
	stack<Point> pointStack;    // ��ջ

	int area = 0;               // ���ڼ�����ͨ������
	int leftBoundary = 0;       // ��ͨ�����߽磬�������С���ε���߿򣬺�����ֵ����������
	int rightBoundary = 0;
	int topBoundary = 0;
	int bottomBoundary = 0;
	Rect box;                   // ��Ӿ��ο�
	Feather feather;

	featherList.clear();	   // �������

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
				labelValue++;           // labelValue���Ϊ254����СΪ1.
				seed = Point(j, i);     // Point�������꣬�����꣩
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

void drawAxis(Mat& img, Point p, Point q, Scalar colour, const float scale = 0.2)
{
	double angle;
	double hypotenuse;
	angle = atan2((double)p.y - q.y, (double)p.x - q.x); // angle in radians
	hypotenuse = sqrt((double)(p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));
	// double degrees = angle * 180 / CV_PI; // convert radians to degrees (0-180 range)
	// cout << "Degrees: " << abs(degrees - 180) << endl; // angle in 0-360 degrees range
	// Here we lengthen the arrow by a factor of scale
	q.x = (int)(p.x - scale * hypotenuse * cos(angle));
	q.y = (int)(p.y - scale * hypotenuse * sin(angle));
	line(img, p, q, colour, 1, CV_AA);
	// create the arrow hooks
	p.x = (int)(q.x + 9 * cos(angle + CV_PI / 4));
	p.y = (int)(q.y + 9 * sin(angle + CV_PI / 4));
	line(img, p, q, colour, 1, CV_AA);
	p.x = (int)(q.x + 9 * cos(angle - CV_PI / 4));
	p.y = (int)(q.y + 9 * sin(angle - CV_PI / 4));
	line(img, p, q, colour, 1, CV_AA);
}

Point2f getOrientation(vector<Point> &pts, Mat &img)
{
	//Construct a buffer used by the pca analysis
	Mat data_pts = Mat(pts.size(), 2, CV_64FC1);
	for (int i = 0; i < data_pts.rows; ++i)
	{
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
	}

	//Perform PCA analysis
	PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);

	//Store the position of the object
	Point pos = Point(pca_analysis.mean.at<double>(0, 0),
		pca_analysis.mean.at<double>(0, 1));

	//Store the eigenvalues and eigenvectors
	vector<Point2d> eigen_vecs(2);
	vector<double> eigen_val(2);
	for (int i = 0; i < 2; ++i)
	{
		eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
			pca_analysis.eigenvectors.at<double>(i, 1));

		eigen_val[i] = pca_analysis.eigenvalues.at<double>(i, 0);
	}

	// Draw the principal components
	circle(img, pos, 3, CV_RGB(255, 0, 255), 2);

	Point p1 = pos + 0.02 * Point(static_cast<int>(eigen_vecs[0].x * eigen_val[0]), static_cast<int>(eigen_vecs[0].y * eigen_val[0]));
	Point p2 = pos - 0.02 * Point(static_cast<int>(eigen_vecs[1].x * eigen_val[1]), static_cast<int>(eigen_vecs[1].y * eigen_val[1]));
	drawAxis(img, pos, p1, Scalar(0, 255, 0), 1);
	drawAxis(img, pos, p2, Scalar(255, 255, 0), 5);
	double angle = atan2(eigen_vecs[0].y, eigen_vecs[0].x); // orientation in radians
	return pos;

}

void DrawArrow(cv::Mat& img, cv::Point pStart, cv::Point pEnd, int len, int alpha,
	cv::Scalar& color, int thickness, int lineType)
{
	const double PI = 3.1415926;
	Point arrow;
	//���� �� �ǣ���򵥵�һ�����������ͼʾ���Ѿ�չʾ���ؼ����� atan2 ��������������棩
	double angle = atan2((double)(pStart.y - pEnd.y), (double)(pStart.x - pEnd.x));
	line(img, pStart, pEnd, color, thickness, lineType);
	//������Ǳߵ���һ�˵Ķ˵�λ�ã�����Ļ��������Ҫ����ͷ��ָ��Ҳ����pStart��pEnd��λ�ã�
	arrow.x = pEnd.x + len * cos(angle + PI * alpha / 180);
	arrow.y = pEnd.y + len * sin(angle + PI * alpha / 180);
	line(img, pEnd, arrow, color, thickness, lineType);
	arrow.x = pEnd.x + len * cos(angle - PI * alpha / 180);
	arrow.y = pEnd.y + len * sin(angle - PI * alpha / 180);
	line(img, pEnd, arrow, color, thickness, lineType);
}

void ThinImage(cv::Mat& src, cv::Mat& dst)
{
	//cvtColor(src, src, COLOR_BGR2GRAY);
	//threshold(src, src, 70, 255, THRESH_BINARY); //appropriate value for s

	if (src.type() != CV_8UC1)
	{
		printf("ֻ�ܴ����ֵ��Ҷ�ͼ��\n");
		return;
	}
	//��ԭ�ز���ʱ��copy src��dst
	if (dst.data != src.data)
	{
		src.copyTo(dst);
	}

	char erase, n[8];
	unsigned char bdr1, bdr2, bdr4, bdr5;
	short k, b;
	unsigned long i, j;


	int width, height;
	width = dst.cols;
	height = dst.rows;

	//�Ѳ�����0��ֵת��Ϊ1�����ں��洦��
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (dst.at<uchar>(i, j) != 0)
			{
				dst.at<uchar>(i, j) = 1;
			}
			//ͼ��߿�����ֵΪ0
			if (i == 0 || i == (height - 1) || j == 0 || j == (width - 1))
				dst.at<uchar>(i, j) = 0;
		}
	}

	erase = 1;
	width = width - 1;
	height = height - 1;
	uchar* img;
	int step = dst.step;
	while (erase)
	{

		img = dst.data;
		//��һ��ѭ����ȡ��ǰ��������������2��ʾ
		for (i = 1; i < height; i++)
		{
			img += step;
			for (j = 1; j < width; j++)
			{
				uchar* p = img + j;


				if (p[0] != 1)
					continue;

				n[0] = p[1];
				n[1] = p[-step + 1];
				n[2] = p[-step];
				n[3] = p[-step - 1];
				n[4] = p[-1];
				n[5] = p[step - 1];
				n[6] = p[step];
				n[7] = p[step + 1];

				//bdr1��2���Ʊ�ʾ��p0...p6p7���У�10000011,p0=1,p6=p7=1
				bdr1 = 0;
				for (k = 0; k < 8; k++)
				{
					if (n[k] >= 1)
						bdr1 |= 0x80 >> k;
				}
				//�ڲ���,p0, p2, p4, p6����Ϊ1, �Ǳ߽�㣬���Լ���ѭ��
				//0xaa 10101010
				//  0   1   0   
				//  1         1
				//   0   1    0

				if ((bdr1 & 0xaa) == 0xaa)
					continue;
				//�����ڲ��㣬���Ǳ߽�㣬���ڱ߽�㣬���Ǳ��Ϊ2��������
				p[0] = 2;

				b = 0;

				for (k = 0; k <= 7; k++)
				{
					b += bdr1&(0x80 >> k);
				}
				//�ڱ߽���У�����1�����Ƕ˵㣬����0�����ǹ����㣬��ʱ���3
				if (b <= 1)
					p[0] = 3;

				//������˵��p�����м�㣬�����ȥ���������
				// 0x70        0x7         0x88      0xc1        0x1c      0x22      0x82     0x1      0xa0     0x40     0x28    0x10       0xa      0x4
				// 0 0 0     0  1  1     1  0   0    0   0   0    1  1  0    0   0   1  0  0  1  0 0 0    0  0  0   0 0 0    1  0  0   0  0  0  1  0  1   0 1 0
				// 1    0     0      1     0       0    0        1    1      0    0        0  0      0  0    1    0      0   0    0    0      0   1      0  0     0    0    0
				// 1 1 0     0  0  0     0  0   1    0   1   1    0  0  0    1   0   1  0  0  1  0 0 0    1  0  1   0 1 0    1  0  0   0  0  0  0  0  0   0 0 0
				if ((bdr1 & 0x70) != 0 && (bdr1 & 0x7) != 0 && (bdr1 & 0x88) == 0)
					p[0] = 3;
				else if ((bdr1 && 0xc1) != 0 && (bdr1 & 0x1c) != 0 && (bdr1 & 0x22) == 0)
					p[0] = 3;
				else if ((bdr1 & 0x82) == 0 && (bdr1 & 0x1) != 0)
					p[0] = 3;
				else if ((bdr1 & 0xa0) == 0 && (bdr1 & 0x40) != 0)
					p[0] = 3;
				else if ((bdr1 & 0x28) == 0 && (bdr1 & 0x10) != 0)
					p[0] = 3;
				else if ((bdr1 & 0xa) == 0 && (bdr1 & 0x4) != 0)
					p[0] = 3;

			}
		}
		//printf("------------------------------\n");
		//PrintMat(dst);
		img = dst.data;
		for (i = 1; i < height; i++)
		{
			img += step;
			for (j = 1; j < width; j++)
			{
				uchar* p = img + j;

				if (p[0] == 0)
					continue;

				n[0] = p[1];
				n[1] = p[-step + 1];
				n[2] = p[-step];
				n[3] = p[-step - 1];
				n[4] = p[-1];
				n[5] = p[step - 1];
				n[6] = p[step];
				n[7] = p[step + 1];

				bdr1 = bdr2 = 0;

				//bdr1��2���Ʊ�ʾ�ĵ�ǰ��p��8������ͨ�����hdr2�ǵ�ǰ����Χ��������������
				for (k = 0; k <= 7; k++)
				{
					if (n[k] >= 1)
						bdr1 |= 0x80 >> k;
					if (n[k] >= 2)
						bdr2 |= 0x80 >> k;
				}

				//��ȣ�������Χȫ��ֵΪ2�����أ�����
				if (bdr1 == bdr2)
				{
					p[0] = 4;
					continue;
				}

				//p0��Ϊ2������
				if (p[0] != 2) continue;
				//=4���ǲ���ɾ����������
				//     0x80       0xa     0x40        0x1      0x30   0x6
				//   0 0 0      1  0 1    0  0  0    0  0  0   0 0 0   0 1 1
				//   0    0      0     0    0      0    0      1   1    0   0    0
				//   0 0 1      0  0 0    0  1  0    0  0  0   1 0 0   0 0 0

				if (
					(bdr2 & 0x80) != 0 && (bdr1 & 0xa) == 0 &&
					//    ((bdr1&0x40)!=0 &&(bdr1&0x1)!=0 ||     ((bdr1&0x40)!=0 ||(bdr1 & 0x1)!=0) &&(bdr1&0x30)!=0 &&(bdr1&0x6)!=0 )
					(((bdr1 & 0x40) != 0 || (bdr1 & 0x1) != 0) && (bdr1 & 0x30) != 0 && (bdr1 & 0x6) != 0)
					)
				{
					p[0] = 4;
				}
				//
				else if ((bdr2 & 0x20) != 0 && (bdr1 & 0x2) == 0 &&
					//((bdr1&0x10)!=0 && (bdr1&0x40)!=0 || ((bdr1&0x10)!=0 || (bdr1&0x40)!=0) &&    (bdr1&0xc)!=0 && (bdr1&0x81)!=0)
					(((bdr1 & 0x10) != 0 || (bdr1 & 0x40) != 0) && (bdr1 & 0xc) != 0 && (bdr1 & 0x81) != 0)
					)
				{
					p[0] = 4;
				}

				else if ((bdr2 & 0x8) != 0 && (bdr1 & 0x80) == 0 &&
					//((bdr1&0x4)!=0 && (bdr1&0x10)!=0 || ((bdr1&0x4)!=0 || (bdr1&0x10)!=0) &&(bdr1&0x3)!=0 && (bdr1&0x60)!=0)
					(((bdr1 & 0x4) != 0 || (bdr1 & 0x10) != 0) && (bdr1 & 0x3) != 0 && (bdr1 & 0x60) != 0)
					)
				{
					p[0] = 4;
				}

				else if ((bdr2 & 0x2) != 0 && (bdr1 & 0x20) == 0 &&
					//((bdr1&0x1)!=0 && (bdr1&0x4)!=0 ||((bdr1&0x1)!=0 || (bdr1&0x4)!=0) &&(bdr1&0xc0)!=0 && (bdr1&0x18)!=0)
					(((bdr1 & 0x1) != 0 || (bdr1 & 0x4) != 0) && (bdr1 & 0xc0) != 0 && (bdr1 & 0x18) != 0)
					)
				{
					p[0] = 4;
				}
			}
		}
		//printf("------------------------------\n");
		//PrintMat(dst);
		img = dst.data;
		for (i = 1; i < height; i++)
		{
			img += step;
			for (j = 1; j < width; j++)
			{
				uchar* p = img + j;

				if (p[0] != 2)
					continue;


				n[0] = p[1];
				n[1] = p[-step + 1];
				n[2] = p[-step];
				n[3] = p[-step - 1];
				n[4] = p[-1];
				n[5] = p[step - 1];
				n[6] = p[step];
				n[7] = p[step + 1];

				bdr4 = bdr5 = 0;
				for (k = 0; k <= 7; k++)
				{
					if (n[k] >= 4)
						bdr4 |= 0x80 >> k;
					if (n[k] >= 5)
						bdr5 |= 0x80 >> k;
				}
				//ֵΪ4��5������
				if ((bdr4 & 0x8) == 0)
				{
					p[0] = 5;
					continue;
				}
				if ((bdr4 & 0x20) == 0 && bdr5 == 0)
				{
					p[0] = 5;
					continue;
				}

			}
		}
		erase = 0;
		//printf("------------------------------\n");
		//PrintMat(dst);
		img = dst.data;
		for (i = 1; i < height; i++)
		{
			img += step;
			for (j = 1; j < width; j++)
			{
				uchar* p = img + j;
				if (p[0] == 2 || p[0] == 5)
				{
					erase = 1;
					p[0] = 0;
				}
			}
		}
		//printf("------------------------------\n");
		//PrintMat(dst);
		//printf("------------------------\n");
	}

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
		if (maxIteration != -1 && count > maxIteration)                  //���ƴ������ҵ��������ﵽ
			break;
		vector<uchar *>mFlag;           //���ڱ����Ҫɾ���ĵ�
		//�Ե���  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//��������ĸ����������б��  
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

		//����ǵĵ�ɾ��  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//ֱ��û�е����㣬�㷨����  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//��mFlag���  
		}

		//�Ե���  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//��������ĸ����������б��  
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
						//���  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//����ǵĵ�ɾ��  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//ֱ��û�е����㣬�㷨����  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//��mFlag���  
		}
		//�Ե���  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//��������ĸ����������б��  
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
						//���  
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//����ǵĵ�ɾ��  
		for (std::vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//ֱ��û�е����㣬�㷨����  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//��mFlag���  
		}

	}
	return dst;
}

void GetQuadCamTf(Mat img, vector<Point2f> crossPoints)
{
	//����ĸ��������
	//vector<double> coordinates_x;
	//vector<double> coordinates_y;
	//for (int i = 0; i < crossPoints.size(); i++)
	//{
	//	coordinates_x.push_back(crossPoints[i].x);
	//	coordinates_y.push_back(crossPoints[i].y);
	//}

	//�����Խ��ߵ�ϵ����ƫ��
	double k1 = (crossPoints[3].y - crossPoints[0].y) / (crossPoints[3].x - crossPoints[0].x);
	double b1 = (crossPoints[3].x * crossPoints[0].y - crossPoints[0].x * crossPoints[3].y) / (crossPoints[3].x - crossPoints[0].x);
	double k2 = (crossPoints[2].y - crossPoints[1].y) / (crossPoints[2].x - crossPoints[1].x);
	double b2 = (crossPoints[2].x * crossPoints[1].y - crossPoints[1].x * crossPoints[2].y) / (crossPoints[2].x - crossPoints[1].x);

	//�����Խ��߽����X����
	double cross_x = -(b1 - b2) / (k1 - k2);
	double cross_y = (k1*b2 - k2 *b1) / (k1 - k2);

	double center_x = (crossPoints[0].x + crossPoints[1].x) / 2;
	double center_y = (crossPoints[0].y + crossPoints[1].y) / 2;
	if (cross_x >= 0 && cross_y >= 0 && cross_x <= img.cols && cross_y <= img.rows)     //��֤������ͼ��ķ�Χ֮��
	{
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

}

int main()
{
	Mat frame;
	Mat result, tmp;
	Mat Y, Cr, Cb;
	vector<Mat> channels;
	bool stop = false;

	frame = imread("line3.png");
	frame.copyTo(tmp);					//��������
	/*ת����ɫ�ռ䲢�ָ���ɫͨ��*/
	cvtColor(tmp, tmp, CV_BGR2YCrCb);
	split(tmp, channels);
	Y = channels.at(0);
	Cr = channels.at(1);
	Cb = channels.at(2);

	result.create(frame.rows, frame.cols, CV_8UC1);

	/*����ͼ�񣬽�������ֵ��Χ����������Ϊ255������Ϊ0*/
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


	//cv::Canny(result, result, 10, 350)
	//vector<Vec4i> lines;//�����ż�⵽��ֱ�ߵĲ����� (X1, Y1, X2, Y2) ��������Ҳ�����߶ε������˵�
	//HoughLinesP(result, lines, 1, CV_PI / 180, 50, 50, 10);
	//for (size_t i = 0; i < lines.size(); i++)
	//{
	//	Vec4i l = lines[i];//Vec4i ����Vec<int, 4>�������ţ���int
	//	line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);    //Ҫ���������ڵ�ͼ��cdst������㣺Point(l[0], l[1])�����յ㣺Point(l[2], l[3])������ɫ��Scalar(0,0,255)
	//}
	// Find all the contours in the thresholded image
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

		if (hierarchy[i][3] != -1)//��ʾ��Ϊĳһ����������Ƕ����
		{
			if (hierarchy[hierarchy[i][3]][3] == -1)//��ʾ��Ϊ����������������⵽�������������ڲ�
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
	vector<double> angles;
	vector<Point2f> points;
	Mat _drawing = Mat::zeros(frame.size(), CV_8UC1);

	for (size_t j = 0; j < _contours.size(); ++j)
	{
		// Calculate the area of each contour
		int size = _contours[j].size();
		// Ignore contours that are too small or too large
		if (size < 70 || 200 < size) continue;

		// Draw each contour only for visualisation purposes
		drawContours(_drawing, _contours, j, Scalar(255), 2, 8, hierarchy, 0);
		// Find the orientation of each shape
		points.push_back(getOrientation(_contours[j], frame));
	}
	//vector<Point2f> crossPoint;
	//goodFeaturesToTrack(_drawing, crossPoint, 7, 0.3, 10, Mat(), 3);
	//for (int i = 0; i < (int)crossPoint.size(); i++)
	//{
	//	circle(frame, crossPoint[i], 3, Scalar(theRNG().uniform(0, 255), theRNG().uniform(0, 255), theRNG().uniform(0, 255))
	//		, 2, 8);
	//}
	sort(points.begin(), points.end(), mySortY);
	while (points.size() > 4)
	{
		points.erase(points.begin());
	}
	//vector<float> slope;
	//vector<int> length;
	////https://stackoverrun.com/cn/q/10330328
	//for (int i = 1; i < points.size(); i++)
	//{
	//	double k = ((points[i].y - points[i - 1].y) / (double)(points[i].x - points[i - 1].x));
	//	int l = cv::norm(points[i] - points[i - 1]);
	//	slope.push_back(k);
	//	length.push_back(l);
	//}
	if (points[0].x > points[1].x)
		swap(points[0], points[1]);
	if (points[2].x > points[3].x)
		swap(points[2], points[3]);

	GetQuadCamTf(frame, points);

	imshow("frame", frame);
	imshow("result", result);

	if (waitKey(30) >= 0)
		stop = true;

	cv::waitKey();
	return 0;
}
