#include <opencv2/opencv.hpp>

//原始LBP特征计算
template <typename _tp>
void getOriginalLBPFeature(cv::InputArray _src, cv::OutputArray _dst)
{
	cv::Mat src = _src.getMat();
	_dst.create(src.rows - 2, src.cols - 2, CV_8UC1);
	cv::Mat dst = _dst.getMat();
	dst.setTo(0);

	for (int i = 1; i < src.rows - 1;i++)
	{
		for (int j = 1; j < src.cols - 1; j++)
		{
			_tp center = src.at<_tp>(i, j);
			unsigned char lbpCode = 0;
			lbpCode |= (src.at<_tp>(i - 1, j - 1) > center) << 7;
			lbpCode |= (src.at<_tp>(i - 1, j) > center) << 6;
			lbpCode |= (src.at<_tp>(i - 1, j + 1) > center) << 5;
			lbpCode |= (src.at<_tp>(i, j + 1) > center) << 4;
			lbpCode |= (src.at<_tp>(i + 1, j + 1) > center) << 3;
			lbpCode |= (src.at<_tp>(i + 1, j) > center) << 2;
			lbpCode |= (src.at<_tp>(i + 1, j - 1) > center) << 1;
			lbpCode |= (src.at<_tp>(i, j - 1) > center) << 0;

			dst.at<int>(i - 1, j - 1) = lbpCode;
		}
	}
}

int main()
{
	cv::Mat src = cv::imread("lena.jpg");
	cv::Mat result;

	getOriginalLBPFeature<uchar>(src, result);

	cv::imshow("result_window", result);
	cv::waitKey(0);
	return 0;
}