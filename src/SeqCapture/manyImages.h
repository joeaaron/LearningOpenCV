#include<opencv2\opencv.hpp>
#include<vector>
using namespace std;
using namespace cv;
/*ʹ������vector��ROI�������һ��������ʾ����ͼƬ
Images��vector<Mat>,����push_back���������������ͼƬ
dstΪ���ͼ�񣬼�������ͼƬ����һ��ͼƬ��
imgRows��ÿ����ʾ����ͼƬ������Ĭ����3
ʾ�����룺
Mat img_1 = imread("1.jpg");
Mat img_2 = imread("2.jpg");
Mat img_3 = imread("3.jpg");
Mat img_4 = imread("4.jpg");
Mat dst;
vector<Mat> manyimgV;
manyimgV.push_back(img_1);
manyimgV.push_back(img_2);
manyimgV.push_back(img_3);
manyimgV.push_back(img_4);
ManyImages(manyimgV, dst,2);
imshow("ManyImagesInWindow", dst);
waitKey(0);

��ע�������ٶȣ�Լ960ms��95%ʱ������resize��
*/
void ManyImages(vector<Mat> Images, Mat& dst, int imgRows);