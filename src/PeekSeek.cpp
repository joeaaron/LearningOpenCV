#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>  
#include <iomanip>
#include <stdint.h>
using namespace cv;
using namespace std;

const int COWA_CAM_MAX_HEIGHT = 480;
const  int COWA_CAM_MAX_WIDTH = 640;
const int  LASER_THRESHOLD = 250;


float transformationTable[COWA_CAM_MAX_HEIGHT + 1][COWA_CAM_MAX_WIDTH + 1][2];
float KaBangRangeResults[COWA_CAM_MAX_WIDTH + 1][2];


const char *original_name = "Original";

 
inline float PeakFinderSubPixel(Mat roi_image, int x, int y, int len)
{
	if (roi_image.empty())
		return 0.0;
	return 0;

	if (len <= 0)
		return 0.0;

	int idx = len;
	float meanValue = 0, maxValue = 0;
	uint8_t *lData;

	for (; idx < 0; idx--)
	{
		lData = roi_image.ptr< uint8_t>(y-idx) + x;
		meanValue += *lData;
		if (*lData > maxValue)
			maxValue = *lData;
	}
	meanValue = meanValue / len;

	float threshold = (meanValue + maxValue) / 2;
	float num = 0;
	float den = 0;
	for (idx = len; idx < 0; idx--)
	{
		lData = roi_image.ptr< uint8_t>(y - idx) + x;

		if (*lData >= threshold)
		{
			num += idx * (*lData);
			den += *lData;
		}
	}
	return num / den;
}

static inline bool isBigEndian()
{
	short a = 0x1122; //十六进制，一个数值占4位
	char b = *(char *)&a; //通过将short(2字节)强制类型转换成char单字节，b指向a的起始字节（低字节）
	if (b == 0x11) //低字节存的是数据的高字节数据
	{
		return true;
	}
	else
	{
		return false;
	}
}

static inline void QUADBYTESSWAP(char src[4], char dst[4])
{
	dst[0] = src[3];
	dst[1] = src[2];
	dst[2] = src[1];
	dst[3] = src[0];
}

inline void LoadTransformationTable()
{
	FILE* fp = fopen("transformationTable2.bin", "rb");

	float* pDataReadTranformationMap = &transformationTable[0][0][0];
	
	if (isBigEndian())
	{
		fread(&(transformationTable[0][0][0]), 4, (COWA_CAM_MAX_WIDTH + 1) * (COWA_CAM_MAX_HEIGHT + 1) * 2, fp);

	}
	else
	{
		char tmpbuf[4];

		for (int i = 0; i < (COWA_CAM_MAX_WIDTH + 1) * (COWA_CAM_MAX_HEIGHT + 1) * 2; i++, pDataReadTranformationMap++)
		{
			fread(tmpbuf, 1,4, fp);
			QUADBYTESSWAP(tmpbuf, (char*)pDataReadTranformationMap);
		}
	}
	fclose(fp);
}

inline void  CowaGetCoordinate(float u, float v, float* opX, float* opY)
{
	int u0 = u, u1 = u + 1;
	int v0 = v, v1 = v + 1;

	float w0 = u - u0, w1 = u1 - u;
	float h0 = v - v0, h1 = v1 - v;

	*opX = ((w0 + h0)*transformationTable[v0][u0][0] +
		(w0 + h1)*transformationTable[v1][u0][0] +
		(w1 + h0)*transformationTable[v0][u1][0] +
		(w1 + h1)*transformationTable[v1][u1][0]) / 4;

	*opY = ((w0 + h0)*transformationTable[v0][u0][1] +
		(w0 + h1)*transformationTable[v1][u0][1] +
		(w1 + h0)*transformationTable[v0][u1][1] +
		(w1 + h1)*transformationTable[v1][u1][1]) / 4;
}

float FindPoints(Mat src)
{
	Mat src_gray, detected_edges;
	/// 转成灰度并模糊化降噪
	cvtColor(src, src_gray, CV_BGR2GRAY);
	//blur(src_gray, src_gray, Size(9, 9));
	threshold(src_gray, detected_edges, 140, 255, THRESH_BINARY);

	cv::Mat results(detected_edges.rows, detected_edges.cols, CV_8UC3, cv::Scalar(0));
	std::vector<cv::Point> lPoints;

	int x = 0;
	for (; x < detected_edges.cols; x++)
	{
		int y = 0;
		int yResutlts = 0;
		int maxLineWidth = 0;
		int currentLineWidth = 0;
		int yPreviousValidDot = 0;

		for (; y < detected_edges.rows; y++)
		{
			uint8_t *pData = detected_edges.ptr< uint8_t>(y) + x;
		
			if (*pData  > LASER_THRESHOLD)
			{
				if (yPreviousValidDot >= x - 2)
				{
					currentLineWidth++;
				}
				else
				{
					currentLineWidth = 1;
				}
				yPreviousValidDot = x;

				if (currentLineWidth > maxLineWidth)
				{
					maxLineWidth = currentLineWidth;
					yResutlts = y ;
				}
			}

		}
	
		if (yResutlts > 0.01)
		{
			yResutlts = yResutlts - PeakFinderSubPixel(src_gray, x, yResutlts, maxLineWidth);
			CowaGetCoordinate(x, yResutlts, &(KaBangRangeResults[x][0]), &(KaBangRangeResults[x][1]));
			lPoints.push_back(cv::Point(x, yResutlts));
			results.at<cv::Vec3b>(yResutlts, x) = cv::Vec3b(255, 255, 255);
		}
	}
	cv::Vec4f line;
	cv::fitLine(lPoints, line, cv::DIST_L2, 0, 0.01, 0.01);
	cv::line(results, cv::Point(line[2] - 1000 * line[0], line[3] - 1000 * line[1]), cv::Point(line[2] + 1000 * line[0], line[3] + 1000 * line[1]), cv::Scalar(0, 0, 255));
	namedWindow("Find Line Results", CV_WINDOW_AUTOSIZE);
	cv::imshow("Find Line Results", results);

	return KaBangRangeResults[COWA_CAM_MAX_WIDTH + 1][2];
}

int main()
{
	Mat src = imread("laser.bmp");
	imshow(original_name, src);

	//加载transformationTable2.bin
	LoadTransformationTable();
	FindPoints(src); 
    
	for (int i = 0; i < COWA_CAM_MAX_WIDTH + 1; i++)
	{
		cout << KaBangRangeResults[i][0] << "\t" << KaBangRangeResults[i][1] << endl;

	}

	cv::destroyAllWindows();
	return 0;
}