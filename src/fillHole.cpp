#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

void fillHole(const cv::Mat srcBw, cv::Mat &dstBw)
{
	cv::Size m_Size = srcBw.size();
	cv::Mat Temp = cv::Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcBw.type());//—”’πÕºœÒ  
	srcBw.copyTo(Temp(cv::Range(1, m_Size.height + 1), cv::Range(1, m_Size.width + 1)));

	cv::floodFill(Temp, cv::Point(0, 0), cv::Scalar(255));

	cv::Mat cutImg;//≤√ºÙ—”’πµƒÕºœÒ  
	Temp(cv::Range(1, m_Size.height + 1), cv::Range(1, m_Size.width + 1)).copyTo(cutImg);

	dstBw = srcBw | (~cutImg);

}

int main()
{
	cv::Mat img = cv::imread("2.jpg");

	cv::Mat gray;
	cv::cvtColor(img, gray, CV_RGB2GRAY);

	cv::Mat bw;
	cv::threshold(gray, bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	cv::Mat bwFill;
	fillHole(bw, bwFill);

	cv::imshow("ÃÓ≥‰«∞", gray);
	cv::imshow("ÃÓ≥‰∫Û", bwFill);
	cv::waitKey();
	return 0;
}