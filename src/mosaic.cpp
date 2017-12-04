///�����˵�ʵ��ԭ���ǰ�ͼ����ĳ�����ص�һ����Χ�����ڵ����е������������ѡȡ��һ�����ص����ɫ���棬��������ģ��ϸ�ڣ����ǿ��Ա��������������

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

Mat imageSourceCopy; //ԭʼͼ��  
Mat imageSource; //ԭʼͼ�񿽱�  
int neightbourHood = 9; //��������ÿ����������ش�С  

RNG rng;
int randomNum; //���������ֵ  

Point ptL; //�������ʱ����  
Point ptR;  //�Ҽ�����ʱ����  

//���ص�����  
void onMouse(int event, int x, int y, int flag, void *ustg);

int main()
{
	imageSourceCopy = imread("lena.jpg");
	imageSource = imageSourceCopy.clone();
	//imshow("������", imageSourceCopy);  
	namedWindow("������");
	setMouseCallback("������", onMouse);
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
		//����껭���ľ��ο򳬳�ͼ��Χ�����������Խ�����  
		x > imageSource.cols - 2 * neightbourHood ? x = imageSource.cols - 2 * neightbourHood : x = x;
		y > imageSource.rows - 2 * neightbourHood ? y = imageSource.rows - 2 * neightbourHood : y = y;

		//���������������ϻ����ο�����������  
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
	imshow("������", imageSourceCopy);
	imwrite("mosaic.bmp", imageSourceCopy);
	waitKey();
}