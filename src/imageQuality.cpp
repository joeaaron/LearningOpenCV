#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\core\core.hpp>

//************************************
// Method:    GetPSNR
// FullName:  GetPSNR
// Access:    public 
// Returns:   double 
// Qualifier: PSNR越高，图像和原图越接近; 用得最多，但是其值不能很好地反映人眼主观感受。一般取值范围：20-40.值越大，视频质量越好。
// Parameter: const cv::Mat & I1
// Parameter: const cv::Mat & I2
//************************************
double GetPSNR(const cv::Mat& I1, const cv::Mat& I2)
{
	cv::Mat s1;
	absdiff(I1, I2, s1); /* |I1 - I2| */
	s1.convertTo(s1, CV_32F); /* cannot make a square on 8 bits */
	s1 = s1.mul(s1); /* |I1 - I2|^2 */
	cv::Scalar s = sum(s1); /* sum elements per channel */
	double sse = s.val[0] + s.val[1] + s.val[2]; /* sum channels */

	if (sse <= 1e-10) /* for small values return zero */
		return 0;
	else {
		double mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0 * log10((255 * 255) / mse);
		return psnr;
	}
}

//************************************
// Method:    GetMSSIM
// FullName:  GetMSSIM
// Access:    public 
// Returns:   cv::Scalar
// Qualifier:
// Parameter: const cv::Mat & i1
// Parameter: const cv::Mat & i2
//************************************
cv::Scalar GetMSSIM(const cv::Mat& i1, const cv::Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	int d = CV_32F;
	cv::Mat I1, I2;

	/* cannot calculate on one byte large values */
	i1.convertTo(I1, d);
	i2.convertTo(I2, d);

	cv::Mat I2_2 = I2.mul(I2); /* I2^2 */
	cv::Mat I1_2 = I1.mul(I1); /* I1^2 */
	cv::Mat I1_I2 = I1.mul(I2); /* I1 * I2 */
	cv::Mat mu1, mu2; /* PRELIMINARY COMPUTING */

	cv::GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
	cv::GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);

	cv::Mat mu1_2 = mu1.mul(mu1);
	cv::Mat mu2_2 = mu2.mul(mu2);
	cv::Mat mu1_mu2 = mu1.mul(mu2);
	cv::Mat sigma1_2, sigma2_2, sigma12;

	cv::GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;
	cv::GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;
	cv::GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;

	/* FORMULA */
	cv::Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	/* t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2)) */
	t3 = t1.mul(t2);
	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	/* t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2)) */
	t1 = t1.mul(t2);

	cv::Mat ssim_map;
	divide(t3, t1, ssim_map); /* ssim_map =  t3./t1; */
	cv::Scalar mssim = cv::mean(ssim_map); /* mssim=avg(ssim map) */

	return mssim;
}

