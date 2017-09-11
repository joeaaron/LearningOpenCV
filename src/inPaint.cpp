#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
//#include <photo/photo.hpp>

using namespace cv;

Point ptL, ptR; //鼠标画出矩形框的起点和终点
Mat imageSource, imageSourceCopy;
Mat ROI; //原图需要修复区域的ROI

//鼠标回调函数
void OnMouse(int event, int x, int y, int flag, void *ustg);

//鼠标圈定区域阈值处理+Mask膨胀处理
int main()
{
	imageSource = imread("lol.png");
	if (!imageSource.data)
	{
		return -1;
	}
	imshow("原图", imageSource);
	setMouseCallback("原图", OnMouse);
	waitKey();
}
void OnMouse(int event, int x, int y, int flag, void *ustg)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		ptL = Point(x, y);
		ptR = Point(x, y);
	}
	if (flag == CV_EVENT_FLAG_LBUTTON)
	{
		ptR = Point(x, y);
		imageSourceCopy = imageSource.clone();
		rectangle(imageSourceCopy, ptL, ptR, Scalar(255, 0, 0));
		imshow("原图", imageSourceCopy);
	}
	if (event == CV_EVENT_LBUTTONUP)
	{
		if (ptL != ptR)
		{
			ROI = imageSource(Rect(ptL, ptR));
			imshow("ROI", ROI);
			waitKey();
		}
	}
	//单击鼠标右键开始图像修复
	if (event == CV_EVENT_RBUTTONDOWN)
	{
		imageSourceCopy = ROI.clone();
		Mat imageGray;
		cvtColor(ROI, imageGray, CV_RGB2GRAY); //转换为灰度图
		Mat imageMask = Mat(ROI.size(), CV_8UC1, Scalar::all(0));

		//通过阈值处理生成Mask
		threshold(imageGray, imageMask, 235, 255, CV_THRESH_BINARY);
		Mat Kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
		dilate(imageMask, imageMask, Kernel);  //对Mask膨胀处理
		inpaint(ROI, imageMask, ROI, 9, INPAINT_TELEA);  //图像修复
		imshow("Mask", imageMask);
		imshow("修复后", imageSource);
	}
}