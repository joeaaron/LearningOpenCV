#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <cmath>

using namespace std;
using namespace cv;


#define SHOW_LINE

#define BASE 100

static string source_window = "source";
static string window_name = "image rotate";
static Mat src;
static int rotateDegree = 0 + BASE;
static int clip = 0;

//ͼ����ת: srcΪԭͼ�� dstΪ��ͼ��, angleΪ��ת�Ƕ�(��ֵΪ˳ʱ����ת,��ֵΪ��ʱ����ת)
int imageRotate1(InputArray src, OutputArray dst, double angle)
{
	Mat input = src.getMat();
	if( input.empty() ) {
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
	Mat trans_mat = getRotationMatrix2D( center, -angle, scale );

	//����任
	warpAffine( input, dst, trans_mat, Size(width, height));

	return 0;
}

//ͼ����ת: srcΪԭͼ�� dstΪ��ͼ��, angleΪ��ת�Ƕ�
int imageRotate2(InputArray src, OutputArray dst, double angle)
{
	Mat input = src.getMat();
	if( input.empty() ) {
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
	Mat trans_mat = getRotationMatrix2D( center, -angle, scale );

	//������ͼ���С
	double angle1 = angle  * CV_PI / 180. ;
	double a = sin(angle1) * scale;
	double b = cos(angle1) * scale;
	double out_width = height * fabs(a) + width * fabs(b);
	double out_height = width * fabs(a) + height * fabs(b);

	//����任
	warpAffine( input, dst, trans_mat, Size(out_width, out_height));

	return 0;
}

//ͼ����ת: srcΪԭͼ�� dstΪ��ͼ��, angleΪ��ת�Ƕ�
int imageRotate3(InputArray src, OutputArray dst, double angle)
{
	Mat input = src.getMat();
	if( input.empty() ) {
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
	Mat trans_mat = getRotationMatrix2D( center, -angle, scale );

	//������ͼ���С
	double angle1 = angle  * CV_PI / 180. ;
	double a = sin(angle1) * scale;
	double b = cos(angle1) * scale;
	double out_width = height * fabs(a) + width * fabs(b);
	double out_height = width * fabs(a) + height * fabs(b);

	//����ת�任�����м���ƽ����
	trans_mat.at<double>(0, 2) += cvRound( (out_width - width) / 2 );
	trans_mat.at<double>(1, 2) += cvRound( (out_height - height) / 2);

	//����任
	warpAffine( input, dst, trans_mat, Size(out_width, out_height));

	return 0;
}


//ͼ����ת: srcΪԭͼ�� dstΪ��ͼ��, angleΪ��ת�Ƕ�, isClip��ʾ�ǲ�ȡ��СͼƬ�ķ�ʽ
int imageRotate4(InputArray src, OutputArray dst, double angle, bool isClip)
{
	Mat input = src.getMat();
	if( input.empty() ) {
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
	Mat trans_mat = getRotationMatrix2D( center, -angle, scale );

	//������ͼ���С
	double angle1 = angle  * CV_PI / 180. ;
	double a = sin(angle1) * scale;
	double b = cos(angle1) * scale;
	double out_width = height * fabs(a) + width * fabs(b); //��߿򳤶�
	double out_height = width * fabs(a) + height * fabs(b);//��߿�߶�

	int new_width, new_height;
	if ( ! isClip ) {
		new_width = cvRound(out_width);
		new_height = cvRound(out_height);
	} else {
		//calculate width and height of clip rect
		double angle2 = fabs(atan(height * 1.0 / width)); //���Ƕ� b
		double len = width * fabs(b);
		double Y = len / ( 1 / fabs(tan(angle1)) + 1 / fabs(tan(angle2)) );
		double X = Y * 1 / fabs(tan(angle2));
		new_width = cvRound(out_width - X * 2);
		new_height= cvRound(out_height - Y * 2);
	}

	//����ת�任�����м���ƽ����
	trans_mat.at<double>(0, 2) += cvRound( (new_width - width) / 2 );
	trans_mat.at<double>(1, 2) += cvRound( (new_height - height) / 2);

	//����任
	warpAffine( input, dst, trans_mat, Size(new_width, new_height));

	return 0;
}

/**
 * ���ͼ����б��
 * ����ֵ������0��ʾ�޼���������ط�0��ʾ����ͼ����Ҫ��ת�ĽǶȣ�-10��10�ȣ�
 */
double detectRotation(InputArray src)
{
	double max_angle = 6; //����ת�����Ƕ�

	Mat in = src.getMat();
	if( in.empty() ) return 0;

	Mat input;

	//תΪ�Ҷ�ͼ
	if ( in.type() == CV_8UC1 )
		input = in;
	else if ( in.type() == CV_8UC3 )
		cvtColor(in, input, CV_BGR2GRAY);
	else if ( in.type() == CV_8UC3 )
		cvtColor(in, input, CV_BGRA2GRAY);
	else
		return 0;

	Mat dst, cdst;

	//ִ��Canny��Ե���(�����Ϊdst, Ϊ�ڰ�ͼ)
	double threshold1 = 90;
	Canny(src, dst, threshold1, threshold1 * 3, 3);

	//��Canny��Ե�����ת��Ϊ�Ҷ�ͼ��(cdst)
	cvtColor(dst, cdst, CV_GRAY2BGR);

	//ִ�л����߱任�����ֱ��
	vector<Vec4i> lines; //��ż������vector
	double minLineLength = std::min(dst.cols, dst.rows) * 0.25; //����߳���
	double maxLineGap = std::min(dst.cols, dst.rows) * 0.03 ; //��С�߼��
	int threshold = 90;
	HoughLinesP(dst, lines, 1, CV_PI / 180, threshold, minLineLength, maxLineGap );

	//�����������
	int x1, y1, x2 , y2; //ֱ�ߵ������˵�
	int x, y;  //ֱ�ߵ��е�
	double angle, rotate_angle; //ֱ�ߵĽǶȣ�����ֱ����Ҫ��ת�ĽǶ�
	double line_length; //ֱ�߳���
	double position_weighted; //ֱ�ߵ�λ��Ȩ�أ���ͼ���������Ȩ��Ϊ1, Խ���ߵ���Ȩ��ԽС
	double main_lens[2]; //���ڴ����Ķ���ֱ�߳��ȵ����� (������ֱ�߼���������)
	double main_angles[2];//���ڴ����Ķ���ֱ�ߵİ�����Ҫ��ת�ĽǶ�
	main_lens[0] = main_lens[1] = 0;
	main_angles[0] = main_angles[1] = 0;

	//�����������ֱ�ߣ��ж��ĸ���������
	for( size_t i = 0; i < lines.size(); i++ ) {
		//ȡ��ֱ�ߵ������˵�����
		x1 = lines[i][0]; y1 = lines[i][1]; x2 = lines[i][2]; y2 = lines[i][3];
		x = (x1 + x2 ) / 2; y = (y1 + y2) / 2;
		//����ֱ�ߵĽǶ�
		angle =	(x1 == x2) ? 90 : ( atan ( (y1 - y2) * 1.0 / (x2 - x1) ) ) / CV_PI * 180;
		//����ֱ����Ҫ��ת�ĽǶ�. �����������ת�����Ƕ�,���������ߡ�
		if ( fabs(angle - 0) <= max_angle ) {
			rotate_angle = angle - 0;
		} else if ( fabs(angle - 90) <= max_angle ) {
			rotate_angle = angle - 90;
		} else {
			continue;
		}

		//�����ߵĳ���
		line_length = sqrt( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)  );
		//����ֱ�ߵ�λ��Ȩ�أ���ͼ���������Ȩ��Ϊ1, Խ���ߵ���Ȩ��ԽС
		position_weighted = 1;
		if ( x < dst.cols / 4 || x > dst.cols * 3 / 4  ) position_weighted *= 0.8;
		if ( x < dst.cols / 6 || x > dst.cols * 5 / 6  ) position_weighted *= 0.5;
		if ( x < dst.cols / 8 || x > dst.cols * 7 / 8  ) position_weighted *= 0.5;
		if ( y < dst.rows / 4 || y > dst.rows * 3 / 4  ) position_weighted *= 0.8;
		if ( y < dst.rows / 6 || y > dst.rows * 5 / 6  ) position_weighted *= 0.5;
		if ( y < dst.rows / 8 || y > dst.rows * 7 / 8  ) position_weighted *= 0.5;

		//��� ֱ�߳��� * λ��Ȩ�� < ��С���ȣ� ����������Ч
		line_length = line_length * position_weighted;
		if ( line_length < minLineLength ) continue;



		//�������Ϊǰ���������������
		if ( line_length > main_lens[1] )  {
			if (line_length > main_lens[0]) {
				 main_lens[1] = main_lens[0];
				 main_lens[0] = line_length;
				 main_angles[1] = main_angles[0];
				 main_angles[0] = rotate_angle;
				 //��������� SHOW_LINE, �򽫸�����������
				 #ifdef SHOW_LINE
				 line( cdst, Point(x1, y1), Point(x2, y2), Scalar(0,0,255), 3, CV_AA);
				 #endif
			} else {
				main_lens[1] = line_length;
				main_angles[1] = rotate_angle;
			}
		}
	}

	//��������� SHOW_LINE, ����source_window����ʾcdst
    #ifdef SHOW_LINE
	imshow(source_window, cdst);
	#endif

	//��󣬷�����Ķ���ֱ�ߣ��ó����
	if ( main_lens[0] > 0 ) {
		//�������� �� �γ����� ���߳���������򷵻�������Ҫ��ת�ĽǶȵ�ƽ��ֵ
		if (main_lens[1] > 0 && (main_lens[0] - main_lens[1] / main_lens[0] < 0.2 )) {
			return (main_angles[0] + main_angles[1] ) / 2;
		} else {
			return main_angles[0];   //���򣬷����������Ҫ��ת�ĽǶ�
		}
	} else {
		return 0;
	}
}


static void callbackAdjust(int , void *)
{
	Mat dst;

	//imageRotate1(src, dst, rotateDegree - BASE);
	//imageRotate2(src, dst, rotateDegree - BASE);
	//imageRotate3(src, dst, rotateDegree - BASE);

	bool isClip = ( clip == 1 );
	imageRotate4(src, dst, rotateDegree - BASE,  isClip );

	imshow(window_name, dst);
}


int main()
{
	src = imread("lena.jpg");

	if ( !src.data ) {
		cout << "error read image" << endl;
		return -1;
	}

	namedWindow(source_window);
	imshow(source_window, src);

	namedWindow(window_name);
	createTrackbar("rotate", window_name, &rotateDegree, BASE * 4, callbackAdjust);
	createTrackbar("clip", window_name, &clip, 1, callbackAdjust);

	//�Զ������ת�Ƕ�
	double angle = detectRotation(src);
	if ( angle != 0 ) {
		rotateDegree = angle + BASE;
		setTrackbarPos("rotate", window_name, rotateDegree);
	}

	callbackAdjust(0, 0);

	waitKey();

        return 0;

}