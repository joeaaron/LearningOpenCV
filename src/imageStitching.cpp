// 功能：代码 9-3 图像拼接操作
// 作者：朱伟 zhu1988wei@163.com
// 来源：《OpenCV图像处理编程实例》
// 博客：http://blog.csdn.net/zhuwei1988
// 更新：2016-8-1
// 说明：版权所有，引用或摘录请联系作者，并按照上面格式注明出处，谢谢。// 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/stitching.hpp>
#include <iostream>
#include <vector>  
using namespace std;
using namespace cv;

void stitch(std::vector<cv::Mat> imgs, cv::Mat& resultMat)
{
	bool Flag = true;
	// 定义Stitcher类
	Stitcher stitcher = Stitcher::createDefault(Flag);
	Stitcher::Status status = stitcher.stitch(imgs, resultMat);
	if (status != Stitcher::OK) {
		std::cout << "error" << std::endl;
	}
}

//计算原始图像点位在经过矩阵变换后在目标图像上对应位置  
Point2f getTransformPoint(const Point2f originalPoint, const Mat &transformMaxtri)
{
	Mat originelP, targetP;
	originelP = (Mat_<double>(3, 1) << originalPoint.x, originalPoint.y, 1.0);
	targetP = transformMaxtri*originelP;
	float x = targetP.at<double>(0, 0) / targetP.at<double>(2, 0);
	float y = targetP.at<double>(1, 0) / targetP.at<double>(2, 0);
	return Point2f(x, y);
}

void stitch2(Mat& srcImage1, Mat& srcImage2, Mat& panorama)
{
	//灰度图转换  
	Mat image1, image2;
	cvtColor(srcImage1, image1, CV_RGB2GRAY);
	cvtColor(srcImage2, image2, CV_RGB2GRAY);

	//提取特征点    
	cv::Ptr<Feature2D> sift = xfeatures2d::SIFT::create(800);

	//SiftFeatureDetector siftDetector(800);  // 海塞矩阵阈值  
	vector<KeyPoint> keyPoint1, keyPoint2;
	sift->detect(image1, keyPoint1);
	sift->detect(image2, keyPoint2);

	//特征点描述，为下边的特征点匹配做准备    
	Mat imageDesc1, imageDesc2;
	sift->compute(image1, keyPoint1, imageDesc1);
	sift->compute(image2, keyPoint2, imageDesc2);

	//获得匹配特征点，并提取最优配对     
	FlannBasedMatcher matcher;
	vector<DMatch> matchePoints;
	matcher.match(imageDesc1, imageDesc2, matchePoints, Mat());
	sort(matchePoints.begin(), matchePoints.end()); //特征点排序    
	
	//获取排在前N个的最优匹配特征点  
	vector<Point2f> imagePoints1, imagePoints2;
	for (int i = 0; i < 10; i++)
	{
		imagePoints1.push_back(keyPoint1[matchePoints[i].queryIdx].pt);
		imagePoints2.push_back(keyPoint2[matchePoints[i].trainIdx].pt);
	}
	//获取图像1到图像2的投影映射矩阵，尺寸为3*3  
	Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
	Mat adjustMat = (Mat_<double>(3, 3) << 1.0, 0, srcImage1.cols, 0, 1.0, 0, 0, 0, 1.0);
	Mat adjustHomo = adjustMat*homo;

	//获取最强配对点在原始图像和矩阵变换后图像上的对应位置，用于图像拼接点的定位  
	Point2f originalLinkPoint, targetLinkPoint, basedImagePoint;
	originalLinkPoint = keyPoint1[matchePoints[0].queryIdx].pt;
	targetLinkPoint = getTransformPoint(originalLinkPoint, adjustHomo);
	basedImagePoint = keyPoint2[matchePoints[0].trainIdx].pt;

	//图像配准  
	Mat imageTransform1;
	warpPerspective(srcImage1, imageTransform1, adjustMat*homo, Size(srcImage2.cols + srcImage1.cols + 110, srcImage2.rows));

	//在最强匹配点左侧的重叠区域进行累加，是衔接稳定过渡，消除突变  
	Mat image1Overlap, image2Overlap; //图1和图2的重叠部分     
	image1Overlap = imageTransform1(Rect(Point(targetLinkPoint.x - basedImagePoint.x, 0), Point(targetLinkPoint.x, srcImage2.rows)));
	image2Overlap = srcImage2(Rect(0, 0, image1Overlap.cols, image1Overlap.rows));
	Mat image1ROICopy = image1Overlap.clone();  //复制一份图1的重叠部分  
	for (int i = 0; i < image1Overlap.rows; i++)
	{
		for (int j = 0; j < image1Overlap.cols; j++)
		{
			double weight;
			weight = (double)j / image1Overlap.cols;  //随距离改变而改变的叠加系数  
			image1Overlap.at<Vec3b>(i, j)[0] = (1 - weight)*image1ROICopy.at<Vec3b>(i, j)[0] + weight*image2Overlap.at<Vec3b>(i, j)[0];
			image1Overlap.at<Vec3b>(i, j)[1] = (1 - weight)*image1ROICopy.at<Vec3b>(i, j)[1] + weight*image2Overlap.at<Vec3b>(i, j)[1];
			image1Overlap.at<Vec3b>(i, j)[2] = (1 - weight)*image1ROICopy.at<Vec3b>(i, j)[2] + weight*image2Overlap.at<Vec3b>(i, j)[2];
		}
	}
	Mat ROIMat = srcImage2(Rect(Point(image1Overlap.cols, 0), Point(srcImage2.cols, srcImage2.rows)));   //图2中不重合的部分  
	ROIMat.copyTo(Mat(imageTransform1, Rect(targetLinkPoint.x, 0, ROIMat.cols, srcImage2.rows))); //不重合的部分直接衔接上去  
	imwrite("./拼接结果.jpg", imageTransform1);
	
	panorama = imageTransform1.clone();
	//// 结果输出
	//Mat roi(panorama, Rect(0, 0,
	//	srcImage1.cols, srcImage1.rows));
	//srcImage1.copyTo(roi);
}
int main(int argc, char* argv[])
{
	cv::Mat image1 = imread("img07.JPG");
	cv::Mat image2 = imread("img08.JPG");
	cv::Mat image3 = imread("img09.JPG");
	if (!image1.data || !image2.data || !image3.data)
		return -1;
	vector<Mat> imgs;
	imgs.push_back(image1);
	imgs.push_back(image2);
	imgs.push_back(image3);
	cv::Mat resultMat1, resultMat2, resultMat;
	stitch(imgs, resultMat1);
	cv::imshow("resultMat1", resultMat1);
	stitch2(image2, image3, resultMat2); 
	cv::imshow("resultMat2", resultMat2);
	//stitch2(resultMat2, image3, resultMat);
	//cv::imshow("resultMat2", resultMat);
	cv::waitKey(0);
	return 0;
}
