#ifdef _CH_
#pragma package <opencv>
#endif


#pragma   comment(lib,   "vfw32.lib ")
#pragma comment (lib , "comctl32.lib")

#ifndef _EiC
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <ctype.h>
#include "basicOCR.h"
#endif

IplImage* imagen;
int red,green,blue;
IplImage* screenBuffer;
int drawing;
int r,last_x, last_y;

void draw(int x,int y)
{
	//Draw a circle where is the mouse
	cvCircle(imagen, cvPoint(x,y), r, CV_RGB(red,green,blue), -1, 4, 0);
	//Get clean copy of image
	screenBuffer=cvCloneImage(imagen);
	cvShowImage( "手写板", screenBuffer );
}

void drawCursor(int x, int y)
{
	//Get clean copy of image
	screenBuffer=cvCloneImage(imagen);
	//Draw a circle where is the mouse
	cvCircle(screenBuffer, cvPoint(x,y), r, CV_RGB(0,0,0), 1, 4, 0);
}


/*************************
* Mouse CallBack 在opencv的highgui.h头文件中有以下宏定义
* event: 
*	#define CV_EVENT_MOUSEMOVE      0
*	#define CV_EVENT_LBUTTONDOWN    1
*	#define CV_EVENT_RBUTTONDOWN    2
*	#define CV_EVENT_MBUTTONDOWN    3
*	#define CV_EVENT_LBUTTONUP      4
*	#define CV_EVENT_RBUTTONUP      5
*	#define CV_EVENT_MBUTTONUP      6
*	#define CV_EVENT_LBUTTONDBLCLK  7
*	#define CV_EVENT_RBUTTONDBLCLK  8
*	#define CV_EVENT_MBUTTONDBLCLK  9
*
* x, y: mouse position
*
* flag:
*	#define CV_EVENT_FLAG_LBUTTON   1
*	#define CV_EVENT_FLAG_RBUTTON   2
*	#define CV_EVENT_FLAG_MBUTTON   4
*	#define CV_EVENT_FLAG_CTRLKEY   8
*	#define CV_EVENT_FLAG_SHIFTKEY  16
*	#define CV_EVENT_FLAG_ALTKEY    32
*
**************************/

void on_mouse( int event, int x, int y, int flags, void* param )
{
	last_x=x;
	last_y=y;
	drawCursor(x,y);
	//Select mouse Event
	if(event==CV_EVENT_LBUTTONDOWN)
	{
		drawing=1;
		draw(x,y);
	}
	else if(event==CV_EVENT_LBUTTONUP)
	{
		//drawing=!drawing;
		drawing=0;
	}
	else if(event == CV_EVENT_MOUSEMOVE  &&  flags & CV_EVENT_FLAG_LBUTTON)
	{
		if(drawing)
			draw(x,y);
	}
}



int main( int argc, char** argv )
{
	printf( "                                 数字字符识别\n"
		"快捷键: \n"
		"\tr - 重置白版\n"
		"\t+ - 笔迹增粗 ++\n"
		"\t- - 笔迹减细 --\n"
		"\ts - 保存输入为 out.pbm\n"	//输入可以作为样本再次部署进去
		"\tc - 输入分类识别, 结果在console显示\n"
		"\tESC - 退出程序\n");
	drawing=0;
	r=10;
	red=green=blue=0;
	last_x=last_y=red=green=blue=0;
	//Create image
	imagen=cvCreateImage(cvSize(128,128),IPL_DEPTH_8U,1); //白板图像保存为128*128
	//Set data of image to white
	cvSet(imagen, CV_RGB(255,255,255),NULL);
	//Image we show user with cursor and other artefacts we need
	screenBuffer=cvCloneImage(imagen);

	//Create window
	cvNamedWindow( "手写板", 0 );

	cvResizeWindow("手写板", 512,512);
	//Create mouse CallBack
	cvSetMouseCallback("手写板",&on_mouse, 0 );


	//////////////////
	//生成基础OCR类
	//////////////////
	basicOCR ocr;

	//Main Loop
	for(;;)
	{
		int c;

		cvShowImage( "手写板", screenBuffer );
		c = cvWaitKey(10);
		if( (char) c == 27 )
			break;
		if( (char) c== '+' )
		{
			r++;
			drawCursor(last_x,last_y);
		}
		if( ((char)c== '-') && (r>1) )
		{
			r--;
			drawCursor(last_x,last_y);
		}
		if( (char)c== 'r')
		{
			cvSet(imagen, cvRealScalar(255),NULL);
			drawCursor(last_x,last_y);
		}
		if( (char)c== 's')
		{
			cvSaveImage("out.pbm", imagen);
		}
		if( (char)c=='c')
		{
			ocr.classify(imagen,1);
		}

	}

	cvDestroyWindow("手写板");

	return 0;
}

#ifdef _EiC
main(1,"mouseEvent.c");
#endif
