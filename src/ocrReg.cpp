#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;

/// Global variables
Mat src, src_gray, src_th, erosion_dst;
Mat dst, dst_gray, dst_th;
Mat original, result;

const char* inputImage = "ocr_1.jpg";
const int threshval = 80;

int erosion_elem = 0;
int erosion_size = 8;

#define BASE 45
RNG &rng = theRNG();
Rect rect;

vector<vector<Point> > contours;
vector<vector<Point> > _contours;
Point2f rect_points[4];

void processImage(int /*h*/, void*)
{
	cvtColor(src, src_gray, CV_BGR2GRAY);
	threshold(src_gray, src_th, threshval, 250, THRESH_BINARY_INV);

	erode(src_th, src_th, Mat());
	dilate(src_th, src_th, Mat());

	
	vector<Vec4i>hierarchy;
	//Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC3);
	findContours(src_th, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	//��ȥ̫������̫�̵�����  
	int cmin = 10;
	int cmax = 70;
	std::vector<std::vector<Point>>::const_iterator itc = contours.begin();
	while (itc != contours.end())
	{
		if (itc->size() < cmin || itc->size() > cmax)
			itc = contours.erase(itc);
		else
			++itc;
	}

	dst = Mat::zeros(src.rows, src.cols, CV_8UC3);
	dst.setTo(Scalar(255, 255, 255));
	drawContours(dst, contours, -1, Scalar(0, 0, 255), 2);
	imshow("IMG", dst);

	cvtColor(dst, dst_gray, CV_BGR2GRAY);
	threshold(dst_gray, dst_th, 80, 255, THRESH_BINARY);

	int erosion_type;
	if (erosion_elem == 0){ erosion_type = MORPH_RECT; }
	else if (erosion_elem == 1){ erosion_type = MORPH_CROSS; }
	else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	/// ��ʴ����
	erode(dst_th, erosion_dst, element);


	vector<Vec4i>_hierarchy;
	//Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC3);
	findContours(erosion_dst, _contours, _hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	//��ȥ̫������̫�̵�����  
	int min = 50;
	int max = 1000;
	std::vector<std::vector<Point>>::const_iterator itd = _contours.begin();
	while (itd != _contours.end())
	{
		if (itd->size() < min || itd->size() > max)
			itd = _contours.erase(itd);
		else
			++itd;
	}

	// ��ÿ���ҵ���������������б�ı߽�����Բ  
	vector<RotatedRect> minRect(_contours.size());
	for (int i = 0; i < _contours.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(_contours[i]));

	}
	/// ��������������б�ı߽��ͱ߽���Բ  
	for (int i = 0; i < _contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		// contour  
		//drawContours(original, _contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		rect = cv::boundingRect(_contours[i]);
		minRect[i].points(rect_points);
		for (int j = 0; j < 4; j++)
			line(src, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
	}
	Mat hole(src.size(), CV_8U, Scalar(0)); //����ͼ��  
	cv::drawContours(hole, _contours, -1, Scalar(255), CV_FILLED); //������ͼ���ϣ��ð�ɫ�����������  
	namedWindow("My hole");
	imshow("My hole", hole);
	Mat crop(src.rows, src.cols, CV_8UC3);
	src.copyTo(crop, hole);//��ԭͼ�񿽱�������ͼ��  
	namedWindow("My warpPerspective");
	imshow("My warpPerspective", crop);
}
//ͼ����ת: srcΪԭͼ�� dstΪ��ͼ��, angleΪ��ת�Ƕ�, isClip��ʾ�ǲ�ȡ��СͼƬ�ķ�ʽ
int imageRotate4(InputArray src, OutputArray dst, double angle, bool isClip)
{
	Mat input = src.getMat();
	if (input.empty()) {
		return -1;
	}

	//�õ�ͼ���С
	int width = input.cols;
	int height = input.rows;

	//����ͼ�����ĵ�
	Point2f center;
	center.x = width / 2.0;
	center.y = height / 2.0;

	//�����ת�任����
	double scale = 1.0;
	Mat trans_mat = getRotationMatrix2D(center, -angle, scale);

	//������ͼ���С
	double angle1 = angle  * CV_PI / 180.;
	double a = sin(angle1) * scale;
	double b = cos(angle1) * scale;
	double out_width = height * fabs(a) + width * fabs(b); //��߿򳤶�
	double out_height = width * fabs(a) + height * fabs(b);//��߿�߶�

	int new_width, new_height;
	if (!isClip) {
		new_width = cvRound(out_width);
		new_height = cvRound(out_height);
	}
	else {
		//calculate width and height of clip rect
		double angle2 = fabs(atan(height * 1.0 / width)); //���Ƕ� b
		double len = width * fabs(b);
		double Y = len / (1 / fabs(tan(angle1)) + 1 / fabs(tan(angle2)));
		double X = Y * 1 / fabs(tan(angle2));
		new_width = cvRound(out_width - X * 2);
		new_height = cvRound(out_height - Y * 2);
	}

	//����ת�任�����м���ƽ����
	trans_mat.at<double>(0, 2) += cvRound((new_width - width) / 2);
	trans_mat.at<double>(1, 2) += cvRound((new_height - height) / 2);

	//����任
	warpAffine(input, dst, trans_mat, Size(new_width, new_height));

	return 0;
}

int main()
{
	original = imread(inputImage);
	if (original.empty())
	{
		cout << "Could not read input image file: " << inputImage << endl;
		return -1;
	}
	src = original.clone();
	processImage(0, 0);
	
	//����ͼ��
	float dx = rect_points[0].x - rect_points[1].x;
	float dy = rect_points[0].y - rect_points[1].y;
	float rotateDegree = atan2f(dy, dx);   //(float)CV_PI*180.0f;
	double angle = rotateDegree  * CV_PI / 180.; // ����  

	bool isClip = true;
	imageRotate4(original, result, rotateDegree - BASE, isClip);
	namedWindow("ROT Image");
	imshow("ROI Image", result);

	//Mat roiImage = original(rect);

	waitKey(0);
	return 0;
}

