#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include "cv.h"  
#include <cv.hpp>  
#include <iostream>  
  
using namespace std;  
using namespace cv;  
 
      
Mat intrinsic;                                          //����ڲ���  
Mat distortion_coeff;                                   //����������  
vector<Mat> rvecs;                                        //��ת����  
vector<Mat> tvecs;                                        //ƽ������  
vector<vector<Point2f>> corners;                        //����ͼ���ҵ��Ľǵ�ļ��� ��objRealPoint һһ��Ӧ  
vector<vector<Point3f>> objRealPoint;                   //����ͼ��Ľǵ��ʵ���������꼯��  
vector<Point2f> corner;                                   //ĳһ��ͼ���ҵ��Ľǵ�  
  
static void help()
{
	cout << "This is a camera calibration sample." << endl
		<< "Usage: calibration configurationFile" << endl
		<< "Near the sample file you'll find the configuration file, which has detailed help of "
		"how to edit it.  It may be any OpenCV supported file format XML/YAML." << endl;
}

cv::Point2f drawCircle(cv::Mat image, Point2f p1, Point2f p2, cv::Point2f& centerPoint);

static class Settings
{
public:
	Settings() : goodInput(false) {}
	enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
	enum InputType { INVALID, CAMERA, VIDEO_FILE, IMAGE_LIST };

	void write(FileStorage& fs) const                        //Write serialization for this class
	{
		fs << "{"
			<< "BoardSize_Width" << boardSize.width
			<< "BoardSize_Height" << boardSize.height
			<< "Square_Size" << squareSize
			<< "Calibrate_Pattern" << patternToUse
			<< "Calibrate_NrOfFrameToUse" << nrFrames
			<< "Calibrate_FixAspectRatio" << aspectRatio
			<< "Calibrate_AssumeZeroTangentialDistortion" << calibZeroTangentDist
			<< "Calibrate_FixPrincipalPointAtTheCenter" << calibFixPrincipalPoint

			<< "Write_DetectedFeaturePoints" << writePoints
			<< "Write_extrinsicParameters" << writeExtrinsics
			<< "Write_outputFileName" << outputFileName

			<< "Show_UndistortedImage" << showUndistorsed

			<< "Input_FlipAroundHorizontalAxis" << flipVertical
			<< "Input_Delay" << delay
			<< "Input" << input
			<< "}";
	}
	void read(const FileNode& node)                          //Read serialization for this class
	{
		node["BoardSize_Width"] >> boardSize.width;
		node["BoardSize_Height"] >> boardSize.height;
		node["Calibrate_Pattern"] >> patternToUse;
		node["Square_Size"] >> squareSize;
		node["Calibrate_NrOfFrameToUse"] >> nrFrames;
		node["Calibrate_FixAspectRatio"] >> aspectRatio;
		node["Write_DetectedFeaturePoints"] >> writePoints;
		node["Write_extrinsicParameters"] >> writeExtrinsics;
		node["Write_outputFileName"] >> outputFileName;
		node["Calibrate_AssumeZeroTangentialDistortion"] >> calibZeroTangentDist;
		node["Calibrate_FixPrincipalPointAtTheCenter"] >> calibFixPrincipalPoint;
		node["Calibrate_UseFisheyeModel"] >> useFisheye;
		node["Input_FlipAroundHorizontalAxis"] >> flipVertical;
		node["Show_UndistortedImage"] >> showUndistorsed;
		node["Input"] >> input;
		node["Input_Delay"] >> delay;
		validate();
	}
	void validate()
	{
		goodInput = true;
		if (boardSize.width <= 0 || boardSize.height <= 0)
		{
			cerr << "Invalid Board size: " << boardSize.width << " " << boardSize.height << endl;
			goodInput = false;
		}
		if (squareSize <= 10e-6)
		{
			cerr << "Invalid square size " << squareSize << endl;
			goodInput = false;
		}
		if (nrFrames <= 0)
		{
			cerr << "Invalid number of frames " << nrFrames << endl;
			goodInput = false;
		}

		if (input.empty())      // Check for valid input
			inputType = INVALID;
		else
		{
			if (input[0] >= '0' && input[0] <= '9')
			{
				stringstream ss(input);
				ss >> cameraID;
				inputType = CAMERA;
			}
			else
			{
				if (readStringList(input, imageList))
				{
					inputType = IMAGE_LIST;
					nrFrames = (nrFrames < (int)imageList.size()) ? nrFrames : (int)imageList.size();
				}
				else
					inputType = VIDEO_FILE;
			}
			if (inputType == CAMERA)
				inputCapture.open(cameraID);
			if (inputType == VIDEO_FILE)
				inputCapture.open(input);
			if (inputType != IMAGE_LIST && !inputCapture.isOpened())
				inputType = INVALID;
		}
		if (inputType == INVALID)
		{
			cerr << " Input does not exist: " << input;
			goodInput = false;
		}

		flag = CALIB_FIX_K4 | CALIB_FIX_K5;
		if (calibFixPrincipalPoint) flag |= CALIB_FIX_PRINCIPAL_POINT;
		if (calibZeroTangentDist)   flag |= CALIB_ZERO_TANGENT_DIST;
		if (aspectRatio)            flag |= CALIB_FIX_ASPECT_RATIO;

		if (useFisheye) {
			// the fisheye model has its own enum, so overwrite the flags
			flag = fisheye::CALIB_FIX_SKEW | fisheye::CALIB_RECOMPUTE_EXTRINSIC |
				// fisheye::CALIB_FIX_K1 |
				fisheye::CALIB_FIX_K2 | fisheye::CALIB_FIX_K3 | fisheye::CALIB_FIX_K4;
		}

		calibrationPattern = NOT_EXISTING;
		if (!patternToUse.compare("CHESSBOARD")) calibrationPattern = CHESSBOARD;
		if (!patternToUse.compare("CIRCLES_GRID")) calibrationPattern = CIRCLES_GRID;
		if (!patternToUse.compare("ASYMMETRIC_CIRCLES_GRID")) calibrationPattern = ASYMMETRIC_CIRCLES_GRID;
		if (calibrationPattern == NOT_EXISTING)
		{
			cerr << " Camera calibration mode does not exist: " << patternToUse << endl;
			goodInput = false;
		}
		atImageList = 0;

	}
	Mat nextImage()
	{
		Mat result;
		if (inputCapture.isOpened())
		{
			Mat view0;
			inputCapture >> view0;
			view0.copyTo(result);
		}
		else if (atImageList < imageList.size())
			result = imread(imageList[atImageList++], IMREAD_COLOR);

		return result;
	}

