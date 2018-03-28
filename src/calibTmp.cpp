#include<opencv2/opencv.hpp>
#include<iostream>
#include<math.h>
#include<highgui.h>

using namespace std;
using namespace cv;
char Filename[30];
int n_boards = 0;
const int board_dt = 10;
int board_w;
int board_h;
IplImage*image;
int main(int argc, char*argv[])
{
	board_w = 8;
	board_h = 8;
	n_boards = 10;   //图片个数
	int board_n = board_w*board_h;
	CvSize board_sz = cvSize(board_w, board_h);
	cvNamedWindow("Calibration", CV_WINDOW_AUTOSIZE);

	CvMat*image_points = cvCreateMat(n_boards*board_n, 2, CV_32FC1);
	CvMat*object_points = cvCreateMat(n_boards*board_n, 3, CV_32FC1);
	CvMat*point_counts = cvCreateMat(n_boards, 1, CV_32FC1);
	CvMat*intrinsic_matrix = cvCreateMat(3, 3, CV_32FC1);
	CvMat*distortion_coeffs = cvCreateMat(5, 1, CV_32FC1);

	CvPoint2D32f*corners = new CvPoint2D32f[board_n];
	int corner_count;
	int successes = 0;
	int step, frame = 0;

	for (int a = 1; a <= n_boards; a++)
	{
		sprintf(Filename, "G:/code/LearningOpenCV/data/calib/imagelist/%d.bmp", a);// 将图片以数字命名：例如1.jpg 2.jpg等，放入D:/test/文件夹下
		
		image = cvLoadImage(Filename, 1);//读入一张图	
		IplImage*gray_image = cvCreateImage(cvGetSize(image), 8, 1);

		int found = cvFindChessboardCorners(image, board_sz, corners, &corner_count,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

		if (successes = 0)
		{
			cout << "第" << a << "张图片没有检测到棋盘格所有的角点" << endl;
			cvShowImage("Calibration", image);
			waitKey(0);
		}
		else
		{
			cout << "第" << a << "张图片检测到" << corner_count << "个角点" << endl;
			//Get Subpixel accuracy on those corners
			cvCvtColor(image, gray_image, CV_BGR2GRAY);
			cvFindCornerSubPix(gray_image, corners, corner_count, cvSize(11, 11), cvSize(-1, -1),
				cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));


			//Draw it
			cvDrawChessboardCorners(image, board_sz, corners, corner_count, found);
			cvShowImage("Calibration", image);
			waitKey(0);
		}
		//If we got a good board,add it to our data
		if (corner_count == board_n)
		{
			step = successes*board_n;
			for (int i = step, j = 0; j < corner_count; ++i, ++j)
			{
				CV_MAT_ELEM(*image_points, float, i, 0) = corners[j].x;
				CV_MAT_ELEM(*image_points, float, i, 1) = corners[j].y;
				CV_MAT_ELEM(*object_points, float, i, 0) = j / board_w;
				CV_MAT_ELEM(*object_points, float, i, 1) = j%board_w;
				CV_MAT_ELEM(*object_points, float, i, 2) = 0.0f;
			}
			CV_MAT_ELEM(*point_counts, int, successes, 0) = board_n;
			successes++;
		}
	}

		//Allocate matrices according to how many chessboards found
		CvMat*object_points2 = cvCreateMat(successes*board_n, 3, CV_32FC1);
		CvMat*image_points2 = cvCreateMat(successes*board_n, 2, CV_32FC1);
		CvMat*point_counts2 = cvCreateMat(successes, 1, CV_32SC1);

		for (int i = 1; i < successes*board_n; ++i) 
		{
			CV_MAT_ELEM(*image_points2, float, i, 0) = CV_MAT_ELEM(*image_points, float, i, 0);
			CV_MAT_ELEM(*image_points2, float, i, 1) = CV_MAT_ELEM(*image_points, float, i, 1);
			CV_MAT_ELEM(*object_points2, float, i, 0) = CV_MAT_ELEM(*object_points, float, i, 0);
			CV_MAT_ELEM(*object_points2, float, i, 0) = CV_MAT_ELEM(*object_points, float, i, 1);
			CV_MAT_ELEM(*object_points2, float, i, 0) = CV_MAT_ELEM(*object_points, float, i, 2);
		}
		for (int i = 0; i < successes; ++i)
		{
			CV_MAT_ELEM(*point_counts2, int, i, 0) = CV_MAT_ELEM(*point_counts, int, i, 0);
		}
		cvReleaseMat(&object_points);
		cvReleaseMat(&image_points);
		cvReleaseMat(&point_counts);

		CV_MAT_ELEM(*intrinsic_matrix, float, 0, 0) = 1.0f;
		CV_MAT_ELEM(*intrinsic_matrix, float, 1, 1) = 1.0f;

		//Calibrate the camera!
		cvCalibrateCamera2(object_points2, image_points2, point_counts2, cvGetSize(image),
			intrinsic_matrix, distortion_coeffs, NULL, NULL, 0 //CV_CALIB_FIX_ASPECT_RATIO(如果设置这个，则优化过程中只同时改变fx和fy)
		);

		//Save the intrinsics and distortons
		cvSave("H:\\imagelist\\output\\Intrinsics.xml", intrinsic_matrix);
		cvSave("H:\\imagelist\\output\\Distortion.xml", distortion_coeffs);
		//Example of loading these matrices back in:
		CvMat*intrinsic = (CvMat*)cvLoad("H:\\imagelist\\output\\Intrinsics.xml");
		CvMat*distortion = (CvMat*)cvLoad("H:\\imagelist\\output\\Distortion.xml");

		IplImage*mapx = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F, 1);
		IplImage*mapy = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F, 1);
		cvInitUndistortMap(intrinsic, distortion, mapx, mapy
		);
		cvNamedWindow("Undistort");
		while (image) {
			IplImage*t = cvCloneImage(image);
			cvShowImage("Calibration", image);  //Show raw images
			cvRemap(t, image, mapx, mapy);      //Undistort image
			cvReleaseImage(&t);
			cvShowImage("Undistort", image);    //Show corrected image
		}
	return 0;
}