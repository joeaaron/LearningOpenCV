#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

int main()
{
	Mat imageSource = imread("lena.jpg");
	Mat imageResult = imageSource.clone();
	RNG rng;
	int randomNum;
	int Number = 5;

	for (int i = 0; i < imageSource.rows - Number; i++)
		for (int j = 0; j < imageSource.cols - Number; j++)
		{
			randomNum = rng.uniform(0, Number);
			imageResult.at<Vec3b>(i, j)[0] = imageSource.at<Vec3b>(i + randomNum, j + randomNum)[0];
			imageResult.at<Vec3b>(i, j)[1] = imageSource.at<Vec3b>(i + randomNum, j + randomNum)[1];
			imageResult.at<Vec3b>(i, j)[2] = imageSource.at<Vec3b>(i + randomNum, j + randomNum)[2];
		}
	imshow("Ã«²£Á§Ð§¹û", imageResult);
	waitKey();
}