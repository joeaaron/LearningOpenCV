#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <iostream>  
#include <stdio.h>  
#include <windows.h>  
#include <string.h>  
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
	char buf[1000];
	GetCurrentDirectory(1000, buf);
	std::string path = buf;
	// ���ö���ͼ�������ļ��е�·��  
	string filePath = path + "\\imgs\\";
	std::cout << filePath << std::endl;
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
			string filename = filePath + "\\m\\";
			name = "middle";
			saveImg(filename, img, name);
			continue;
		}
	
		else if (imgNames[i].find(stdPosLeft) != -1)
		{
			string filename = filePath + "\\l\\";
			name = "left";
			saveImg(filename, img, name);
			continue;
		}
		
		else if (imgNames[i].find(stdPosRight) != -1)
		{
			string filename = filePath + "\\r\\";
			name = "right";
			saveImg(filename, img, name);
			continue;
		}
		

		else if (imgNames[i].find(stdPosFront) != -1)
		{
			
			string filename = filePath + "\\f\\";
			name = "front";
			saveImg(filename, img, name);
			continue;
		}
	    
		else
		{
			string filename = filePath + "\\b\\";
			name = "back";
			saveImg(filename, img, name);
			continue;
		}
		//cv::imshow("im", img);
		//cv::waitKey(0);��

	}
	return 0;
}