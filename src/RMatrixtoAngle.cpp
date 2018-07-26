#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

#define INPUT_ROTATION_ANGLE  (-CV_PI/3)
#define TEST 0          //0:开启测试； 1：运行模式

#if TEST
const float SQUARESIZE = 1.75;
const int BOARDSIZEWIDTH = 5;
const int BOARDSIZEHEIGHT = 4;
#else
const float SQUARESIZE = 33.333;
const int BOARDSIZEWIDTH = 13;
const int BOARDSIZEHEIGHT = 9;
#endif

char filename[100];			

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
float rotationMatrixToEulerAngles(Mat& R, float& angle)
{
	vector<float> rads;
	vector<float> angles;

	assert(isRotationMatrix(R));
	
	float sy = sqrt(R.at<double>(0, 0) * R.at<double>(0, 0) + R.at<double>(1, 0) * R.at<double>(1, 0));
	bool singular = sy < 1e-6;	//If singular is true
	float x, y, z;
	float pitch, roll, yaw;

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
	
	roll = y / CV_PI * 180;     //rad to angle
	/*rads.push_back(x);
	rads.push_back(y);
	rads.push_back(z);*/

	//eulerAnglesToOrdinaryAngles(rads, angles);

	//pitch = angles[0];        //ROTATIONAXIS:X
	//roll = angles[1];			//ROTATIONAXIS:Y
	//yaw = angles[2];			//ROTATIONAXIS:Z

	//cout << yaw << "\t" << roll << "\t" << pitch << "\t" << fps<< endl;
	cout << roll << endl;

	angle = roll;
	return angle;
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
// http://www.cnblogs.com/xpvincent/archive/2013/02/15/2912836.html
cv::Mat getRotationMatix(cv::Mat originVec, cv::Mat expectedVec)
{
	cv::Mat rotationAxis = originVec.cross(expectedVec);
	rotationAxis = rotationAxis / cv::norm(rotationAxis);

	double rotationAngle = std::acos(originVec.dot(expectedVec) / cv::norm(originVec) / cv::norm(expectedVec));

	cv::Mat omega(3, 3, CV_64FC1, Scalar(0));
	omega.at<double>(0, 1) = -rotationAxis.at<double>(2);
	omega.at<double>(0, 2) = rotationAxis.at<double>(1);
	omega.at<double>(1, 0) = rotationAxis.at<double>(2);
	omega.at<double>(1, 2) = -rotationAxis.at<double>(0);
	omega.at<double>(2, 0) = -rotationAxis.at<double>(1);
	omega.at<double>(2, 1) = rotationAxis.at<double>(0);
	cv::Mat rotationMatrix = cv::Mat::eye(3, 3, CV_64FC1) + std::sin(rotationAngle)*omega + (1 - std::cos(rotationAngle))*omega*omega;

	cv::Mat normedOri = originVec / cv::norm(originVec);
	cv::Mat normedExp = expectedVec / cv::norm(expectedVec);

	double BP = cv::norm(rotationMatrix*normedOri - normedExp);

	//std::cout << "BP error of getTransformationMatrix of two vector is: " << BP << std::endl;
	return rotationMatrix;
}

void sortConnerPoints(std::vector<cv::Point2f>& corners)
{
	if (corners[0].y > corners[corners.size() - 1].y)
	{
		for (int i = 0; i < corners.size() / 2; i++)
		{
			cv::Point2f tmp = corners[i];
			corners[i] = corners[corners.size() - 1 - i];
			corners[corners.size() - 1 - i] = tmp;
		}
	}
}

template<typename _Tp>
vector<_Tp> convertMat2Vector(const Mat& mat)
{
	return (vector<_Tp>)(mat.reshape(1, 1));
}

float RunComputeAngle(const string inputCameraDataFile, float& angle)
{
	cv::Mat cameraMatrix;
	cv::Mat distcofficients;
	//! [file_read]
	FileStorage fs(inputCameraDataFile, FileStorage::READ); // Read the settings
	if (!fs.isOpened())
	{
		std::cout << "Could not open the configuration file: \"" << inputCameraDataFile << "\"" << std::endl;
		return -1;
	}
	fs["camera_matrix"] >> cameraMatrix;
	fs["distortion_coefficients"] >> distcofficients;

	///find object points
	vector<Point3f> objectPoints;
	calcBoardCornerPositions(SQUARESIZE, objectPoints);


	///find image points
#if TEST
	VideoCapture capture(0);
	if (!capture.isOpened())     //if the cam is opened
		return 1;
	double rate = capture.get(CV_CAP_PROP_FPS);
	int frameHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);  // the height of video frame
	int frameWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);    // the width of video frame
	cv::Mat frame;

	int delay = 1000 / rate;
