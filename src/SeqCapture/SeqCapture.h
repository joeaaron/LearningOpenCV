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
	bool operator >> (Mat& img);   //重载 >> 运算符

private:
	ifstream ifs;

	//创建批处理文件
	void createBAT(string path, string ext);
	//打开创建出来的图片路径列表
	void openNameList();
};



