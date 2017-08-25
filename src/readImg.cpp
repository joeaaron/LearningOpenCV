#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <iostream>  
#include <stdio.h>  
#include <windows.h>  
#include<string.h>  
#include <io.h>  
#include <fstream>

using namespace std;

int num = 0;
//��ȡ���е��ļ���  
void GetAllFiles(string path, vector<string>& files)
{

	long   hFile = 0;
	//�ļ���Ϣ    
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}

		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}

}

// LPCWSTRתstring  
std::string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen <= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}

// ����winWIN32_FIND_DATA��ȡ�ļ��µ��ļ���  
void readImgNamefromFile(char* fileName, vector <string> &imgNames)
{
	// vector���� ��������  
	imgNames.clear();
	WIN32_FIND_DATA file;
	int i = 0;
	char tempFilePath[MAX_PATH + 1];
	char tempFileName[50];
	// ת�������ļ���  
	sprintf_s(tempFilePath, "%s/*", fileName);
	// ���ֽ�ת��  
	WCHAR   wstr[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, tempFilePath, -1, wstr, sizeof(wstr));
	// ���Ҹ��ļ��������ļ���������Զ�ȡ��WIN32_FIND_DATA  
	HANDLE handle = FindFirstFile(wstr, &file);
	if (handle != INVALID_HANDLE_VALUE)
	{
		FindNextFile(handle, &file);
		FindNextFile(handle, &file);
		// ѭ�������õ��ļ��е������ļ���    
		do
		{
			sprintf(tempFileName, "%s", fileName);
			imgNames.push_back(WChar2Ansi(file.cFileName));
			imgNames[i].insert(0, tempFileName);
			i++;
		} while (FindNextFile(handle, &file));
	}
	FindClose(handle);
}


//��ȡ�ض���ʽ���ļ���  
void GetAllFormatFiles(string path, vector<string>& files, string format)
{
	//�ļ����    
	long   hFile = 0;
	//�ļ���Ϣ    
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*" + format).c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
					GetAllFormatFiles(p.assign(path).append("\\").append(fileinfo.name), files, format);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

void saveImg(string filename, cv::Mat img, string preName)
{
	string temp;
	temp = std::to_string(++num);
	string addname = preName + temp + ".jpg";
	filename.append(addname);
	cv::imwrite(filename, img);
	if (15 == num)
		num = 0;
	
}

int main()
{
	// ���ö���ͼ�������ļ��е�·��  
	string filePath = "C:\\Users\\Aaron\\Desktop\\11111111111\\3";
	std::vector <string>  imgNames;
	//��ȡ���и�ʽΪjpg���ļ�  
	string format = ".bmp";
	GetAllFormatFiles(filePath, imgNames, format);

	string name;
	// ��ȡ��Ӧ�ļ����������ļ���  
	//readImgNamefromFile(filePath, imgNames);
	// ������Ӧ�ļ����������ļ���  
	for (int i = 0; i < imgNames.size(); i++)
	{
		cv::Mat img = cv::imread(imgNames[i]);
		if (!img.data)
			return -1;
		/* �����ͼ�����㷨code*/
		string stdPosMiddle = "��";
		string stdPosFront = "ǰ";
		string stdPosBack = "��";
		string stdPosLeft = "��";
		string stdPosRight = "��";

		
		//std::cout << imgNames[i];  
		if (imgNames[i].find(stdPosMiddle) != -1)
		{
			string filename = "C:\\Users\\Aaron\\Desktop\\11111111111\\3\\m\\";
			name = "middle";
			saveImg(filename, img, name);
			continue;
		}
	
		else if (imgNames[i].find(stdPosLeft) != -1)
		{
			string filename = "C:\\Users\\Aaron\\Desktop\\11111111111\\3\\l\\";
			name = "left";
			saveImg(filename, img, name);
			continue;
		}
		
		else if (imgNames[i].find(stdPosRight) != -1)
		{
			string filename = "C:\\Users\\Aaron\\Desktop\\11111111111\\3\\r\\";
			name = "right";
			saveImg(filename, img, name);
			continue;
		}
		

		else if (imgNames[i].find(stdPosFront) != -1)
		{
			
			string filename = "C:\\Users\\Aaron\\Desktop\\11111111111\\3\\f\\";
			name = "front";
			saveImg(filename, img, name);
			continue;
		}
	    
		else
		{
			string filename = "C:\\Users\\Aaron\\Desktop\\11111111111\\3\\b\\";
			name = "back";
			saveImg(filename, img, name);
			continue;
		}
		//cv::imshow("im", img);
		//cv::waitKey(0);��

	}
	return 0;
}