#endif
	bool stop(false);
	//namedWindow("capVideo");

	int i = 1;
	while (!stop)
	{
		clock_t start, end;
		start = clock();

		vector<Point2f> pointBuf;
		
#if TEST
		if (!capture.read(frame))
			break;
		
		imshow("capVideo", frame);
		Mat view = frame;
#else
		Mat view = cv::imread("x0.bmp");
#endif
		Mat viewGray;
		cvtColor(view, viewGray, COLOR_BGR2GRAY);

		int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
		Size boardSize = Size(BOARDSIZEWIDTH, BOARDSIZEHEIGHT);

		clock_t T1, T2;
		T1 = clock();
		assert(findChessboardCorners(view, boardSize, pointBuf, chessBoardFlags));		//pointBuf size : 117 = 13 * 9;
		if (pointBuf.size() != BOARDSIZEWIDTH * BOARDSIZEHEIGHT)
			continue;
		T2 = clock();
		double dur_1 = (double)(T2 - T1);

		cout << "检测角点时间：" << dur_1 / CLOCKS_PER_SEC << endl;
		/*
		winSize：搜索窗口边长的一半，例如如果winSize = Size(5, 5)，则一个大小为的搜索窗口将被使用。
		zeroZone：搜索区域中间的dead region边长的一半，有时用于避免自相关矩阵的奇异性。如果值设为(-1, -1)则表示没有这个区域。
		criteria：角点精准化迭代过程的终止条件。也就是当迭代次数超过criteria.maxCount，或者角点位置变化小于criteria.epsilon时，停止迭代过程。
		*/
		cornerSubPix(viewGray, pointBuf, Size(5, 5),
			Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
		sortConnerPoints(pointBuf);        

		///compute rotation vector
		Mat rvecs, tvecs;
		clock_t T3, T4;
		T3 = clock();
		solvePnP(objectPoints, pointBuf, cameraMatrix, distcofficients, rvecs, tvecs, false, CV_ITERATIVE);
		T4 = clock();
		double dur_2 = (double)(T4 - T3);
		cout << "求旋转向量时间：" << dur_2 / CLOCKS_PER_SEC << endl;
		///rotation vector to rotation matrix
		//rvecs.convertTo(rvecs, CV_32F);
		cv::Mat R;
		cv::Rodrigues(rvecs, R);
		//R = getRotationMatix(cv::Mat(cv::Vec3d(0, 0, 1)), rvecs);         //2018-01-19 some problems needed to be verified later
		rotationMatrixToEulerAngles(R, angle);
		vector<float> Tvec = convertMat2Vector<float>(tvecs);
		for (vector<float>::iterator it = Tvec.begin(); it != Tvec.end(); it++)
		{
			cout << "tve" << *it;
		}
		vector<Point> tmp;
		
#if TEST
		if (waitKey(delay) >= 0)
			stop = true;

		//按ENTER保存
		if (13 == c)
		{
			sprintf(filename, "%s%d%s", "x", i++, ".bmp");		
			imwrite(filename, frame);
		}

		//Esc键停止
		if (27 == c)
			break;

#else
		char c = cvWaitKey(33);
#endif
		
		end = clock();
		double dur = (double)(end - start);

		cout << "运行时间：" << dur / CLOCKS_PER_SEC << endl;
	}
#if TEST
	capture.release();
#endif
	return angle;
}

int main()
{
	//cv::Mat rotationMatrix = RotationMatrix(cv::Mat(cv::Vec3d(0, 0, 1)), INPUT_ROTATION_ANGLE);   
	//rotationMatrixToEulerAngles(rotationMatrix);

	float angle = 0.0;
	RunComputeAngle("out_camera_data.xml", angle);
	cout << "steering angle = " << angle << endl;
}