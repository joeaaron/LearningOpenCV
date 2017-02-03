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

//图像旋转: src为原图像， dst为新图像, angle为旋转角度(正值为顺时针旋转,负值为逆时针旋转)
int imageRotate1(InputArray src, OutputArray dst, double angle)
{
	Mat input = src.getMat();
	if( input.empty() ) {
		return -1;
	}

	//得到图像大小
	int width = input.cols;
	int height = input.rows;

	//计算图像中心点
	Point2f center;
	center.x = width / 2.0;
	center.y = height / 2.0;

	//获得旋转变换矩阵
	double scale = 1.0;
	Mat trans_mat = getRotationMatrix2D( center, -angle, scale );

	//仿射变换
	warpAffine( input, dst, trans_mat, Size(width, height));

	return 0;
}

//图像旋转: src为原图像， dst为新图像, angle为旋转角度
int imageRotate2(InputArray src, OutputArray dst, double angle)
{
	Mat input = src.getMat();
	if( input.empty() ) {
		return -1;
	}

	//得到图像大小
	int width = input.cols;
	int height = input.rows;

	//计算图像中心点
	Point2f center;
	center.x = width / 2.0;
	center.y = height / 2.0;

	//获得旋转变换矩阵
	double scale = 1.0;
	Mat trans_mat = getRotationMatrix2D( center, -angle, scale );

	//计算新图像大小
	double angle1 = angle  * CV_PI / 180. ;
	double a = sin(angle1) * scale;
	double b = cos(angle1) * scale;
	double out_width = height * fabs(a) + width * fabs(b);
	double out_height = width * fabs(a) + height * fabs(b);

	//仿射变换
	warpAffine( input, dst, trans_mat, Size(out_width, out_height));

	return 0;
}

//图像旋转: src为原图像， dst为新图像, angle为旋转角度
int imageRotate3(InputArray src, OutputArray dst, double angle)
{
	Mat input = src.getMat();
	if( input.empty() ) {
		return -1;
	}

	//得到图像大小
	int width = input.cols;
	int height = input.rows;

	//计算图像中心点
	Point2f center;
	center.x = width / 2.0;
	center.y = height / 2.0;

	//获得旋转变换矩阵
	double scale = 1.0;
	Mat trans_mat = getRotationMatrix2D( center, -angle, scale );

	//计算新图像大小
	double angle1 = angle  * CV_PI / 180. ;
	double a = sin(angle1) * scale;
	double b = cos(angle1) * scale;
	double out_width = height * fabs(a) + width * fabs(b);
	double out_height = width * fabs(a) + height * fabs(b);

	//在旋转变换矩阵中加入平移量
	trans_mat.at<double>(0, 2) += cvRound( (out_width - width) / 2 );
	trans_mat.at<double>(1, 2) += cvRound( (out_height - height) / 2);

	//仿射变换
	warpAffine( input, dst, trans_mat, Size(out_width, out_height));

	return 0;
}


//图像旋转: src为原图像， dst为新图像, angle为旋转角度, isClip表示是采取缩小图片的方式
int imageRotate4(InputArray src, OutputArray dst, double angle, bool isClip)
{
	Mat input = src.getMat();
	if( input.empty() ) {
		return -1;
	}

	//得到图像大小
	int width = input.cols;
	int height = input.rows;

	//计算图像中心点
	Point2f center;
	center.x = width / 2.0;
	center.y = height / 2.0;

	//获得旋转变换矩阵
	double scale = 1.0;
	Mat trans_mat = getRotationMatrix2D( center, -angle, scale );

	//计算新图像大小
	double angle1 = angle  * CV_PI / 180. ;
	double a = sin(angle1) * scale;
	double b = cos(angle1) * scale;
	double out_width = height * fabs(a) + width * fabs(b); //外边框长度
	double out_height = width * fabs(a) + height * fabs(b);//外边框高度

	int new_width, new_height;
	if ( ! isClip ) {
		new_width = cvRound(out_width);
		new_height = cvRound(out_height);
	} else {
		//calculate width and height of clip rect
		double angle2 = fabs(atan(height * 1.0 / width)); //即角度 b
		double len = width * fabs(b);
		double Y = len / ( 1 / fabs(tan(angle1)) + 1 / fabs(tan(angle2)) );
		double X = Y * 1 / fabs(tan(angle2));
		new_width = cvRound(out_width - X * 2);
		new_height= cvRound(out_height - Y * 2);
	}

	//在旋转变换矩阵中加入平移量
	trans_mat.at<double>(0, 2) += cvRound( (new_width - width) / 2 );
	trans_mat.at<double>(1, 2) += cvRound( (new_height - height) / 2);

	//仿射变换
	warpAffine( input, dst, trans_mat, Size(new_width, new_height));

	return 0;
}

/**
 * 检测图像倾斜度
 * 返回值：返回0表示无检测结果，返回非0表示摆正图象需要旋转的角度（-10至10度）
 */
