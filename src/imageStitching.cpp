// ���ܣ����� 9-3 ͼ��ƴ�Ӳ���
// ���ߣ���ΰ zhu1988wei@163.com
// ��Դ����OpenCVͼ������ʵ����
// ���ͣ�http://blog.csdn.net/zhuwei1988
// ���£�2016-8-1
// ˵������Ȩ���У����û�ժ¼����ϵ���ߣ������������ʽע��������лл��// 
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
	// ����Stitcher��
	Stitcher stitcher = Stitcher::createDefault(Flag);
	Stitcher::Status status = stitcher.stitch(imgs, resultMat);
	if (status != Stitcher::OK) {
		std::cout << "error" << std::endl;
	}
}

//����ԭʼͼ���λ�ھ�������任����Ŀ��ͼ���϶�Ӧλ��  
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
	//�Ҷ�ͼת��  
	Mat image1, image2;
	cvtColor(srcImage1, image1, CV_RGB2GRAY);
	cvtColor(srcImage2, image2, CV_RGB2GRAY);

	//��ȡ������    
	cv::Ptr<Feature2D> sift = xfeatures2d::SIFT::create(800);

	//SiftFeatureDetector siftDetector(800);  // ����������ֵ  
	vector<KeyPoint> keyPoint1, keyPoint2;
	sift->detect(image1, keyPoint1);
	sift->detect(image2, keyPoint2);

	//������������Ϊ�±ߵ�������ƥ����׼��    
	Mat imageDesc1, imageDesc2;
	sift->compute(image1, keyPoint1, imageDesc1);
	sift->compute(image2, keyPoint2, imageDesc2);

	//���ƥ�������㣬����ȡ�������     
	FlannBasedMatcher matcher;
	vector<DMatch> matchePoints;
	matcher.match(imageDesc1, imageDesc2, matchePoints, Mat());
	sort(matchePoints.begin(), matchePoints.end()); //����������    
	
	//��ȡ����ǰN��������ƥ��������  
	vector<Point2f> imagePoints1, imagePoints2;
	for (int i = 0; i < 10; i++)
	{
		imagePoints1.push_back(keyPoint1[matchePoints[i].queryIdx].pt);
		imagePoints2.push_back(keyPoint2[matchePoints[i].trainIdx].pt);
	}
	//��ȡͼ��1��ͼ��2��ͶӰӳ����󣬳ߴ�Ϊ3*3  
	Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
	Mat adjustMat = (Mat_<double>(3, 3) << 1.0, 0, srcImage1.cols, 0, 1.0, 0, 0, 0, 1.0);
	Mat adjustHomo = adjustMat*homo;

	//��ȡ��ǿ��Ե���ԭʼͼ��;���任��ͼ���ϵĶ�Ӧλ�ã�����ͼ��ƴ�ӵ�Ķ�λ  
	Point2f originalLinkPoint, targetLinkPoint, basedImagePoint;
	originalLinkPoint = keyPoint1[matchePoints[0].queryIdx].pt;
	targetLinkPoint = getTransformPoint(originalLinkPoint, adjustHomo);
	basedImagePoint = keyPoint2[matchePoints[0].trainIdx].pt;

	//ͼ����׼  
	Mat imageTransform1;
	warpPerspective(srcImage1, imageTransform1, adjustMat*homo, Size(srcImage2.cols + srcImage1.cols + 110, srcImage2.rows));

	//����ǿƥ��������ص���������ۼӣ����ν��ȶ����ɣ�����ͻ��  
	Mat image1Overlap, image2Overlap; //ͼ1��ͼ2���ص�����     
	image1Overlap = imageTransform1(Rect(Point(targetLinkPoint.x - basedImagePoint.x, 0), Point(targetLinkPoint.x, srcImage2.rows)));
	image2Overlap = srcImage2(Rect(0, 0, image1Overlap.cols, image1Overlap.rows));
	Mat image1ROICopy = image1Overlap.clone();  //����һ��ͼ1���ص�����  
	for (int i = 0; i < image1Overlap.rows; i++)
	{
		for (int j = 0; j < image1Overlap.cols; j++)
		{
			double weight;
			weight = (double)j / image1Overlap.cols;  //�����ı���ı�ĵ���ϵ��  
			image1Overlap.at<Vec3b>(i, j)[0] = (1 - weight)*image1ROICopy.at<Vec3b>(i, j)[0] + weight*image2Overlap.at<Vec3b>(i, j)[0];
			image1Overlap.at<Vec3b>(i, j)[1] = (1 - weight)*image1ROICopy.at<Vec3b>(i, j)[1] + weight*image2Overlap.at<Vec3b>(i, j)[1];
			image1Overlap.at<Vec3b>(i, j)[2] = (1 - weight)*image1ROICopy.at<Vec3b>(i, j)[2] + weight*image2Overlap.at<Vec3b>(i, j)[2];
		}
	}
	Mat ROIMat = srcImage2(Rect(Point(image1Overlap.cols, 0), Point(srcImage2.cols, srcImage2.rows)));   //ͼ2�в��غϵĲ���  
	ROIMat.copyTo(Mat(imageTransform1, Rect(targetLinkPoint.x, 0, ROIMat.cols, srcImage2.rows))); //���غϵĲ���ֱ���ν���ȥ  
	imwrite("./ƴ�ӽ��.jpg", imageTransform1);
	
	panorama = imageTransform1.clone();
	//// ������
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
