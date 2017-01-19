#include "SeqCapture.h"

CSeqCapture::CSeqCapture(string path, string ext)
{
	createBAT(path, ext);
	system("ListName.bat");  //ִ��BAT�ļ�  
	openNameList();
}

bool CSeqCapture::operator >> (Mat& img)
{
	string path;

	if (ifs >> path)   //δ��ȡ��Ϸ���TRUE
	{
		img = imread(path);
		return true;
	}
	else         //��ȡ��Ϸ���FALSE,��ɾ����������ʱ�ļ�ListName.bat��NameList.txt  
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
		cout << "����BAT�������ļ�ʧ�ܣ�" << endl;
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
		cout << "��ȡ�ļ����б���ʧ��" << endl;
		exit(1);
	}
}