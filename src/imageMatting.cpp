
#include <string>  
#include <iostream>  
#include <vector>  
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>


using namespace std;
using namespace cv;

Point p0 = Point(-1, -1);
bool select_flag = false;
Mat img, showImg;
Mat imgmask;

void onMouse(int event, int x, int y, int flags, void*param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		p0.x = x;
		p0.y = y;
		select_flag = true;
	}
	else if (select_flag &&event == CV_EVENT_MOUSEMOVE)
	{
		Point pt = Point(x, y);
		line(showImg, p0, pt, Scalar(0, 255, 0), 2, 8, 0);
		line(imgmask, p0, pt, Scalar::all(0), 2, 8, 0);
		p0 = pt;
		imshow("showImg", showImg);
	}
	else if (select_flag && event == CV_EVENT_LBUTTONUP)
	{
		select_flag = false;
		p0 = Point(-1, -1);
		//img.copyTo(showImg);  
	}
	else if (event == CV_EVENT_RBUTTONUP)//�һ���ʾ�ٳ���ͼ  
	{
		Mat dst;//��ϣ����Ȧѡ�Ľ����ӣ���������ͷ  
		floodFill(imgmask, Point(x, y), Scalar(0));//point���ӵ����ڵ���ͨ�����  
		//imshow("floodFill",imgmask);  
		img.copyTo(dst, imgmask);//mask�����в�Ϊ��ĵ㱻dst��Ӧ��ֵ���  
		imshow("dst", dst);
		img.copyTo(showImg);
		imgmask.setTo(Scalar(255));//����ȡֵ  
	}

}
//��ͼ��������ʱ�ᱨ��  
int main()
{
	img = imread("ocr_1.jpg");
	img.copyTo(showImg);

	imgmask.create(img.size(), CV_8UC1);
	imgmask.setTo(Scalar(255));

	namedWindow("showImg");
	imshow("showImg", showImg);
	setMouseCallback("showImg", onMouse, 0);

	waitKey(0);
	//system("pause");  
	return 0;
}