	static bool readStringList(const string& filename, vector<string>& l)
	{
		l.clear();
		FileStorage fs(filename, FileStorage::READ);
		if (!fs.isOpened())
			return false;
		FileNode n = fs.getFirstTopLevelNode();
		if (n.type() != FileNode::SEQ)
			return false;
		FileNodeIterator it = n.begin(), it_end = n.end();
		for (; it != it_end; ++it)
			l.push_back((string)*it);
		return true;
	}
public:
	Size boardSize;              // The size of the board -> Number of items by width and height
	Pattern calibrationPattern;  // One of the Chessboard, circles, or asymmetric circle pattern
	float squareSize;            // The size of a square in your defined unit (point, millimeter,etc).
	int nrFrames;                // The number of frames to use from the input for calibration
	float aspectRatio;           // The aspect ratio
	int delay;                   // In case of a video input
	bool writePoints;            // Write detected feature points
	bool writeExtrinsics;        // Write extrinsic parameters
	bool calibZeroTangentDist;   // Assume zero tangential distortion
	bool calibFixPrincipalPoint; // Fix the principal point at the center
	bool flipVertical;           // Flip the captured images around the horizontal axis
	string outputFileName;       // The name of the file where to write
	bool showUndistorsed;        // Show undistorted images after calibration
	string input;                // The input ->
	bool useFisheye;             // use fisheye camera model for calibration

	int cameraID;
	vector<string> imageList;
	size_t atImageList;
	VideoCapture inputCapture;
	InputType inputType;
	bool goodInput;
	int flag;

private:
	string patternToUse;


};

static inline void read(const FileNode& node, Settings& x, const Settings& default_value = Settings())
{
	if (node.empty())
		x = default_value;
	else
		x.read(node);
}

static inline void write(FileStorage& fs, const String&, const Settings& s)
{
	s.write(fs);
}

enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };
static bool runCalibrationAndSave(Settings& s, Size imageSize, Mat&  cameraMatrix, Mat& distCoeffs,
	vector<vector<Point2f> > corners);
/*����궨����ģ���ʵ����������*/  
void calRealPoint(vector<vector<Point3f>>& obj, int boardwidth,int boardheight, int imgNumber, int squaresize)  
{  
//  Mat imgpoint(boardheight, boardwidth, CV_32FC3,Scalar(0,0,0));  
    vector<Point3f> imgpoint;  
    for (int rowIndex = 0; rowIndex < boardheight; rowIndex++)  
    {  
        for (int colIndex = 0; colIndex < boardwidth; colIndex++)  
        {  
        //  imgpoint.at<Vec3f>(rowIndex, colIndex) = Vec3f(rowIndex * squaresize, colIndex*squaresize, 0);  
            imgpoint.push_back(Point3f(rowIndex * squaresize, colIndex * squaresize, 0));  
        }  
    }  
    for (int imgIndex = 0; imgIndex < imgNumber; imgIndex++)  
    {  
        obj.push_back(imgpoint);  
    }  
}  

