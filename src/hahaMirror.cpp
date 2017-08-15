#include<iostream>  

#include "cv.h"  
#include "highgui.h"  
#include "math.h"  
#include "opencv2/core/core.hpp"  
   
#define DOWNRESIZE 0 // ��С  
#define UPRESIZE   1 // �Ŵ�  
#define HORAO      2 // ˮƽ�ⰼ  
#define HORTU      3 // ˮƽ��͹  
#define LADDER     4 // ���α���  
#define TRIANGLE   5 // �����α���  
#define SSHAPE     6 // S�α���  
#define WAVESHAPE  7 // �����α���  
#define Concentrated 8 //�����α���  
#define Scattered   9 // ɢ���α���  

#define RANGE     100 // ˮƽ�ⰼ����͹�ķ���  
#define PI        3.1415926  

using namespace std;
using namespace cv;

void MaxFrame(IplImage* frame)
{
	uchar* old_data = (uchar*)frame->imageData;
	uchar* new_data = new uchar[frame->widthStep * frame->height];

	int center_X = frame->width / 2;
	int center_Y = frame->height / 2;
	int radius = 400;
	int newX = 0;
	int newY = 0;

	int real_radius = (int)(radius / 2.0);
	for (int i = 0; i < frame->width; i++)
	{
		for (int j = 0; j < frame->height; j++)
		{
			int tX = i - center_X;
			int tY = j - center_Y;

			int distance = (int)(tX * tX + tY * tY);
			if (distance < radius * radius)
			{
				newX = (int)((float)(tX) / 2.0);
				newY = (int)((float)(tY) / 2.0);

				newX = (int)(newX * (sqrt((double)distance) / real_radius));
				newX = (int)(newX * (sqrt((double)distance) / real_radius));

				newX = newX + center_X;
				newY = newY + center_Y;

				new_data[frame->widthStep * j + i * 3] = old_data[frame->widthStep * newY + newX * 3];
				new_data[frame->widthStep * j + i * 3 + 1] = old_data[frame->widthStep * newY + newX * 3 + 1];
				new_data[frame->widthStep * j + i * 3 + 2] = old_data[frame->widthStep * newY + newX * 3 + 2];
			}
			else
			{
				new_data[frame->widthStep * j + i * 3] = old_data[frame->widthStep * j + i * 3];
				new_data[frame->widthStep * j + i * 3 + 1] = old_data[frame->widthStep * j + i * 3 + 1];
				new_data[frame->widthStep * j + i * 3 + 2] = old_data[frame->widthStep * j + i * 3 + 2];
			}
		}
	}
	memcpy(old_data, new_data, sizeof(uchar) * frame->widthStep * frame->height);
	delete[] new_data;
}


void MinFrame(IplImage* frame)
{
	uchar* old_data = (uchar*)frame->imageData;
	uchar* new_data = new uchar[frame->widthStep * frame->height];

	int center_X = frame->width / 2;
	int center_Y = frame->height / 2;

	int radius = 0;
	double theta = 0;
	int newX = 0;
	int newY = 0;

	for (int i = 0; i < frame->width; i++)
	{
		for (int j = 0; j < frame->height; j++)
		{
			int tX = i - center_X;
			int tY = j - center_Y;

			theta = atan2((double)tY, (double)tX);
			radius = (int)sqrt((double)(tX * tX) + (double)(tY * tY));
			int newR = (int)(sqrt((double)radius) * 12);
			newX = center_X + (int)(newR * cos(theta));
			newY = center_Y + (int)(newR * sin(theta));

			if (!(newX > 0 && newX < frame->width))
			{
				newX = 0;
			}
			if (!(newY > 0 && newY < frame->height))
			{
				newY = 0;
			}

			new_data[frame->widthStep * j + i * 3] = old_data[frame->widthStep * newY + newX * 3];
			new_data[frame->widthStep * j + i * 3 + 1] = old_data[frame->widthStep * newY + newX * 3 + 1];
			new_data[frame->widthStep * j + i * 3 + 2] = old_data[frame->widthStep * newY + newX * 3 + 2];
		}
	}
	memcpy(old_data, new_data, sizeof(uchar) * frame->widthStep * frame->height);
	delete[] new_data;
}

int main(int argc, char* argv[])
{
	IplImage* pImg;
	IplImage* pImg1;
	int i, j;
	int method = 0;
	CvSize size;
	double tmp;

	method = 8;

	//����ͼ��  
	pImg = cvLoadImage("lena.jpg", 1);
	cvNamedWindow("Image", 1);//��������  
	cvShowImage("Image", pImg);//��ʾͼ��  
	printf("imageSize: %d height: %d, width: %d, nChannels: %d\n", pImg->imageSize, pImg->height, pImg->width, pImg->nChannels);

	//MaxFrame(pImg);  
	//MinFrame(pImg);  

	switch (method)
	{
		// ͼ����С  
	case DOWNRESIZE:
		size = cvGetSize(pImg);
		size.width = (size.width >> 3) << 2; // ��OpenCV��ߣ�widthStep������4�ı������Ӷ�ʵ���ֽڶ��룬��������������ٶȡ�  
		size.height = size.height >> 1;
		pImg1 = cvCreateImage(size, IPL_DEPTH_8U, 1);
		printf("imageSize: %d height: %d, width: %d, nChannels: %d\n", pImg1->imageSize, pImg1->height, pImg1->width, pImg1->nChannels);
		for (i = 0; i < pImg1->height; i++)
			for (j = 0; j < pImg1->width; j++)
			{
				pImg1->imageData[i*pImg1->width + j] = pImg->imageData[i * 2 * pImg->width + j * 2];
			}
		break;
		// ͼ��Ŵ�  
	case UPRESIZE:
		/* ��Ӵ��� */
		break;
		// ˮƽ�ⰼ  
	case HORAO:
		pImg1 = cvCreateImage(cvGetSize(pImg), IPL_DEPTH_8U, 1);
		printf("imageSize: %d height: %d, width: %d, nChannels: %d\n", pImg1->imageSize, pImg1->height, pImg1->width, pImg1->nChannels);
		for (i = 0; i < pImg1->height; i++)
		{
			tmp = RANGE*sin(i*PI / pImg1->height);
			for (j = tmp; j < pImg1->width - tmp; j++)
			{
				pImg1->imageData[i*pImg1->width + j] = pImg->imageData[i*pImg->width + (int)((j - tmp)*(pImg->width) / (pImg->width - 2 * tmp))];
			}
		}
		break;
		// ˮƽ��͹  
	case HORTU:
		/* ��Ӵ��� */
		break;
		// ���α���  
	case LADDER:
		/* ��Ӵ��� */
		break;
		// �����α���  
	case TRIANGLE:
		/* ��Ӵ��� */
		break;
		// S�α���  
	case SSHAPE:
		/* ��Ӵ��� */
		break;
		// �����α���  
	case WAVESHAPE:
		/* ��Ӵ��� */
		break;
	case Concentrated:
		MaxFrame(pImg);
		break;
	case Scattered:
		MinFrame(pImg);
		break;
	default:
		printf("no method support\n");
		break;
	}

	// ��ʾ���  
	cvNamedWindow("Image1", 1);//��������  
	cvShowImage("Image1", pImg);//��ʾͼ��  

	cvWaitKey(0); //�ȴ�����  

	//���ٴ��� �ͷ��ڴ�  
	cvDestroyWindow("Image");//���ٴ���  
	cvReleaseImage(&pImg); //�ͷ�ͼ��  
	cvDestroyWindow("Image1");//���ٴ���  
	cvReleaseImage(&pImg1); //�ͷ�ͼ��  

	return 0;

}