//
// Created by shensir on 17-5-17.
// http://datahonor.com/2017/05/18/%E8%87%AA%E5%8A%A8%E5%85%89%E5%AD%A6%E6%A3%80%E6%B5%8B%EF%BC%8C%E7%9B%AE%E6%A0%87%E5%88%86%E5%89%B2%E5%92%8C%E6%A3%80%E6%B5%8B/
#include <iostream>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace std;
using namespace cv;
// 光纹去除
Mat removeLight(Mat img, Mat pattern, int method){
	Mat aux;
	// 如果方法是归一化
	if (method == 1){
		// 相除时需要将图像改为32位浮点型
		Mat img32, pattern32;
		img.convertTo(img32, CV_32F);
		pattern.convertTo(pattern32, CV_32F);
		// 图像相除模式
		aux = 255 * (1 - (img32 / pattern32));
		// 换回8bit
		aux.convertTo(aux, CV_8U);
	}
	else{
		aux = pattern - img;
	}
	return aux;
}
// 估计背景图像，大尺寸核矩阵模糊
Mat calculateLightPattern(Mat img){
	Mat pattern;
	blur(img, pattern, Size(img.cols / 3, img.rows / 3));
	return pattern;
}
// 先进行形态学操作[在背景色是核函数模糊得到的时候，很有必要]，再二值化
Mat getBinary(Mat img_no_light, int method_light){
	Mat img_thr;
	if (method_light != 2){
		threshold(img_no_light, img_thr, 30, 255, THRESH_BINARY);
	}
	else{
		threshold(img_no_light, img_thr, 140, 255, THRESH_BINARY_INV);
	}
	// 进行形态学操作去除边缘不规则噪点
	cv::Mat closed;
	cv::Mat element5(5, 5, CV_8U, cv::Scalar(1));
	cv::morphologyEx(img_thr, closed,   // input and output
		cv::MORPH_CLOSE,  // operator code
		element5);  // structuring element
	// Opening
	cv::Mat opening;
	cv::morphologyEx(closed, opening,   // input and output
		cv::MORPH_OPEN,  // operator code
		element5);  // structuring element
	imshow("img_thr_closed-opening", opening);
	return opening;
}
static Scalar randomColor(RNG& rng)
{
	int icolor = (unsigned)rng;
	return Scalar(icolor & 255, (icolor >> 8) & 255, (icolor >> 16) & 255);
}
// 连通区域算法
void ConnectedComponents(Mat img){
	// 使用连通区域分离符合要求部分图像Mat标签
	Mat labels;
	int num_objects = connectedComponents(img, labels);
	// 检查检测到的目标数目
	if (num_objects < 2){
		cout << "No objects detected" << endl;
		return;
	}
	else{
		cout << "Number of objects detected: " << num_objects - 1 << endl;
	}
	// 创建彩色目标的输出图像
	Mat output = Mat::zeros(img.rows, img.cols, CV_8UC3);
	RNG rng(0xFFFFFFFF);
	for (int i = 1; i < num_objects; i++){
		Mat mask = labels == i;
		output.setTo(randomColor(rng), mask);
	}
	imshow("Result", output);
}
// 连通区域算法
void ConnectedComponentsStats(Mat img){
	// 连通区域统计信息
	Mat labels, stats, centroids;
	int num_objects = connectedComponentsWithStats(img, labels, stats, centroids);
	// 检测物体数目判别
	if (num_objects < 2){
		cout << "No objects detedcted" << endl;
		return;
	}
	else{
		cout << "Number of objects detected: " << num_objects - 1 << endl;
	}
	// 创建彩色对象的输出图像并显示区域
	Mat output = Mat::zeros(img.rows, img.cols, CV_8UC3);
	RNG rng(0xFFFFFFFF);
	for (int i = 1; i < num_objects; i++){
		cout << "Object " << i << " with pos: " << centroids.at<Point2f>(i)
			<< " with area " << stats.at<int>(i, CC_STAT_AREA) << endl;
		Mat mask = labels == i;
		output.setTo(randomColor(rng), mask);
		// 使用区域绘制文本
		stringstream ss;
		ss << "area: " << stats.at<int>(i, CC_STAT_AREA);
		putText(output, ss.str(), centroids.at<Point2f>(i), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 255, 255));
	}
	imshow("Result", output);
}
//  边缘检测算法
void FindContoursBasic(Mat img){
	vector<vector<Point>> contours;
	findContours(img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat output = Mat::zeros(img.rows, img.cols, CV_8UC3);
	// 检查检测到的对象的数目
	if (contours.size() == 0){
		cout << "No objects detected" << endl;
		return;
	}
	else{
		cout << "Number of objects detected: " << contours.size() << endl;
	}
	RNG rng(0xFFFFFFFF);
	for (int i = 0; i < contours.size(); i++){
		drawContours(output, contours, i, randomColor(rng));
	}
	imshow("Contours", output);
}
int main(){
	Mat img = imread("test.png", IMREAD_GRAYSCALE);
	imshow("Original Image", img);
	Mat pattern = imread("light.png", 0);
	imshow("Back", pattern);
	// Mat pattern = calculateLightPattern(img);
	Mat img_no_light = removeLight(img, pattern, 1);
	imshow("removelight--minus", img_no_light);
	Mat img_thr = getBinary(img_no_light, 1);
	// ConnectedComponents(img_thr);
	ConnectedComponentsStats(img_thr);
	FindContoursBasic(img_thr);
	waitKey(0);
	return 0;
}
