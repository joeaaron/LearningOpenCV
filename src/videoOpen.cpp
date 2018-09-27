#include <opencv2\opencv.hpp>  
using namespace cv;

//-----------------------------------��main( )������--------------------------------------------
//		����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼ
//-------------------------------------------------------------------------------------------------
int main()
{
	//��1��������Ƶ
	VideoCapture capture("LaneDetection.avi");
	int i = 0;
	// open the cam
	//VideoCapture capture(0);

	int count = 0;
	//��2��ѭ����ʾÿһ֡
	while (1)
	{
		Mat frame;//����һ��Mat���������ڴ洢ÿһ֡��ͼ��
		capture >> frame;  //��ȡ��ǰ֡
		
		if (!frame.empty()) //�жϵ�ǰ֡�Ƿ�׽�ɹ� **�ⲽ����Ҫ  
			imshow("��ȡ��Ƶ", frame);  //��ʾ��ǰ֡ 
		else
			std::cout << "can not ";
		if (waitKey(3) == 27)
			break;

		char file[100]; 

		if (waitKey(1) == 's')
		{
			sprintf(file, "x%d.bmp", count + 1);
			imwrite(file, frame);
		}

		count++;
			
		
	}
	return 0;
}
