#include<opencv2\opencv.hpp>
#include<vector>
using namespace std;
using namespace cv;
/*使用容器vector和ROI方法完成一个窗口显示多张图片
Images是vector<Mat>,可有push_back方法往里面加若干图片
dst为输出图像，即将所有图片放在一张图片中
imgRows是每行显示几张图片，不填默认是3
示例代码：
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

备注：运行速度：约960ms，95%时间用在resize上
*/
void ManyImages(vector<Mat> Images, Mat& dst, int imgRows);