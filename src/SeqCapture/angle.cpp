#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include "angle.h"
using namespace cv;
using namespace std;

const double SQUARESIZE = 1.75;
const int BOARDSIZEWIDTH = 5;
const int BOARDSIZEHEIGHT = 4;
//const float SQUARESIZE = 33.333;
//const int BOARDSIZEWIDTH = 13;
//const int BOARDSIZEHEIGHT = 9;

//Checks if a matrix is a valid rotation matrix
bool isRotationMatrix(Mat& R)
{
	cv::Mat Rt;
	transpose(R, Rt);
	Mat shouldBeIdentity = Rt * R;
	Mat I = Mat::eye(3, 3, shouldBeIdentity.type());

	return norm(I, shouldBeIdentity) < 1e-6;
}

///pitch angle is the angle we need
void eulerAnglesToOrdinaryAngles(vector<float> rads, vector<float>& angles)
{
	for (vector<float>::iterator it = rads.begin(); it != rads.end(); it++)
	{
		float angle = *it / CV_PI * 180;     //rad to angle
		//cout << angle << endl;
		angles.push_back(angle);
	}
	
}

//Calculates rotation matrix to euler angles
//The results is the same as MATLAB except the order
//of the euler angles(x and z are swapped).
float rotationMatrixToEulerAngles(Mat& R, double& angle)
{
	vector<float> rads;
	vector<float> angles;

	double fps;
	double t = 0;
	assert(isRotationMatrix(R));
	
	float sy = sqrt(R.at<double>(0, 0) * R.at<double>(0, 0) + R.at<double>(1, 0) * R.at<double>(1, 0));
	bool singular = sy < 1e-6;	//If singular is true
	float x, y, z;

	if (!singular)
	{
		x = atan2(R.at<double>(2, 1), R.at<double>(2, 2));
		y = atan2(-R.at<double>(2, 0), sy);
		z = atan2(R.at<double>(1, 0), R.at<double>(0, 0));
	}
	else
	{
		x = atan2(-R.at<double>(1, 2), R.at<double>(1, 1));
		y = atan2(-R.at<double>(2, 0), sy);
		z = 0;
	}
	
	rads.push_back(x);
	rads.push_back(y);
	rads.push_back(z);
	eulerAnglesToOrdinaryAngles(rads, angles);

	float pitch = angles[0];        //ROTATIONAXIS:X
	float roll = angles[1];			//ROTATIONAXIS:Y
	float yaw = angles[2];			//ROTATIONAXIS:Z

	cout << yaw << "\t" << roll << "\t" << pitch << endl;
	t = ((double)cv::getTickCount() - 1) / cv::getTickFrequency();
	fps = 1.0 / t;

	angle = pitch;
	return angle;
}

class angle_t: public AngleProc
{
private:
	cv::Mat cameraMatrix;
	cv::Mat distcofficients;

	VideoCapture* capture;

	vector<Point3f> corners;
public:
	angle_t(): capture(0)
	{
		/* generate the expected corners */
		corners.clear();

		for (int i = 0; i < BOARDSIZEHEIGHT; ++i)
			for (int j = 0; j < BOARDSIZEWIDTH; ++j)
				corners.push_back(Point3f(j*SQUARESIZE, i*SQUARESIZE, 0));

	}
	int Open(int carmarIdx, const string inputCameraDataFile)
	{
		capture = new VideoCapture(carmarIdx);
		if (!capture->isOpened())     //if the cam is opened
			return -1;

		//! [file_read]
		FileStorage fs(inputCameraDataFile, FileStorage::READ); // Read the settings
		if (!fs.isOpened())
		{
			std::cout << "Could not open the configuration file: \"" << inputCameraDataFile << "\"" << std::endl;
			return -2;
		}

		try{
			fs["camera_matrix"] >> cameraMatrix;
			fs["distortion_coefficients"] >> distcofficients;
		} 
		catch(...)
		{
			std::cout << "configuration file error: \"" << inputCameraDataFile << "\"" << std::endl;
			return -2;
		}
	}

	int Angle(double& angle)
	{
		if(!capture)
			return -1;

		///find image points			
		cv::Mat dst;

		vector<Point2f> pointBuf;
		vector<Point3f> objectPoints;

		cv::Mat frame;	
		if (!capture->read(frame))
			return -1;
		
#if DBEUG
		namedWindow("capVideo");
		imshow("capVideo", frame);
#endif

		//Mat view = cv::imread("x0.bmp");
		Mat& view = frame;
		Mat viewGray;
		cvtColor(view, viewGray, COLOR_BGR2GRAY);

		int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FILTER_QUADS | CALIB_CB_FAST_CHECK;
		Size boardSize = Size(BOARDSIZEWIDTH, BOARDSIZEHEIGHT);
		findChessboardCorners(view, boardSize, pointBuf, chessBoardFlags);   //pointBuf size : 117 = 13 * 9;
		cornerSubPix(viewGray, pointBuf, Size(11, 11),
			Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
		//sortConnerPoints(pointBuf);        imagePoints counter to objectPoints


		///compute rotation vector
		Mat rvecs, tvecs;
		solvePnP(this->corners, pointBuf, cameraMatrix, distcofficients, rvecs, tvecs, false, CV_ITERATIVE);

		///rotation vector to rotation matrix
		cv::Mat R;
		cv::Rodrigues(rvecs, R);
		//R = getRotationMatix(cv::Mat(cv::Vec3d(0, 0, 1)), rvecs);         //2018-01-19 some problems needed to be verified later
		rotationMatrixToEulerAngles(R, angle);
	
		return 0;
	}

	~angle_t()
	{
		capture->release();
		delete capture;
		capture = 0;
	}
	
};

AngleProc* AngleProc::Create()
{
	return new angle_t();
}