double detectRotation(InputArray src)
{
	double max_angle = 6; //可旋转的最大角度

	Mat in = src.getMat();
	if( in.empty() ) return 0;

	Mat input;

	//转为灰度图
	if ( in.type() == CV_8UC1 )
		input = in;
	else if ( in.type() == CV_8UC3 )
		cvtColor(in, input, CV_BGR2GRAY);
	else if ( in.type() == CV_8UC3 )
		cvtColor(in, input, CV_BGRA2GRAY);
	else
		return 0;

	Mat dst, cdst;

	//执行Canny边缘检测(检测结果为dst, 为黑白图)
	double threshold1 = 90;
	Canny(src, dst, threshold1, threshold1 * 3, 3);

	//将Canny边缘检测结果转化为灰度图像(cdst)
	cvtColor(dst, cdst, CV_GRAY2BGR);

	//执行霍夫线变换，检测直线
	vector<Vec4i> lines; //存放检测结果的vector
	double minLineLength = std::min(dst.cols, dst.rows) * 0.25; //最短线长度
	double maxLineGap = std::min(dst.cols, dst.rows) * 0.03 ; //最小线间距
	int threshold = 90;
	HoughLinesP(dst, lines, 1, CV_PI / 180, threshold, minLineLength, maxLineGap );

	//分析所需变量
	int x1, y1, x2 , y2; //直线的两个端点
	int x, y;  //直线的中点
	double angle, rotate_angle; //直线的角度，摆正直线需要旋转的角度
	double line_length; //直线长度
	double position_weighted; //直线的位置权重：靠图像中央的线权重为1, 越靠边的线权重越小
	double main_lens[2]; //用于存放最长的二条直线长度的数组 (这两条直线即是主线条)
	double main_angles[2];//用于存放最长的二条直线的摆正需要旋转的角度
	main_lens[0] = main_lens[1] = 0;
	main_angles[0] = main_angles[1] = 0;

	//逐个分析各条直线，判断哪个是主线条
	for( size_t i = 0; i < lines.size(); i++ ) {
		//取得直线的两个端点座标
		x1 = lines[i][0]; y1 = lines[i][1]; x2 = lines[i][2]; y2 = lines[i][3];
		x = (x1 + x2 ) / 2; y = (y1 + y2) / 2;
		//计算直线的角度
		angle =	(x1 == x2) ? 90 : ( atan ( (y1 - y2) * 1.0 / (x2 - x1) ) ) / CV_PI * 180;
		//摆正直线需要旋转的角度. 如果超出可旋转的最大角度,则忽略这个线。
		if ( fabs(angle - 0) <= max_angle ) {
			rotate_angle = angle - 0;
		} else if ( fabs(angle - 90) <= max_angle ) {
			rotate_angle = angle - 90;
		} else {
			continue;
		}

		//计算线的长度
		line_length = sqrt( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)  );
		//计算直线的位置权重：靠图像中央的线权重为1, 越靠边的线权重越小
		position_weighted = 1;
		if ( x < dst.cols / 4 || x > dst.cols * 3 / 4  ) position_weighted *= 0.8;
		if ( x < dst.cols / 6 || x > dst.cols * 5 / 6  ) position_weighted *= 0.5;
		if ( x < dst.cols / 8 || x > dst.cols * 7 / 8  ) position_weighted *= 0.5;
		if ( y < dst.rows / 4 || y > dst.rows * 3 / 4  ) position_weighted *= 0.8;
		if ( y < dst.rows / 6 || y > dst.rows * 5 / 6  ) position_weighted *= 0.5;
		if ( y < dst.rows / 8 || y > dst.rows * 7 / 8  ) position_weighted *= 0.5;

		//如果 直线长度 * 位置权重 < 最小长度， 则这条线无效
		line_length = line_length * position_weighted;
		if ( line_length < minLineLength ) continue;



		//如果长度为前两名，则存入数据
		if ( line_length > main_lens[1] )  {
			if (line_length > main_lens[0]) {
				 main_lens[1] = main_lens[0];
				 main_lens[0] = line_length;
				 main_angles[1] = main_angles[0];
				 main_angles[0] = rotate_angle;
				 //如果定义了 SHOW_LINE, 则将该线条画出来
				 #ifdef SHOW_LINE
				 line( cdst, Point(x1, y1), Point(x2, y2), Scalar(0,0,255), 3, CV_AA);
				 #endif
			} else {
				main_lens[1] = line_length;
				main_angles[1] = rotate_angle;
			}
		}
	}

	//如果定义了 SHOW_LINE, 则在source_window中显示cdst
    #ifdef SHOW_LINE
	imshow(source_window, cdst);
	#endif

	//最后，分析最长的二条直线，得出结果
	if ( main_lens[0] > 0 ) {
		//如果最长的线 与 次长的线 两者长度相近，则返回两者需要旋转的角度的平均值
		if (main_lens[1] > 0 && (main_lens[0] - main_lens[1] / main_lens[0] < 0.2 )) {
			return (main_angles[0] + main_angles[1] ) / 2;
		} else {
			return main_angles[0];   //否则，返回最长的线需要旋转的角度
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

	//自动检测旋转角度
	double angle = detectRotation(src);
	if ( angle != 0 ) {
		rotateDegree = angle + BASE;
		setTrackbarPos("rotate", window_name, rotateDegree);
	}

	callbackAdjust(0, 0);

	waitKey();

        return 0;

}