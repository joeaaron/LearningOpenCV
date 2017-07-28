/*
* \file: svmSampleDemo
* \brief: 简单的支持向量机示例
*
* \详细概述: 选取8个样本点，进行分类
* 通过绿色和蓝色来划分
*
* \author: Aaron | Email: joeaaron007@gmail.com
* \version 1.0
* \date: 2017/7/28
*/
#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>

int main(int, char**)
{
	int width = 512;
	int height = 512;
	cv::Mat image = cv::Mat::zeros(height, width, CV_8UC3);

	int labels[8] = { 1, 1, 0, 0, 1, 1, 0, 0 };
	cv::Mat labelsMat(8, 1, CV_32SC1, labels);

	float trainingData[8][2] = { { 10, 10 },				//8 样本点（和结果对应）
	{ 10, 50 },
	{ 501, 255 },
	{ 500, 501 },
	{ 40, 30 },
	{ 70, 60 },
	{ 300, 300 },
	{ 60, 500 } };

	cv::Mat trainingDataMat(8, 2, CV_32FC1, trainingData);
	
	cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
	svm->setType(cv::ml::SVM::C_SVC);       //type
	svm->setKernel(cv::ml::SVM::LINEAR);    //kernel function
	cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(trainingDataMat, cv::ml::ROW_SAMPLE, labelsMat);      //样本是按行排列的

	svm->train(td);        //训练
	
	cv::Vec3b green(0, 255, 0), blue(255, 0, 0);
	cv::Mat sampleMat(1, 2, CV_32F);
	float response;
	//预测512 * 512正方形区域内每个点的归类
	for (int i = 0; i < image.rows; ++i)
	{
		for (int j = 0; j < image.cols; ++j)
		{
			sampleMat.at<float>(0, 0) = i;
			sampleMat.at<float>(0, 1) = j;
			response = svm->predict(sampleMat);

			if (response == 1)
				image.at <cv::Vec3b>(i, j) = green;
			else
				image.at<cv::Vec3b>(i, j) = blue;
		}
	}

	//标出样本点的位置
	int thickness = -1;
	int lineType = 8;
	int x, y;
	cv::Scalar s;
	for (int m = 0; m < 8; ++m)
	{
		if (labels[m])
			s = cv::Scalar(255, 0, 255);
		else
			s = cv::Scalar(255, 255, 0);

		x = trainingData[m][0];
		y = trainingData[m][1];
		cv::circle(image, cv::Point(x, y), 5, s, thickness, lineType);
	}
	cv::imshow("CV SVM SAMPLE", image);
	cv::waitKey(0);
}