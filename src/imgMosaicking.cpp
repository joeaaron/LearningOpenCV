#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <vector>

void OptimizeSeam(cv::Mat& img1, cv::Mat& trans, cv::Mat& dst); 

typedef struct 
{
	cv::Point2f leftTop;
	cv::Point2f leftBottom;
	cv::Point2f rightTop;
	cv::Point2f rightBottom;
}four_Corners_t;

four_Corners_t corners;

void CalcCorners(const cv::Mat& H, const cv::Mat& src)
{
	double v2[] = { 0, 0, 1 };//���Ͻ�
	double v1[3];//�任�������ֵ
	cv::Mat V2 = cv::Mat(3, 1, CV_64FC1, v2);  //������
	cv::Mat V1 = cv::Mat(3, 1, CV_64FC1, v1);  //������

	V1 = H * V2;
	//���Ͻ�(0,0,1)
	std::cout << "V2: " << V2 << std::endl;
	std::cout << "V1: " << V1 << std::endl;
	corners.leftTop.x = v1[0] / v1[2];
	corners.leftTop.y = v1[1] / v1[2];

	//���½�(0,src.rows,1)
	v2[0] = 0;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = cv::Mat(3, 1, CV_64FC1, v2);  //������
	V1 = cv::Mat(3, 1, CV_64FC1, v1);  //������
	V1 = H * V2;
	corners.leftBottom.x = v1[0] / v1[2];
	corners.leftBottom.y = v1[1] / v1[2];

	//���Ͻ�(src.cols,0,1)
	v2[0] = src.cols;
	v2[1] = 0;
	v2[2] = 1;
	V2 = cv::Mat(3, 1, CV_64FC1, v2);  //������
	V1 = cv::Mat(3, 1, CV_64FC1, v1);  //������
	V1 = H * V2;
	corners.rightTop.x = v1[0] / v1[2];
	corners.rightTop.y = v1[1] / v1[2];

	//���½�(src.cols,src.rows,1)
	v2[0] = src.cols;
	v2[1] = src.rows;
	v2[2] = 1;
	V2 = cv::Mat(3, 1, CV_64FC1, v2);  //������
	V1 = cv::Mat(3, 1, CV_64FC1, v1);  //������
	V1 = H * V2;
	corners.rightBottom.x = v1[0] / v1[2];
	corners.rightBottom.y = v1[1] / v1[2];
}

///�Ż�ͼ�����Ӵ���ʹ��ƴ����Ȼ
void OptimizeSeam(cv::Mat& img1, cv::Mat& trans, cv::Mat& dst)
{
	int start = MIN(corners.leftTop.x, corners.leftBottom.x);		//��ʼ���򣬼��ص��������߽�
	double processWidth = img1.cols - start;		//�ص�������
	int rows = dst.rows;
	int cols = img1.cols;
	double alpha = 1;
	for (int i = 0; i < rows; i++)
	{
		uchar* p = img1.ptr<uchar>(i);
		uchar* t = trans.ptr<uchar>(i);
		uchar* d = dst.ptr<uchar>(i);

		for (int j = 0; j < cols; j++)
		{
			//�������ͼ��trans�������صĺڵ㣬����ȫ����img1�е�����
			if (t[j * 3] == 0 && t[j * 3 + 1] == 0 && t[j * 3 + 2] == 0)
				alpha = 1;
			else
				alpha = (processWidth - (j - start)) / processWidth;

			d[j * 3] = p[j * 3] * alpha + t[j * 3] * (1 - alpha);
			d[j * 3 + 1] = p[j * 3 + 1] * alpha + t[j * 3 + 1] * (1 - alpha);
			d[j * 3 + 2] = p[j * 3 + 2] * alpha + t[j * 3 + 2] * (1 - alpha);
		}
	}

}

