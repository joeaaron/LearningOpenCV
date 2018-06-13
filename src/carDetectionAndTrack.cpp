// CarDetectAndTrack.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

//�������������������
bool biggerSort(vector<cv::Point> v1, vector<cv::Point> v2)
{
	return cv::contourArea(v1) > cv::contourArea(v2);
}

int main(int argc, char** argv[])
{
	//��Ƶ�����ڣ��ͷ���
	cv::VideoCapture cap("camera1.mov");
	if (cap.isOpened() == false)
		return 0;

	//�������
	int i;

	cv::Mat frame;			//��ǰ֡
	cv::Mat foreground;		//ǰ��
	cv::Mat bw;				//�м��ֵ����
	cv::Mat se;				//��̬ѧ�ṹԪ��

	//�û�ϸ�˹ģ��ѵ������ͼ��
	cv::BackgroundSubtractorMOG mog;
	for (i = 0; i < 100; ++i)
	{
		cout << "����ѵ������:" << i << endl;
		cap >> frame;
		if (frame.empty() == true)
		{
			cout << "��Ƶ̫֡�٣��޷�ѵ������" << endl;
			getchar();
			return 0;
		}
		mog(frame, foreground, 0.01);
	}

	//Ŀ����ӿ����ɽṹԪ�أ��������ӶϿ���СĿ�꣩
	cv::Rect rt;
	se = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

	//ͳ��Ŀ��ֱ��ͼʱʹ�õ��ı���
	vector<cv::Mat> vecImg;
	vector<int> vecChannel;
	vector<int> vecHistSize;
	vector<float> vecRange;
	cv::Mat mask(frame.rows, frame.cols, cv::DataType<uchar>::type);
	//������ʼ��
	vecChannel.push_back(0);
	vecHistSize.push_back(32);
	vecRange.push_back(0);
	vecRange.push_back(180);

	cv::Mat hsv;		//HSV��ɫ�ռ䣬��ɫ��H�ϸ���Ŀ�꣨camshift�ǻ�����ɫֱ��ͼ���㷨��
	cv::MatND hist;		//ֱ��ͼ����
	double maxVal;		//ֱ��ͼ���ֵ��Ϊ�˱���ͶӰͼ��ʾ����Ҫ��ֱ��ͼ��һ����[0 255]������
	cv::Mat backP;		//����ͶӰͼ
	cv::Mat result;		//���ٽ��

	//��Ƶ��������
	while (1)
	{
		//����Ƶ
		cap >> frame;
		if (frame.empty() == true)
			break;

		//���ɽ��ͼ
		frame.copyTo(result);

		//���Ŀ��
		mog(frame, foreground, 0.05);
		cv::imshow("��ϸ�˹���ǰ��", foreground);
		cv::moveWindow("��ϸ�˹���ǰ��", 400, 0);
		//��ǰ��������ֵ�˲�����̬ѧ���Ͳ�������ȥ��αĿ��ͽ����Ͽ���СĿ�꣨һ��������ʱ��Ͽ��ɼ���СĿ�꣩		
		cv::medianBlur(foreground, foreground, 5);
		cv::imshow("��ֵ�˲�", foreground);
		cv::moveWindow("��ֵ�˲�", 800, 0);
		cv::morphologyEx(foreground, foreground, cv::MORPH_DILATE, se);

		//����ǰ���и�����ͨ����������
		foreground.copyTo(bw);
		vector<vector<cv::Point>> contours;
		cv::findContours(bw, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		if (contours.size() < 1)
			continue;
		//����ͨ������������
		std::sort(contours.begin(), contours.end(), biggerSort);

		//���camshift���¸���λ�ã�����camshift�㷨�ڵ�һ�����£�����Ч���ǳ��ã�
		//�����ڼ����Ƶ�У����ڷֱ���̫�͡���Ƶ����̫�Ŀ��̫��Ŀ����ɫ��������
		//�ȸ������أ����¸���Ч���ǳ����ˣ���Ҫ�߸��١��߼�⣬������ٲ����ã�
		//���ü��λ���޸�
		cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
		vecImg.clear();
		vecImg.push_back(hsv);
		for (int k = 0; k < contours.size(); ++k)
		{
			//��k����ͨ��������Ӿ��ο�
			if (cv::contourArea(contours[k]) < cv::contourArea(contours[0]) / 5)
				break;
			rt = cv::boundingRect(contours[k]);
			mask = 0;
			mask(rt) = 255;

			//ͳ��ֱ��ͼ
			cv::calcHist(vecImg, vecChannel, mask, hist, vecHistSize, vecRange);
			cv::minMaxLoc(hist, 0, &maxVal);
			hist = hist * 255 / maxVal;
			//���㷴��ͶӰͼ
			cv::calcBackProject(vecImg, vecChannel, hist, backP, vecRange, 1);
			//camshift����λ��
			cv::Rect search = rt;
			cv::RotatedRect rrt = cv::CamShift(backP, search, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 1));
			cv::Rect rt2 = rrt.boundingRect();
			rt &= rt2;

			//���ٿ򻭵���Ƶ��
			cv::rectangle(result, rt, cv::Scalar(0, 255, 0), 2);
		}

		//�����ʾ
		cv::imshow("Origin", frame);
		cv::moveWindow("Origin", 0, 0);

		cv::imshow("��������", foreground);
		cv::moveWindow("��������", 0, 300);

		cv::imshow("����ͶӰ", backP);
		cv::moveWindow("����ͶӰ", 400, 300);

		cv::imshow("����Ч��", result);
		cv::moveWindow("����Ч��", 800, 300);
		cv::waitKey(30);
	}

	getchar();
	return 0;
}

