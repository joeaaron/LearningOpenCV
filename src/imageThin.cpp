/*!
* \file: thinning.cpp
* \brief: Zhang���п���ϸ���㷨
*
* \��ϸ����: canny��Ե����㷨��ȥ��Ǽ���ֵ���ơ�˫��ֵ
*                    ofstream���ڱ������ݵ�txt
*
* \author: Aaron | Email: pant333@163.com
* \version 1.0
* \date: 2016/9/20
*/

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

//Global variables
Mat src, src_gray, src_binary;
Mat dst;
const char *original_name = "Original";
const char *window_name = "ThinImage";
/*----------------------------
* ���� : ϸ���㷨
*----------------------------
* ���� : ThinImage
* ���� : protected
* ���� : ������ͼ��
*
* ���� : src       [in]    ͼ���ļ�
* ���� : maxIteration   [in]     ����������
*/
Mat ThinImage(Mat &src, const int maxIteration )
{
	cvtColor(src, src_gray, CV_BGR2GRAY);
	threshold(src_gray, src_binary, 128, 1, THRESH_BINARY);
	assert(src_binary.type() == CV_8UC1);
	int width = src_binary.cols;
	int height = src_binary.rows;
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
                        mFlag.push_back(p+j);  
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

int main()
{
	src = imread("./data/imgs/preThinImage.png");
    if (src.empty())
    {
		cout << "reading failed!" << endl;
    }

	dst = ThinImage(src, -1);

	dst = dst * 255;
	namedWindow(original_name, CV_WINDOW_AUTOSIZE);
	namedWindow(window_name, CV_WINDOW_AUTOSIZE);
	imshow(original_name, src);
	imshow(window_name, dst);

	imwrite("./Image/aftThinImage.png", dst);
	waitKey(0);
}