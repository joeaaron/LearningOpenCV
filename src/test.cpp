#include <iostream> 
#include <fstream> 
#include <iterator> 
#include <vector> 
#include <opencv2/opencv.hpp>

using namespace std;

/*----------------------------
* ���� : �� cv::Mat ����д�뵽 .txt �ļ�
*----------------------------
* ���� : WriteData
* ���� : public
* ���� : -1�����ļ�ʧ�ܣ�0��д�����ݳɹ���1������Ϊ��
*
* ���� : fileName [in] �ļ���
* ���� : matData [in] ��������
*/
int WriteData(string fileName, cv::Mat& matData)
{
	int retVal = 0;

	// ���ļ� 
	ofstream outFile(fileName.c_str(), ios_base::out); //���½��򸲸Ƿ�ʽд�� 
	if (!outFile.is_open())
	{
		cout << "���ļ�ʧ��" << endl;
		retVal = -1;
		return (retVal);
	}

	// �������Ƿ�Ϊ�� 
	if (matData.empty())
	{
		cout << "����Ϊ��" << endl;
		retVal = 1;
		return (retVal);
	}

	// д������ 
	for (int r = 0; r < matData.rows; r++)
	{
		for (int c = 0; c < matData.cols; c++)
		{
			uchar data = matData.at<uchar>(r, c); //��ȡ���ݣ�at<type> - type �Ǿ���Ԫ�صľ������ݸ�ʽ 
			outFile << data << "\t"; //ÿ�������� tab ���� 
		}
		outFile << endl; //���� 
	}

	return (retVal);
}

/*----------------------------
* ���� : �� .txt �ļ��ж������ݣ����浽 cv::Mat ����
* - Ĭ�ϰ� float ��ʽ�������ݣ�
* - ���û��ָ��������С��к�ͨ������������ľ����ǵ�ͨ����N �� 1 �е�
*----------------------------
* ���� : LoadData
* ���� : public
* ���� : -1�����ļ�ʧ�ܣ�0�����趨�ľ��������ȡ���ݳɹ���1����Ĭ�ϵľ��������ȡ����
*
* ���� : fileName [in] �ļ���
* ���� : matData [out] ��������
* ���� : matRows [in] ����������Ĭ��Ϊ 0
* ���� : matCols [in] ����������Ĭ��Ϊ 0
* ���� : matChns [in] ����ͨ������Ĭ��Ϊ 0
*/
int LoadData(string fileName, cv::Mat& matData, int matRows = 0, int matCols = 0, int matChns = 0)
{
	int retVal = 0;

	// ���ļ� 
	ifstream inFile(fileName.c_str(), ios_base::in);
	if (!inFile.is_open())
	{
		cout << "��ȡ�ļ�ʧ��" << endl;
		retVal = -1;
		return (retVal);
	}

	// �������� 
	istream_iterator<float> begin(inFile); //�� float ��ʽȡ�ļ�����������ʼָ�� 
	istream_iterator<float> end; //ȡ�ļ�������ֹλ�� 
	vector<float> inData(begin, end); //���ļ����ݱ����� std::vector �� 
	cv::Mat tmpMat = cv::Mat(inData); //�������� std::vector ת��Ϊ cv::Mat 

	// ����������д��� 
	//copy(vec.begin(),vec.end(),ostream_iterator<double>(cout,"\t")); 

	// ����趨�ľ���ߴ��ͨ���� 
	size_t dataLength = inData.size();
	//1.ͨ���� 
	if (matChns == 0)
	{
		matChns = 1;
	}
	//2.������ 
	if (matRows != 0 && matCols == 0)
	{
		matCols = dataLength / matChns / matRows;
	}
	else if (matCols != 0 && matRows == 0)
	{
		matRows = dataLength / matChns / matCols;
	}
	else if (matCols == 0 && matRows == 0)
	{
		matRows = dataLength / matChns;
		matCols = 1;
	}
	//3.�����ܳ��� 
	if (dataLength != (matRows * matCols * matChns))
	{
		cout << "��������ݳ��� ������ �趨�ľ���ߴ���ͨ����Ҫ�󣬽���Ĭ�Ϸ�ʽ�������" << endl;
		retVal = 1;
		matChns = 1;
		matRows = dataLength;
	}

	// ���ļ����ݱ������������ 
	matData = tmpMat.reshape(matChns, matRows).clone();

	return (retVal);
}

int main()
{
	cv::Mat matData;
	LoadData("depth.txt", matData, 480, 640, 1);
}