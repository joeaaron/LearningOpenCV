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

/*----------------------------
* ��|�� : ��ͼ�ں�
*----------------------------
* ���� : MulitBlend
* ���� : private
* ���� : Mat&
*
* ����y : matinput      [in]     ͼƬ��������D
* ����y : imagesrc      [in]     �Ѿ�-��׼��ͼƬ
* ����y : matloc1       [in]     ��һͼƥ��λ��
* ����y : matloc2       [in]     �ڶ�ͼƥ��λ��
*/
Mat MultiBlend(deque<Mat>& input, const Mat& src, deque<Point>& loc1, deque<Point>& loc2)
{
	Mat outImg;
	src.copyTo(outImg);
	int ioffsetdetail = 0;
	double dblend = 0.0;
	for (int i = 0; i < loc1.size() - 1;i++)
	{
		int ioffset = loc1[i].y - loc2[i].y;      //row��ƫ��
		for (int j = 0; j < 100; j++)
		{
			outImg.row(ioffsetdetail + ioffset + j) = input[i].row(ioffset + j)*(1 - dblend) + input[i + 1].row(j) * (dblend);
			dblend = dblend + 0.01;
		}
		ioffsetdetail += ioffset;
	}
	return outImg;
}

void stitch2(Mat& srcImage1, Mat& srcImage2, Mat& panorama)
{
	// SURF����������
	Mat srcImage2Warped;
	// ��ʼ��SURF���������
	cv::Ptr<Feature2D> surf = xfeatures2d::SURF::create();

	// �ؼ��㼰����������������
	vector<cv::KeyPoint> kPointMat, kPointMat2;;
	cv::Mat desMat1, desMat2;

	surf->detectAndCompute(srcImage1, Mat(), kPointMat, desMat1);
	surf->detectAndCompute(srcImage2, Mat(), kPointMat2, desMat2);

	// FLANN �ؼ���ƥ��
	// FLANN ��ʼ�� RANSACһ���� ���������
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(desMat1, desMat2, matches);
	double max_dist = 0, min_dist = 100;
	// �������������С�������
	for (int i = 0; i < desMat1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	// ʹ��good �������������޶� 
	std::vector< DMatch > good_matches;
	for (int i = 0; i < desMat1.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}
	// ͼ��Ĺؼ���ƥ��
	std::vector<Point2f> srcImage1_matchedKPs;
	std::vector<Point2f> srcImage2_matchedKPs;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		srcImage1_matchedKPs.push_back(
			kPointMat[good_matches[i].queryIdx].pt);		srcImage2_matchedKPs.push_back(
			kPointMat2[good_matches[i].trainIdx].pt);
	}
	// ����ͼ��1��ͼ��2��ӳ��
	Mat H = findHomography(Mat(srcImage2_matchedKPs),
		Mat(srcImage1_matchedKPs), CV_RANSAC);
	// ����任
	warpPerspective(srcImage2, srcImage2Warped, H,
		Size(srcImage2.cols * 2, srcImage2.rows), INTER_CUBIC);
	panorama = srcImage2Warped.clone();
	// ������
	Mat roi(panorama, Rect(0, 0,
		srcImage1.cols, srcImage1.rows));
	srcImage1.copyTo(roi);
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
	cv::Mat resultMat1, resultMat2;
	stitch(imgs, resultMat1);
	cv::imshow("resultMat1", resultMat1);
	stitch2(image1, image2, resultMat2);
	cv::imshow("resultMat2", resultMat2);
	cv::waitKey(0);
	return 0;
}