int main()
{
	
	cv::Mat img1 = cv::imread("img07.jpg", 1);
	cv::Mat img2 = cv::imread("img08.jpg", 1);
	if (img1.empty() || img2.empty())
	{
		std::cout << "Can't read one of the images!" << std::endl;;
		return -1;
	}

	cv::Mat img1Gray;
	cv::Mat img2Gray;

	cvtColor(img1, img1Gray, CV_BGR2GRAY);
	cvtColor(img2, img2Gray, CV_BGR2GRAY);

	///��ȡ������
	//cv::xfeatures2d::SurfFeatureDetector Detector(2000);
	cv::Ptr<cv::xfeatures2d::SurfFeatureDetector> detector = cv::xfeatures2d::SurfFeatureDetector::create(2000);
	std::vector<cv::KeyPoint> keyPoint1, keyPoint2;
	detector->detect(img1Gray, keyPoint1);
	detector->detect(img2Gray, keyPoint2);

	///����������
	//cv::SurfDescriptorExtractor Descriptor;
	cv::Ptr<cv::xfeatures2d::SURF> Descriptor = cv::xfeatures2d::SURF::create();
	cv::Mat imgDesc1, imgDesc2;
	Descriptor->compute(img1Gray, keyPoint1, imgDesc1);
	Descriptor->compute(img2Gray, keyPoint2, imgDesc2);

	cv::FlannBasedMatcher matcher;
	std::vector<std::vector<cv::DMatch>> matchPoints;
	std::vector<cv::DMatch> GoodMatchPoints;

	std::vector<cv::Mat> trainDesc(1, imgDesc1);
	matcher.add(trainDesc);
	matcher.train();

	matcher.knnMatch(imgDesc2, matchPoints, 2);
	std::cout << "total match points: " << matchPoints.size() << std::endl;

	///Lowe's algorithm, ��ȡ����ƥ���
	for (int i = 0; i < matchPoints.size(); i++)
	{
		if (matchPoints[i][0].distance < 0.4 * matchPoints[i][1].distance)
			GoodMatchPoints.push_back(matchPoints[i][0]);
	}

	cv::Mat firstMatch;
	drawMatches(img2, keyPoint2, img1, keyPoint1, GoodMatchPoints, firstMatch);
	cv::imshow("firstMatch", firstMatch);

	std::vector<cv::Point2f> imagePoints1, imagePoints2;

	for (int i = 0; i < GoodMatchPoints.size(); i++)
	{
		imagePoints2.push_back(keyPoint2[GoodMatchPoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchPoints[i].trainIdx].pt);
	}

	///��ȡͼ��1��ͼ��2��ͶӰӳ����󣬳ߴ�Ϊ3*3
	cv::Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
	std::cout << "�任����Ϊ��\n" << homo << std::endl << std::endl;  //���ӳ�����      
	   
	///������׼ͼ���ĸ���������
	CalcCorners(homo, img1);
	std::cout << "left_top:" << corners.leftTop << std::endl;
	std::cout << "left_bottom:" << corners.leftBottom << std::endl;
	std::cout << "right_top:" << corners.rightTop << std::endl;
	std::cout << "right_bottom:" << corners.rightBottom << std::endl;

	///ͼ����׼
	cv::Mat imgTransform1, imgTransform2;
	cv::Mat adjustMat = (cv::Mat_<double>(3, 3) << 1.0, 0, img1.cols, 0, 1.0, 0, 0, 0, 1.0);
	warpPerspective(img1, imgTransform1, homo, cv::Size(MAX(corners.rightTop.x, corners.rightBottom.x), img2.rows));
	//warpPerspective(img1, imgTransform2, adjustMat*homo, cv::Size(img2.cols*1.3, img2.rows*1.8));
	cv::imshow("ֱ�Ӿ���͸�Ӿ���任", imgTransform1);
	imwrite("trans1.png", imgTransform1);

	//����ƴ�Ӻ��ͼ������ǰ����ͼ�Ĵ�С
	int dstWidth = imgTransform1.cols;
	int dstHeight = img2.rows;

	cv::Mat dst(dstHeight, dstWidth, CV_8UC3);
	dst.setTo(0);

	imgTransform1.copyTo(dst(cv::Rect(0, 0, imgTransform1.cols, imgTransform1.rows)));
	img2.copyTo(dst(cv::Rect(0, 0, img2.cols, img2.rows)));
	cv::imshow("b_dst", dst);
	
	OptimizeSeam(img2, imgTransform1, dst);
	cv::imshow("dst", dst);
	cv::imwrite("dst.png", dst);

	cv::waitKey(0);
	return 0;
}