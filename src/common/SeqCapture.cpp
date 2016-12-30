#include "SeqCapture.h"

CSeqCapture::CSeqCapture(string path, string ext)
{
	createBAT(path, ext);
	system("ListName.bat");  //执行BAT文件  
	openNameList();
}

bool CSeqCapture::operator >> (Mat& img)
{
	string path;

	if (ifs >> path)   //未读取完毕返回TRUE
	{
		img = imread(path);
		return true;
	}
	else         //读取完毕返回FALSE,并删除创建的临时文件ListName.bat和NameList.txt  
	{
		ifs.close();
		system("del ListName.bat");
		system("del NameList.txt");
		return false;
	}
}


void CSeqCapture::createBAT(string path, string ext)
{
	ofstream ofs("ListName.bat");
	if (ofs.fail())
	{
		cout << "创建BAT批处理文件失败！" << endl;
		exit(1);
	}
	ofs << "@echo off" << endl;
	ofs << "if exist NameList.txt del NameList.txt" << endl;
	ofs << "for %%i in (" << path << "\\*." << ext << ") do echo %%~fi>>NameList.txt" << endl;

	ofs.close();
}

void CSeqCapture::openNameList()
{
	ifs.open("NameList.txt");
	if (ifs.fail())
	{
		cout << "读取文件名列表名失败" << endl;
		exit(1);
	}
}