void GetHash(cv::Mat src1, cv::Mat src2)
{
	//cv::resize(src, src1, cv::Size(357, 419), 0, 0, cv::INTER_NEAREST);
	////cv::flip(matSrc1, matSrc1, 1);
	//cv::resize(src, src2, cv::Size(2177, 3233), 0, 0, cv::INTER_LANCZOS4);

	cv::Mat matDst1, matDst2;

	cv::resize(src1, matDst1, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
	cv::resize(src2, matDst2, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);

	cv::cvtColor(matDst1, matDst1, CV_BGR2GRAY);
	cv::cvtColor(matDst2, matDst2, CV_BGR2GRAY);

	int iAvg1 = 0;
	int iAvg2 = 0;

	int arr1[64], arr2[64];

	for (int i = 0; i < 8; i++)
	{
		uchar* data1 = matDst1.ptr<uchar>(i);
		uchar* data2 = matDst2.ptr<uchar>(i);

		int tmp = i * 8;

		for (int j = 0; j < 8; j++)
		{
			int tmp1 = tmp + j;

			arr1[tmp1] = data1[j] / 4 * 4;
			arr2[tmp1] = data2[j] / 4 * 4;

			iAvg1 += arr1[tmp1];
			iAvg2 += arr2[tmp1];
		}
	}

	iAvg1 /= 64;
	iAvg2 /= 64;

	for (int i = 0; i < 64; i++)
	{
		arr1[i] = (arr1[i] >= iAvg1) ? 1 : 0;
		arr2[i] = (arr2[i] >= iAvg2) ? 1 : 0;
	}

	int iDiffNum = 0;

	for (int i = 0; i < 64; i++)
		if (arr1[i] != arr2[i])
			++iDiffNum;

	std::cout << "iDiffNum = " << iDiffNum << std::endl;

	if (iDiffNum <= 5)
		std::cout << "two images ars very similar!" << std::endl;
	else if (iDiffNum > 10)
		std::cout << "they are two different images!" << std::endl;
	else
		std::cout << "two image are somewhat similar!" << std::endl;
}

void CalculateHistogram(const cv::Mat& srcImage, const std::string& windowName, int x, int y)
{
	/*separate the image in 3 places(B, G and R) */
	std::vector<cv::Mat> bgr_planes;
	cv::split(srcImage, bgr_planes);

	/*Establish the number of bins*/
	int histSize = 256;

	/*set the ranges(for B,G,R)*/
	float range[] = { 0, 256};
	const float* histRange = { range };
	bool uniform = true;
	bool accumulate = true;
	cv::Mat b_hist, g_hist, r_hist;

	/*compute the histograms:*/
	cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize,
		&histRange, uniform, accumulate);
	/*std::cout << "---BLUE---" << std::endl << b_hist << std::endl
		<< std::endl;*/
	cv::calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize,
		&histRange, uniform, accumulate);
	//std::cout << "---GREEN---" << std::endl << g_hist
	//	<< std::endl << std::endl;
	cv::calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize,
		&histRange, uniform, accumulate);
	/*std::cout << "---RED---" << std::endl << r_hist
		<< std::endl << std::endl;*/

	/* Draw the histograms for B, G and R */
	int hist_w = 512;
	int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);
	cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

	/* Normalize the result to [ 0, histImage.rows ] */
	cv::normalize(b_hist, b_hist, 0, histImage.rows,
		cv::NORM_MINMAX, -1, cv::Mat());
	cv::normalize(g_hist, g_hist, 0, histImage.rows,
		cv::NORM_MINMAX, -1, cv::Mat());
	cv::normalize(r_hist, r_hist, 0, histImage.rows,
		cv::NORM_MINMAX, -1, cv::Mat());

	/* Draw for each channel */
	for (int i = 1; i < histSize; i++)
	{
		cv::line(histImage, cv::Point(bin_w*(i - 1),
			hist_h - cvRound(b_hist.at<float>(i - 1))),
			cv::Point(bin_w*(i),
			hist_h - cvRound(b_hist.at<float>(i))),
			cv::Scalar(255, 0, 0), 1, 4, 0);
		cv::line(histImage, cv::Point(bin_w*(i - 1),
			hist_h - cvRound(g_hist.at<float>(i - 1))),
			cv::Point(bin_w*(i),
			hist_h - cvRound(g_hist.at<float>(i))),
			cv::Scalar(0, 255, 0), 1, 4, 0);
		cv::line(histImage, cv::Point(bin_w*(i - 1),
			hist_h - cvRound(r_hist.at<float>(i - 1))),
			cv::Point(bin_w*(i),
			hist_h - cvRound(r_hist.at<float>(i))),
			cv::Scalar(0, 0, 255), 1, 4, 0);
	}

	/* Display */
	cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
	cv::imshow(windowName, histImage);
	cv::moveWindow(windowName, x, y);
}

void ShowFrames(const cv::Mat& imageToShow,
	const std::string& windowname, int x, int y)
{
	//display image
	cv::namedWindow(windowname, cv::WINDOW_AUTOSIZE);
	cv::imshow(windowname, imageToShow);
	cv::moveWindow(windowname, x, y);
}

std::string DoubleToString(double doubleVal)
{
	std::stringstream doubleBuf;
	doubleBuf << doubleVal;
	return doubleBuf.str();
}

int main()
{
	cv::Mat referenceImage = cv::imread("std.bmp");
	if (!referenceImage.data)
	{
		std::cout << "Reference image has no data!" << std::endl;
		return -1;
	}

	const std::string sourceCompareWith = "x5.bmp";       //file path attention please!
	cv::Mat sampleImage = cv::imread(sourceCompareWith);
	if (!sampleImage.data) {
		std::cout << "Sample image has no data!" << std::endl;
		return -1;
	}

	//感知哈希算法
	GetHash(referenceImage, sampleImage);
	//check sizes here, in case of resizing required before test
	//do comparisons
	double psnrV;
	cv::Scalar mssimV;

	/* PSNR */
	psnrV = GetPSNR(referenceImage, sampleImage);
	std::cout << "PSNR:"<< " " << std::setiosflags(std::ios::fixed) << std::setprecision(3) << psnrV << "dB" << std::endl;

	/* SSIM */
	mssimV = GetMSSIM(referenceImage, sampleImage);
	std::cout << "MSSIM:"
		<< " R " << std::setiosflags(std::ios::fixed) << std::setprecision(2)
		<< mssimV.val[2] * 100 << "%"
		<< " G " << std::setiosflags(std::ios::fixed) << std::setprecision(2)
		<< mssimV.val[1] * 100 << "%"
		<< " B " << std::setiosflags(std::ios::fixed) << std::setprecision(2)
		<< mssimV.val[0] * 100 << "%" << std::endl;

	std::cout << std::endl;

	//show images under test
	ShowFrames(referenceImage, "REFERENCE_WINDOW", 200, 0);
	ShowFrames(sampleImage, "SAMPLE_WINDOW", 800, 0);

	//calculate and display histograms
	CalculateHistogram(referenceImage, "Reference Image", 200, 400);
	CalculateHistogram(sampleImage, "Sample Image", 800, 400);
	
	cv::waitKey(0);
	cv::destroyAllWindows();

	system("pause");
	return 0;
	
}