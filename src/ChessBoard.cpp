#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

//���ؿ���̨����
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

int main(int argc ,char*argv[])
{
	int width = 140;//���̸���
	int height = 140;//���̸�߶�
	IplImage *src = cvCreateImage(cvSize(1120,1120),IPL_DEPTH_8U,1);
	cvZero(src);
	for (int i =0;i<src->width;i++)
	{
		for (int j=0;j<src->height;j++)
		{
			if ((i/width+j/height)%2==0)
			{
				src->imageData[i*src->widthStep+j*src->nChannels] =255;
			}
		}
	}
	
	cvNamedWindow("src");
	cvShowImage("src",src);

	cvSaveImage("ChessBoard.bmp",src,0);
	cvWaitKey(0);

	return 0;


}