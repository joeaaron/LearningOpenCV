#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;
//���ֶ�ֵ��
void thresholdIntegral(Mat inputMat, Mat& outputMat)
{

	int nRows = inputMat.rows;
	int nCols = inputMat.cols;

	// create the integral image
	Mat sumMat;
	integral(inputMat, sumMat);

	int S = MAX(nRows, nCols) / 8;
	double T = 0.15;

	// perform thresholding
	int s2 = S / 2;
	int x1, y1, x2, y2, count, sum;

	int* p_y1, *p_y2;
	uchar* p_inputMat, *p_outputMat;

	for (int i = 0; i < nRows; ++i)
	{
		y1 = i - s2;
		y2 = i + s2;

		if (y1 < 0)
		{
			y1 = 0;
		}
		if (y2 >= nRows)
		{
			y2 = nRows - 1;
		}

		p_y1 = sumMat.ptr<int>(y1);
		p_y2 = sumMat.ptr<int>(y2);
		p_inputMat = inputMat.ptr<uchar>(i);
		p_outputMat = outputMat.ptr<uchar>(i);

		for (int j = 0; j < nCols; ++j)
		{
			// set the SxS region
			x1 = j - s2;
			x2 = j + s2;

			if (x1 < 0)
			{
				x1 = 0;
			}
			if (x2 >= nCols)
			{
				x2 = nCols - 1;
			}

			count = (x2 - x1)* (y2 - y1);

			// I(x,y)=s(x2,y2)-s(x1,y2)-s(x2,y1)+s(x1,x1)
			sum = p_y2[x2] - p_y1[x2] - p_y2[x1] + p_y1[x1];

			if ((int)(p_inputMat[j] * count) < (int)(sum* (1.0 - T)))
			{
				p_outputMat[j] = 0;
			}
			else
			{
				p_outputMat[j] = 255;
			}
		}
	}
}
//��ֱ����ͶӰ
void picshadowx(Mat binary)
{
	Mat paintx(binary.size(), CV_8UC1, Scalar(255)); //����һ��ȫ��ͼƬ��������ʾ

	int* blackcout = new int[binary.cols];
	memset(blackcout, 0, binary.cols * 4);

	for (int i = 0; i < binary.rows; i++)
	{
		for (int j = 0; j < binary.cols; j++)
		{
			if (binary.at<uchar>(i, j) == 0)
			{
				blackcout[j]++; //��ֱͶӰ������x�����ͶӰ
			}
		}
	}
	for (int i = 0; i < binary.cols; i++)
	{
		for (int j = 0; j < blackcout[i]; j++)
		{
			paintx.at<uchar>(binary.rows - 1 - j, i) = 0; //��ת�����棬���ڹۿ�
		}
	}
	delete blackcout;
	imshow("paintx", paintx);

}
//ˮƽ����ͶӰ���зָ�
void picshadowy(Mat binary)
{
	//�Ƿ�Ϊ��ɫ���ߺ�ɫ���ݶ�ֵͼ��Ĵ������
	Mat painty(binary.size(), CV_8UC1, Scalar(255)); //��ʼ��Ϊȫ��

	//ˮƽͶӰ
	int* pointcount = new int[binary.rows]; //�ڶ�ֵͼƬ�м�¼����������ĸ���
	memset(pointcount, 0, binary.rows * 4);//ע��������Ҫ���г�ʼ��

	for (int i = 0; i < binary.rows; i++)
	{
		for (int j = 0; j < binary.cols; j++)
		{
			if (binary.at<uchar>(i, j) == 0)
			{
				pointcount[i]++; //��¼ÿ���к�ɫ��ĸ��� //ˮƽͶӰ������y���ϵ�ͶӰ
			}
		}
	}

	for (int i = 0; i < binary.rows; i++)
	{
		for (int j = 0; j < pointcount[i]; j++) //����ÿ���к�ɫ��ĸ���������ѭ��
		{

			painty.at<uchar>(i, j) = 0;
		}

	}

	imshow("painty", painty);

	vector<Mat> result;
	int startindex = 0;
	int endindex = 0;
	bool inblock = false; //�Ƿ�������ַ�λ��

	for (int i = 0; i < painty.rows; i++)
	{

		if (!inblock&&pointcount[i] != 0) //�������ַ�����
		{
			inblock = true;
			startindex = i;
			cout << "startindex:" << startindex << endl;
		}
		if (inblock&&pointcount[i] == 0) //����հ���
		{
			endindex = i;
			inblock = false;
			Mat roi = binary.rowRange(startindex, endindex + 1); //�Ӷ���¼�ӿ�ʼ�������е�λ�ã����ɽ������з�
			result.push_back(roi);
		}
	}

	for (int i = 0; i < result.size(); i++)
	{
		Mat tmp = result[i];
		imshow("test" + to_string(i), tmp);
	}
	delete pointcount;

}
int main(int argc, char* argv[])
{

	Mat src = cv::imread("test.png");

	if (src.empty())
	{
		cerr << "Problem loading image!!!" << endl;
		return -1;
	}

	imshow("in", src);

	Mat gray;

	if (src.channels() == 3)
	{
		cv::cvtColor(src, gray, CV_BGR2GRAY);
	}
	else
	{
		gray = src;
	}


	Mat bw2 = Mat::zeros(gray.size(), CV_8UC1);
	thresholdIntegral(gray, bw2);

	cv::imshow("binary integral", bw2);

	//picshadowx (bw2);
	picshadowy(bw2);
	waitKey(0);

	return 0;
}