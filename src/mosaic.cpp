///马赛克的实现原理是把图像上某个像素点一定范围邻域内的所有点用邻域内随机选取的一个像素点的颜色代替，这样可以模糊细节，但是可以保留大体的轮廓。

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

Mat imageSourceCopy; //原始图像  
Mat imageSource; //原始图像拷贝  
int neightbourHood = 9; //马赛克上每个方框的像素大小  

RNG rng;
int randomNum; //邻域内随机值  

Point ptL; //左键按下时坐标  
Point ptR;  //右键按下时坐标  

//鼠标回掉函数  
void onMouse(int event, int x, int y, int flag, void *ustg);

int main()
{
	imageSourceCopy = imread("lena.jpg");
	imageSource = imageSourceCopy.clone();
	//imshow("马赛克", imageSourceCopy);  
	namedWindow("马赛克");
	setMouseCallback("马赛克", onMouse);
	waitKey();
}

void onMouse(int event, int x, int y, int flag, void *ustg)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		ptL = Point(x, y);
	}
	if (event == CV_EVENT_LBUTTONUP)
	{
		//对鼠标画出的矩形框超出图像范围做处理，否则会越界崩溃  
		x > imageSource.cols - 2 * neightbourHood ? x = imageSource.cols - 2 * neightbourHood : x = x;
		y > imageSource.rows - 2 * neightbourHood ? y = imageSource.rows - 2 * neightbourHood : y = y;

		//对鼠标从右下往右上画矩形框的情况做处理  
		ptR = Point(x, y);
		Point pt = ptR;
		ptR.x < ptL.x ? ptR = ptL, ptL = pt : ptR = ptR;
		for (int i = 0; i < ptR.y - ptL.y; i += neightbourHood)
		{
			for (int j = 0; j < ptR.x - ptL.x; j += neightbourHood)
			{
				randomNum = rng.uniform(-neightbourHood / 2, neightbourHood / 2);
				Rect rect = Rect(j + neightbourHood + ptL.x, i + neightbourHood + ptL.y, neightbourHood, neightbourHood);
				Mat roi = imageSourceCopy(rect);
				Scalar sca = Scalar(
					imageSource.at<Vec3b>(i + randomNum + ptL.y, j + randomNum + ptL.x)[0],
					imageSource.at<Vec3b>(i + randomNum + ptL.y, j + randomNum + ptL.x)[1],
					imageSource.at<Vec3b>(i + randomNum + ptL.y, j + randomNum + ptL.x)[2]);
				Mat roiCopy = Mat(rect.size(), CV_8UC3, sca);
				roiCopy.copyTo(roi);
			}
		}
	}
	imshow("马赛克", imageSourceCopy);
	imwrite("mosaic.bmp", imageSourceCopy);
	waitKey();
}