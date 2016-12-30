#pragma once
#include <string>  
#include <cstdio>  
#include <cstdlib>  
#include <fstream>  
#include <iostream>  
#include <opencv2\opencv.hpp>  

using namespace  cv;
using namespace std;

class CSeqCapture
{
public:
	CSeqCapture(string path, string ext);
	bool operator >> (Mat& img);   //���� >> �����

private:
	ifstream ifs;

	//�����������ļ�
	void createBAT(string path, string ext);
	//�򿪴���������ͼƬ·���б�
	void openNameList();
};



