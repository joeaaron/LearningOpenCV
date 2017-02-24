/*!
* \file: OCR_Crop.cpp
* \brief: �����ֲ��ַָ����
*
* \��ϸ����: ��̬ѧ������������⡢��Ӿ��Ρ���תУ��
*           ģ��ƥ�䶨λ
*
* \author: Aaron | Email: joeaaron007@gmail.com
* \version 1.0
* \date: 2017/2/7
*/

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;

void MatchingMethod(Mat result, Mat templ, int i);
int imageRotate(InputArray src, OutputArray dst, double angle, bool isClip);
double GetDistance(Point2f pointA, Point2f pointB);


/*----------------------------
* ���� :	 ͼ��ָ�
*----------------------------
* ���� : ImageCrop
* ���� : public
* ���� : ������ͼ��
*
* ���� :
* ���� :
*/
int ImageCrop(InputArray src, OutputArray dst, Rect rect)
{
	Mat input = src.getMat();
	if (input.empty()){
		return -1;
	}

	//����������򣺼���Rect��Դͼ������Rect�Ľ���
	Rect srcRect(0, 0, input.cols, input.rows);
	rect = rect & srcRect;
	if (rect.width <= 0 || rect.height <= 0) return -1;

	//�������ͼ��
	dst.create(Size(rect.width, rect.height), src.type());
	Mat output = dst.getMat();
	if (output.empty()) return -1;

	try{
		input(rect).copyTo(output);
		return 0;
	}
	catch (...){
		return -1;
	}
}

/*----------------------------
* ���� : ģ��ƥ��
*----------------------------
* ���� : MatchingMethod
* ���� : public
* ���� : �޷���ֵ
*
* ���� : 
* ���� :
*/
void MatchingMethod(Mat result, Mat templ, int i)
{
	int match_method;
	Mat img;
	
	result.copyTo(img);
	Mat img_display;
	img_display = img.clone();
	/// Create the result matrix
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);
	/// Do the Matching and Normalize
	match_method = CV_TM_SQDIFF;
	matchTemplate(img, templ, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}
	/// Show me what you got
	rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);   //��rect
	//rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	//imshow(image_window, img_display);
	//imshow(result_window, result);
	Rect rect(matchLoc.x,matchLoc.y,templ.cols, templ.rows);     //�õ�����  
	ImageCrop(img_display, img, rect);

	namedWindow("final Image");
	imshow("final Image", img);


	char ImageName[50];
	sprintf(ImageName, "%s%02d%s", "result", i, ".bmp"); //�����ͼƬ��

	imwrite(ImageName, img);
	
	return;
}

