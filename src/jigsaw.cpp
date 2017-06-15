#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <time.h>
#include <iostream>
#include <vector>  

using namespace std;
using namespace cv;

Mat Sourceimage,Spilteimage,Rebuildimage,Dstimage;
int rows,cols;
int Roirows,Roicols;
vector<Mat>arraryimage;
void Randarrary( vector<Mat> &vectorMat);    //���������ͼ�����к���
static int vectornumber=0;
void OnMouseAction(int event,int x,int y,int flags,void *ustc);  //���ص��¼�����

int main(int argc,char*argv[])
{
	Sourceimage=imread("Source image.jpg");
	imshow("Source image",Sourceimage);
	rows = 4;      // atoi(argv[2]);
	cols = 4;      // atoi(argv[3]);
	Roirows=Sourceimage.rows/rows;
	Roicols=Sourceimage.cols/cols;

	Spilteimage=Mat::zeros(Sourceimage.rows,Sourceimage.cols,Sourceimage.type());
	Dstimage=Mat::zeros(Sourceimage.rows,Sourceimage.cols,Sourceimage.type());
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			Mat SourceRoi=Sourceimage(Rect(j*Roicols,i*Roirows,Roicols-1,Roirows-1));
			arraryimage.push_back(SourceRoi);

		}
	}

	Randarrary( arraryimage);

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			Mat SpilterRoi=Spilteimage(Rect(j*Roicols,i*Roirows,Roicols-1,Roirows-1));
			addWeighted(SpilterRoi,0,arraryimage[vectornumber],1,0,SpilterRoi);
			vectornumber++;
			imshow("Splite image",Spilteimage);
			waitKey(150);
		}
	}

	setMouseCallback("Splite image",OnMouseAction);

	waitKey();

}

//*******************************************************************//
//����������е���ͼ�����е�λ�ã������� Splite image����ʾ
//*******************************************************************//
void Randarrary( vector<Mat>& vectorMat)
{
	for(int i=0;i<vectorMat.size();i++)
	{
		srand(int(time(0)));
		int a=rand()%(vectorMat.size()-i)+i;
		swap(vectorMat[i],vectorMat[a]);
	}

}

//*******************************************************************//
//���ص����������ڻ�ȡ��Ҫ���ҵ���ͼ����ԭͼ���е�λ�ã���������ʾ��Ŀ��ͼ����
//*******************************************************************//
void OnMouseAction(int event,int x,int y,int flags,void *ustc)
{
	if(event==CV_EVENT_LBUTTONDOWN)
	{
		Mat RoiSpilte,RoiSource;
		int rows=(y/Roirows)*Roirows;
		int cols=(x/Roicols)*Roicols;

		RoiSpilte=Spilteimage(Rect(cols,rows,Roicols,Roirows));
		imshow("Slice",RoiSpilte);

		Mat image=Mat::zeros(Sourceimage.rows-Roirows,Sourceimage.cols-Roicols,CV_32FC1);
		matchTemplate(Sourceimage,RoiSpilte,image,1);
		normalize(image,image,0,1,NORM_MINMAX);

		double minV=0;
		double maxV=0;
		Point minP,maxP;

		minMaxLoc(image,&minV,&maxV,&minP,&maxP);

		//Mat ROIS=Sourceimage(Rect(maxP.x,maxP.y,Roicols,Roirows));
		Mat ROIDst=Dstimage(Rect(minP.x,minP.y,Roicols,Roirows));
		addWeighted(ROIDst,0,RoiSpilte,1,0,ROIDst,-1);
		imshow("Jigsaw image",Dstimage);
	}
}
