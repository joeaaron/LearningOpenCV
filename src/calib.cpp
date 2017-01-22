#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include "cv.h"  
#include <cv.hpp>  
#include <iostream>  
  
using namespace std;  
using namespace cv;  
  
const int imageWidth = 640;                             //摄像头的分辨率  
const int imageHeight = 480;  
//const int boardWidth = 11;                               //横向的角点数目  
//const int boardHeight = 7;                              //纵向的角点数据  
//const int boardCorner = boardWidth * boardHeight;       //总的角点数据  
//const int frameNumber = 5;                             //相机标定时需要采用的图像帧数  
//const int squareSize = 30;                              //标定板黑白格子的大小 单位mm  
//const Size boardSize = Size(boardWidth, boardHeight);   //  
      
Mat intrinsic;                                          //相机内参数  
Mat distortion_coeff;                                   //相机畸变参数  
vector<Mat> rvecs;                                        //旋转向量  
vector<Mat> tvecs;                                        //平移向量  
vector<vector<Point2f>> corners;                        //各个图像找到的角点的集合 和objRealPoint 一一对应  
vector<vector<Point3f>> objRealPoint;                   //各副图像的角点的实际物理坐标集合  
  
  
vector<Point2f> corner;                                   //某一副图像找到的角点  
  
Mat rgbImage, grayImage;  
  
static void help()
{
	cout << "This is a camera calibration sample." << endl
		<< "Usage: calibration configurationFile" << endl
		<< "Near the sample file you'll find the configuration file, which has detailed help of "
		"how to edit it.  It may be any OpenCV supported file format XML/YAML." << endl;
}

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


/*计算标定板上模块的实际物理坐标*/  
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
  
/*设置相机的初始参数 也可以不估计*/  
void guessCameraParam(void )  
{  
    /*分配内存*/  
    intrinsic.create(3, 3, CV_64FC1);  
    distortion_coeff.create(5, 1, CV_64FC1);  
  
    /* 
    fx 0 cx 
    0 fy cy 
    0 0  1 
    */  
    intrinsic.at<double>(0,0) = 256.8093262;   //fx         
    intrinsic.at<double>(0, 2) = 160.2826538;   //cx  
    intrinsic.at<double>(1, 1) = 254.7511139;   //fy  
    intrinsic.at<double>(1, 2) = 127.6264572;   //cy  
  
    intrinsic.at<double>(0, 1) = 0;  
    intrinsic.at<double>(1, 0) = 0;  
    intrinsic.at<double>(2, 0) = 0;  
    intrinsic.at<double>(2, 1) = 0;  
    intrinsic.at<double>(2, 2) = 1;  
  
    /* 
    k1 k2 p1 p2 p3 
    */  
    distortion_coeff.at<double>(0, 0) = -0.193740;  //k1  
    distortion_coeff.at<double>(1, 0) = -0.378588;  //k2  
    distortion_coeff.at<double>(2, 0) = 0.028980;   //p1  
    distortion_coeff.at<double>(3, 0) = 0.008136;   //p2  
    distortion_coeff.at<double>(4, 0) = 0;          //p3  
}  
  