int processImage(Mat src, Mat templ, int i)
{
	const int threshval = 35;
	Mat src_gray, src_th, erosion_dst;
	Mat dst, dst_gray, dst_th;
	Mat result;

	const Scalar RED(0, 0, 255), GREEN(0, 255, 0);
	int erosion_elem = 0;
	int erosion_size = 15;

	RNG &rng = theRNG();
	Rect rect;

	/*int blurSize = 5;
	GaussianBlur(src, src, Size(blurSize, blurSize), 0, 0, BORDER_DEFAULT);*/
img_process:
	cvtColor(src, src_gray, CV_BGR2GRAY);
	threshold(src_gray, src_th, threshval, 250, THRESH_BINARY_INV);

	erode(src_th, src_th, Mat());
	dilate(src_th, src_th, Mat());

	vector<vector<Point> > contours;
	vector<Vec4i>hierarchy;
	//Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC3);
	findContours(src_th, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	//��ȥ̫������̫�̵�����  
	int cmin = 50;
	int cmax = 120;
	std::vector<std::vector<Point>>::const_iterator itc = contours.begin();
	while (itc != contours.end())
	{
		if (itc->size() < cmin || itc->size() > cmax)
			itc = contours.erase(itc);
		else
			++itc;
	}

	/*if (contours.empty())
	{
	string msg = "find the date area";
	int baseLine = 0;
	Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
	Point textOrigin(src.cols - 2 * textSize.width - 10, src.rows - 2 * baseLine - 10);
	putText(src, "no date display", textOrigin, 1, 1, RED);

	}*/

	///�ж�����
	if (contours.size() == 0)
	{
		cout << "There is no date";
		return -1;
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

	vector<vector<Point> > _contours;
	Point2f rect_points[4];
	vector<Vec4i>_hierarchy;
	//Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC3);
	findContours(erosion_dst, _contours, _hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	imshow("erosion_dst", erosion_dst);
	/// ������� test
	/*for (int i = 0; i < _contours.size(); i++)
	{
	drawContours(src, _contours, i, Scalar(0,0,255), 1, 8, vector<Vec4i>(), 0, Point());
	}*/

	//��ȥ̫������̫�̵�����  
	int min = 150;
	int max = 500;
	std::vector<std::vector<Point>>::const_iterator itd = _contours.begin();
	while (itd != _contours.end())
	{
		if (itd->size() < min || itd->size() > max)
			itd = _contours.erase(itd);
		else
			++itd;
	}

	for (int i = 0; i < _contours.size(); i++)
	{
		drawContours(erosion_dst, _contours, i, Scalar(0, 0, 255), 1, 8, vector<Vec4i>(), 0, Point());
	}

	//��ֹԽ��
	if (_contours.empty())
	{
		cout << "failed";
		imageRotate(src, src, -30, false);
		goto img_process;
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
	//namedWindow("My hole");
	//imshow("My hole", hole);
	Mat crop(src.rows, src.cols, CV_8UC3);
	src.copyTo(crop, hole);//��ԭͼ�񿽱�������ͼ��  
	namedWindow("My warpPerspective");
	imshow("My warpPerspective", crop);

	//����ͼ��
	double recWidth = GetDistance(rect_points[0], rect_points[1]);
	double recHeight = GetDistance(rect_points[2], rect_points[1]);
	float dx, dy;

	if (recWidth > recHeight)
	{
		dx = rect_points[0].x - rect_points[1].x;
		dy = rect_points[0].y - rect_points[1].y;
	}

	else
	{
		dx = rect_points[2].x - rect_points[1].x;
		dy = rect_points[2].y - rect_points[1].y;
	}

	float rotateDegree = atan2f(dy, dx);   //(float)CV_PI*180.0f;
	double angle = rotateDegree * 180 / CV_PI; // ����  

	bool isClip = false;
	imageRotate(src, result, -angle, isClip);  //angleΪ+
	namedWindow("ROI Image");
	imshow("ROI Image", result);
	MatchingMethod(result, templ, i);

	return 0;
}

/*----------------------------
* ���� : ������������
*----------------------------
* ���� : GetDistance
* ���� : public
* ���� : ��������
*
* ���� : pointAΪ��һ����
* ���� : pointBΪ�ڶ�����
*/
double GetDistance(Point2f pointA, Point2f pointB)
{
	double distance;
	distance = powf((pointA.x - pointB.x), 2) + powf((pointA.y - pointB.y), 2);
	distance = sqrtf(distance);

	return distance;
}

/*----------------------------
* ���� : ͼ����ת
*----------------------------
* ���� : imageRotate
* ���� : public
* ���� : ��ת���ͼ��
*
* ���� : srcΪԭͼ��
* ���� : dstΪ��ͼ��
* ���� : angleΪ��ת�Ƕ�
* ���� : isClip��ʾ�ǲ�ȡ��СͼƬ�ķ�ʽ
*/
int imageRotate(InputArray src, OutputArray dst, double angle, bool isClip)
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
		new_width = cvRound(out_width - X * 2) ;
		new_height = cvRound(out_height - Y * 2) ;
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
	const int num = 16;
	char fileName[50];
	for (int i = 0; i <= num; i++)
	{
		Mat templ = imread("templ.jpg");
		sprintf_s(fileName, "%02d.tiff", i + 1);
		Mat original = imread(fileName);
		if (original.empty())
		{
			cout << "filename invalid: " << fileName << endl;
			return -1;
		}
		Mat src = original.clone();
		imageRotate(src, src, -10, false);  //����ƫת�Ƕȣ���ֹԽ��
		processImage(src, templ, i + 1);

	}

	return 0;
}

