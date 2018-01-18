#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

cv::Mat cameraMatrix;
cv::Mat distcofficients;
vector<vector<Point2f>> imagePoints;
vector<Point2f> pointBuf;
vector<vector<Point3f> > objectPoints(1);

#define INPUT_ROTATION_ANGLE  (-CV_PI/3)
const float SQUARESIZE = 33.33333;
const int BOARDSIZEWIDTH = 13;
const int BOARDSIZEHEIGHT = 9;

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
cv::Vec3f rotationMatrixToEulerAngles(Mat& R)
{
	vector<float> rads;
	vector<float> angles;
	rads.clear();

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

	float yaw = angles[2];
	float roll = angles[1];
	float pitch = angles[0];

	cout << yaw << endl;

	return Vec3f(x, y, z);
}

//http://blog.csdn.net/wang15061955806/article/details/51028484
Mat RotationMatrix(Mat& axis,
	const double& angleRad)
{
	//compute dimension of matrix
	double length = axis.cols;

	//check dimension of matrix
	if (length < 0)
	{
		//rotation matrix could not be computed because axis vector is not defined
		cout << "SOMETHING WENT WRONG!";
	}

	//normal axis vector 
	double f = 1.0 / length;
	double x = f * axis.at<double>(0);
	double y = f * axis.at<double>(1);
	double z = f * axis.at<double>(2);

	//compute rotation matrix
	double c = ::cos(angleRad);
	double s = ::sin(angleRad);
	double v = 1 - c;

	//3 * 3 null matrix
	Mat m = Mat::zeros(3, 3, CV_64FC1);

	m.at<double>(0, 0) = x*x*v + c;     m.at<double>(0, 1) = x*y*v - z*s;  m.at<double>(0, 2) = x*z*v + y*s;
	m.at<double>(1, 0) = x*y*v + z*s;   m.at<double>(1, 1) = y*y*v + c;    m.at<double>(1, 2) = y*z*v - x*s;
	m.at<double>(2, 0) = x*z*v - y*s;   m.at<double>(2, 1) = y*z*v + x*s;  m.at<double>(2, 2) = z*z*v + c;
	return m;

}


//************************************
// Method:    calcBoardCornerPositions
// FullName:  calcBoardCornerPositions
// Access:    public static 
// Returns:   void
// Qualifier:
// Parameter: float squareSize
// Parameter: vector<Point3f> & corners
//************************************
static void calcBoardCornerPositions(float squareSize, vector<Point3f>& corners)
{
	corners.clear();

	for (int i = 0; i < BOARDSIZEHEIGHT; ++i)
		for (int j = 0; j < BOARDSIZEWIDTH; ++j)
			corners.push_back(Point3f(j*squareSize, i*squareSize, 0));
	
}

bool RunComputeAngle(const string inputCameraDataFile, float angle)
{
	//! [file_read]
	FileStorage fs(inputCameraDataFile, FileStorage::READ); // Read the settings
	if (!fs.isOpened())
	{
		std::cout << "Could not open the configuration file: \"" << inputCameraDataFile << "\"" << std::endl;
		return -1;
	}

	fs["camera_matrix"] >> cameraMatrix;
	fs["distortion_coefficients"] >> distcofficients;

	///find image points
	Mat viewGray;
	Mat view;
	cvtColor(view, viewGray, COLOR_BGR2GRAY);
	cornerSubPix(viewGray, pointBuf, Size(11, 11),
		Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
	//sortConnerPoints(pointBuf);        imagePoints counter to objectPoints
	imagePoints.push_back(pointBuf);

	///find object points
	calcBoardCornerPositions(SQUARESIZE, objectPoints[0]);

	///compute rotation matrix
	Mat rvecs, tvecs;
	solvePnP(objectPoints, imagePoints, cameraMatrix, distcofficients, rvecs, tvecs);
}

int main()
{
	cv::Mat rotationMatrix = RotationMatrix(cv::Mat(cv::Vec3d(0, 0, 1)), INPUT_ROTATION_ANGLE);   
	rotationMatrixToEulerAngles(rotationMatrix);

	float yaw = 0.0;
	float pitch = 0.0;
	float roll = 0.0;

	//float angle = 0.0;
	//RunComputeAngle("out_camera_data.xml", angle);
}