void outputCameraParam(Settings &s)  
{  
    /*保存数据*/ 
	FileStorage fs;
	fs.open(s.outputFileName, FileStorage::WRITE);
    //cvSave("cameraMatrix.xml", &intrinsic);  
    //cvSave("cameraDistoration.xml", &distortion_coeff);  
    //cvSave("rotatoVector.xml", &rvecs);  
    //cvSave("translationVector.xml", &tvecs);  
    /*输出数据*/  
	if (fs.isOpened())
	{
		fs << "camera_matrix" << intrinsic;
		fs << "distortion_coefficients" << distortion_coeff;
		fs.release();
	}

	cout << "fx :" << intrinsic.at<double>(0, 0) << endl << "fy :" << intrinsic.at<double>(1, 1) << endl;
	cout << "cx :" << intrinsic.at<double>(0, 2) << endl << "cy :" << intrinsic.at<double>(1, 2) << endl;

	cout << "k1 :" << distortion_coeff.at<double>(0, 0) << endl;
	cout << "k2 :" << distortion_coeff.at<double>(1, 0) << endl;
	cout << "p1 :" << distortion_coeff.at<double>(2, 0) << endl;
	cout << "p2 :" << distortion_coeff.at<double>(3, 0) << endl;
	cout << "p3 :" << distortion_coeff.at<double>(4, 0) << endl;

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
	fs.release();                                         // close Settings file

	const string outCameraDataFilePath = "out_camera_data.xml";
	if (outCameraDataFilePath.size() > 1)
		s.outputFileName = outCameraDataFilePath;


	//! [file_read]

	//FileStorage fout("settings.yml", FileStorage::WRITE); // write config as YAML
	//fout << "Settings" << s;

	if (!s.goodInput)
	{
		cout << "Invalid input detected. Application stopping. " << endl;
		return -1;
	}

    Mat img;  
    int goodFrameCount = 0;  
    namedWindow("chessboard");  
    cout << "按Q退出 ..." << endl;  
    while (goodFrameCount < s.nrFrames)  
    {  
        char filename[100];  
        sprintf_s(filename,"x%d.bmp", goodFrameCount + 1);  
    //  cout << filename << endl;  
        rgbImage = imread(filename, CV_LOAD_IMAGE_COLOR);  
        cvtColor(rgbImage, grayImage, CV_BGR2GRAY);  
        imshow("Camera", grayImage);  
		bool isFind;
		int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
		switch (s.calibrationPattern) // Find feature points on the input format
		{
		case Settings::CHESSBOARD:
			isFind = findChessboardCorners(rgbImage, s.boardSize, corner, chessBoardFlags);
			break;
		case Settings::CIRCLES_GRID:
			isFind = findCirclesGrid(rgbImage, s.boardSize, corner);
			break;
		case Settings::ASYMMETRIC_CIRCLES_GRID:
			isFind = findCirclesGrid(rgbImage, s.boardSize, corner, CALIB_CB_ASYMMETRIC_GRID);
			break;
		default:
			isFind = false;
			break;
		}


        //bool isFind = findChessboardCorners(rgbImage, s.boardSize, corner,0);  
        if (isFind == true) //所有角点都被找到 说明这幅图像是可行的  
        {  
            /* 
            Size(5,5) 搜索窗口的一半大小 
            Size(-1,-1) 死区的一半尺寸 
            TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 20, 0.1)迭代终止条件 
            */  
            cornerSubPix(grayImage, corner, Size(5,5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 20, 0.1));  
            drawChessboardCorners(rgbImage, s.boardSize, corner, isFind);  
            imshow("chessboard", rgbImage);  
            corners.push_back(corner);  
            //string filename = "res\\image\\calibration";  
            //filename += goodFrameCount + ".jpg";  
            //cvSaveImage(filename.c_str(), &IplImage(rgbImage));       //把合格的图片保存起来  
            goodFrameCount++;  
            cout << "The image is good" << endl;  
        }  
        else  
        {  
            cout << "The image is bad please try again" << endl;  
        }  
    //  cout << "Press any key to continue..." << endl;  
    //  waitKey(0);  
  
       if (waitKey(10) == 'q')  
        {  
            break;  
        }  
    //  imshow("chessboard", rgbImage);  
    }  
  
    /* 
    图像采集完毕 接下来开始摄像头的校正 
    calibrateCamera() 
    输入参数 objectPoints  角点的实际物理坐标 
             imagePoints   角点的图像坐标 
             imageSize     图像的大小 
    输出参数 
             cameraMatrix  相机的内参矩阵 
             distCoeffs    相机的畸变参数 
             rvecs         旋转矢量(外参数) 
             tvecs         平移矢量(外参数） 
    */  
      
    /*设置实际初始参数 根据calibrateCamera来 如果flag = 0 也可以不进行设置*/  
    guessCameraParam();           
    cout << "guess successful" << endl;  
    /*计算实际的校正点的三维坐标*/  
    calRealPoint(objRealPoint, s.boardSize.width, s.boardSize.height,s.nrFrames, s.squareSize);  
    cout << "cal real successful" << endl;  
    /*标定摄像头*/  
    calibrateCamera(objRealPoint, corners, Size(imageWidth, imageHeight), intrinsic, distortion_coeff, rvecs, tvecs, 0);  
    cout << "calibration successful" << endl;  
    /*保存并输出参数*/  
	outputCameraParam(s);
    cout << "out successful" << endl;  
      
    /*显示畸变校正效果*/  
    Mat cImage;  
    undistort(rgbImage, cImage, intrinsic, distortion_coeff);  
    imshow("Correct Image", cImage);  
    cout << "Correct Image" << endl;  
    cout << "Wait for Key" << endl;  
    waitKey(0);  
    system("pause");  
    return 0;  
}  