int main(int argc, char* argv[])  
{
	help();

	//! [file_read]
	Settings s;
	FileStorage fs("in_VID5.xml", FileStorage::READ); // Read the settings
	if (!fs.isOpened())
	{
		cout << "Could not open the configuration file: \"" << "in_VID5.xml" << "\"" << endl;
		return -1;
	}
	fs["Settings"] >> s;
	fs.release();                                                     // close Settings file

	const string outCameraDataFilePath = "out_camera_data.xml";       // write config as YAML
	if (outCameraDataFilePath.size() > 1)
		s.outputFileName = outCameraDataFilePath;

	if (!s.goodInput)
	{
		cout << "Invalid input detected. Application stopping. " << endl;
		return -1;
	}

	//[file_read]

	int mode = s.inputType == Settings::IMAGE_LIST ? CAPTURING : DETECTION;
	Size imageSize;
	clock_t prevTimestamp = 0;
	const Scalar RED(0, 0, 255), GREEN(0, 255, 0);

	for (;;)
	{
		Mat view;
		view = s.nextImage();
		//-----  If no more image, or got enough, then stop calibration and show result -------------
		if (mode == CAPTURING && corners.size() >= (size_t)s.nrFrames)
		{
			if (runCalibrationAndSave(s, imageSize, intrinsic, distortion_coeff, corners))
				mode = CALIBRATED;
			else
				mode = DETECTION;
		}
		if (view.empty())          // If there are no more images stop the loop
		{
			// if calibration threshold was not reached yet, calibrate now
			if (mode != CALIBRATED && !corners.empty())
				runCalibrationAndSave(s, imageSize, intrinsic, distortion_coeff, corners);
			break;
		}

		imageSize = view.size();  // Format input image.
		if (s.flipVertical)    flip(view, view, 0);

		//cvtColor(view, view, CV_BGR2GRAY);

		bool isFind;
		int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
		switch (s.calibrationPattern) // Find feature points on the input format
		{
		case Settings::CHESSBOARD:
			isFind = findChessboardCorners(view, s.boardSize, corner, chessBoardFlags);
			break;
		case Settings::CIRCLES_GRID:
			isFind = findCirclesGrid(view, s.boardSize, corner);
			break;
		case Settings::ASYMMETRIC_CIRCLES_GRID:
			isFind = findCirclesGrid(view, s.boardSize, corner, CALIB_CB_ASYMMETRIC_GRID);
			break;
		default:
			isFind = false;
			break;
		}

		if (isFind)                // If done with success,
		{
			// improve the found corners' coordinate accuracy for chessboard
			if (s.calibrationPattern == Settings::CHESSBOARD)
			{
				Mat viewGray;
				cvtColor(view, viewGray, COLOR_BGR2GRAY);
				cornerSubPix(viewGray, corner, Size(11, 11),
					Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
			}

			if (mode == CAPTURING &&  // For camera only take new samples after delay time
				(!s.inputCapture.isOpened() || clock() - prevTimestamp > s.delay*1e-3*CLOCKS_PER_SEC))
			{
				corners.push_back(corner);
				prevTimestamp = clock();
				//blinkOutput = s.inputCapture.isOpened();
			}

			Point2f boardCenter;
			Point2f imageCenter, p1, p2;
			double lrDeviation;
			double udDeviation;
			double boardArea;

			p1.x = 0;
			p1.y = 0;
			
			p2.y = view.rows;
			p2.x = view.cols;
			// Draw the corners.
			//drawChessboardCorners(view, s.boardSize, Mat(corner), isFind);
			drawCircle(view, corner[0], corner[s.boardSize.width * s.boardSize.height - 1], boardCenter);
			drawCircle(view, p1, p2, imageCenter);

			lrDeviation = boardCenter.x - imageCenter.x;
			udDeviation = boardCenter.y - imageCenter.y;
			boardArea = (corner[s.boardSize.width * s.boardSize.height - 1].x - corner[0].x) * (corner[s.boardSize.width * s.boardSize.height - 1].y - corner[0].y);

			if (fabs(lrDeviation) < 2)
			{
			}
	
			std::cout << boardCenter << std::endl << lrDeviation << std::endl << udDeviation << std::endl << boardArea << std::endl;
		}
		else
		{
			cout << "The image is bad please try again" << endl;
		}
	}
   
    return 0;  
}  

cv::Point2f drawCircle(cv::Mat image, cv::Point2f p1, cv::Point2f p2, cv::Point2f& centerPoint)
{
	centerPoint.x = (p1.x + p2.x) / 2;
	centerPoint.y = (p1.y + p2.y) / 2;

	cv::circle(image, centerPoint, 2, cv::Scalar(0, 0, 255));
	return centerPoint;
}
//! [run_and_save]
static bool runCalibrationAndSave(Settings& s, Size imageSize, Mat& cameraMatrix, Mat& distCoeffs,
	vector<vector<Point2f> > corners)
{
	cout << "guess successful" << endl;
	//����ʵ�ʵ�У�������ά����
	calRealPoint(objRealPoint, s.boardSize.width, s.boardSize.height, s.nrFrames, s.squareSize);
	cout << "cal real successful" << endl;

	double totalAvgErr = 0;

	bool ok = calibrateCamera(objRealPoint, corners, Size(imageSize.width, imageSize.height), intrinsic, distortion_coeff, rvecs, tvecs, 0);
	cout << (ok ? "Calibration succeeded" : "Calibration failed")
		<< ". avg re projection error = " << totalAvgErr << endl;

	return ok;
}
//! [run_and_save]