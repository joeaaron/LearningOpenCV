#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//求一幅灰度图像的直方图图像，返回的是直方图图像
Mat getHistograph(const Mat grayImage);

int main(int argc, char* argv[])
{
	//加载图像
	Mat image;
	image = imread("lena.jpg", IMREAD_COLOR);
	//判断是否为空
	if (image.empty())
	{
		cerr << "" << endl;
		return -1;
	}
	//定义灰度图像，转成灰度图
	Mat grayImage;
	cvtColor(image, grayImage, COLOR_BGR2GRAY);
	//double x=compareHist(hist,hist,/*CV_COMP_CORREL、CV_COMP_INTERSECT*/CV_COMP_BHATTACHARYYA);
	//直方图图像
	Mat hist = getHistograph(grayImage);

	imshow("src", image);
	imshow("hist", hist);
	waitKey(0);
	return 0;
}

Mat getHistograph(const Mat grayImage)
{
	//定义求直方图的通道数目，从0开始索引
	int channels[] = { 0 };
	const int histSize[] = { 256 };
	//每一维bin的变化范围
	float range[] = { 0, 256 };
	//所有bin的变化范围，个数跟channels应该跟channels一致
	const float* ranges[] = { range };

	Mat hist;
	//opencv中计算直方图的函数，hist大小为256*1，每行存储的统计的该行对应的灰度值的个数
	calcHist(&grayImage, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);

	//找出直方图统计的个数的最大值，用来作为直方图纵坐标的高
	double maxValue = 0;
	//找矩阵中最大最小值及对应索引的函数
	minMaxLoc(hist, 0, &maxValue, 0, 0);
	//最大值取整
	int rows = cvRound(maxValue);
	//定义直方图图像，直方图纵坐标的高作为行数，列数为256(灰度值的个数)
	//因为是直方图的图像，所以以黑白两色为区分，白色为直方图的图像
	Mat histImage = Mat::zeros(rows, 256, CV_8UC1);

	//直方图图像表示
	for (int i = 0; i < 256; i++)
	{
		//取每个bin的数目
		int temp = (int)(hist.at<float>(i, 0));
		//如果bin数目为0，则说明图像上没有该灰度值，则整列为黑色
		//如果图像上有该灰度值，则将该列对应个数的像素设为白色
		if (temp)
		{
			//由于图像坐标是以左上角为原点，所以要进行变换，使直方图图像以左下角为坐标原点
			histImage.col(i).rowRange(Range(rows - temp, rows)) = 255;
		}
	}
	//由于直方图图像列高可能很高，因此进行图像对列要进行对应的缩减，使直方图图像更直观
	Mat resizeImage;
	resize(histImage, resizeImage, Size(256, 256));
	return resizeImage;
}