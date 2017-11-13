#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/stitching/detail/seam_finders.hpp>
#include <opencv2/core/mat.hpp>
const int OVERLAPREGION = 50;

#if 1
void FindGap(cv::Mat canvas1, cv::Mat canvas2)
{
	//将两幅图剪切出来，剪切位置包含了配准（两幅图像的相对位置）信息
	cv::Mat image1 = canvas1(cv::Range::all(), cv::Range(0, canvas1.cols / 2));
	cv::Mat image2 = canvas2(cv::Range::all(), cv::Range(canvas2.cols / 4, canvas2.cols * 3 / 4)); //假设大概1/2重复区域

	image1.convertTo(image1, CV_32FC3);
	image2.convertTo(image2, CV_32FC3);
	image1 /= 255.0;
	image2 /= 255.0;

	//在找拼缝的操作中，为了减少计算量，用image_small
	cv::Mat image1_small;
	cv::Mat image2_small;
	cv::Size small_size1 = cv::Size(image1.cols / 2, image1.rows / 2);
	cv::Size small_size2 = cv::Size(image2.cols / 2, image2.rows / 2);
	resize(image1, image1_small, small_size1);
	resize(image2, image2_small, small_size2);

	// 左图的左上角坐标
	cv::Point corner1;
	corner1.x = 0;
	corner1.y = 0;

	//右图的左上角坐标
	cv::Point corner2;
	corner2.x = image2_small.cols / 2;
	corner2.y = 0;

	std::vector<cv::Point> corners;

	corners.push_back(corner1);
	corners.push_back(corner2);

	std::vector<cv::Mat> masks;
	cv::Mat imageMask1(small_size1, CV_8U);
	cv::Mat imageMask2(small_size2, CV_8U);
	imageMask1 = cv::Scalar::all(255);
	imageMask2 = cv::Scalar::all(255);

	masks.push_back(imageMask1);
	masks.push_back(imageMask2);

	std::vector<cv::Mat> sources;

	sources.push_back(image1_small);
	sources.push_back(image2_small);

	cv::Ptr<cv::detail::SeamFinder> seam_finder = new cv::detail::GraphCutSeamFinder(cv::detail::GraphCutSeamFinderBase::COST_COLOR);

	seam_finder->find(sources, corners, masks);

	//将mask恢复放大
	resize(masks[0], imageMask1, image1.size());
	resize(masks[1], imageMask2, image2.size());

	cv::Mat canvas(image1.rows, image1.cols * 3 / 2, CV_32FC3);
	image1.copyTo(canvas(cv::Range::all(), cv::Range(0, canvas.cols * 2 / 3)), imageMask1);
	image2.copyTo(canvas(cv::Range::all(), cv::Range(canvas.cols / 3, canvas.cols)), imageMask2);
	/*canvas *= 255;
	canvas.convertTo(canvas, CV_8UC3);*/
	cv::imshow("canvas", canvas);

	cv::imshow("Mask1", masks[0]);
	cv::imshow("Mask2", masks[1]);

	cv::imshow("src1", sources[0]);
	cv::imshow("src2", sources[1]);

	cv::waitKey(0);

}
#endif


int main()
{
	cv::Mat src = cv::imread("laser.bmp");
	if (src.empty())
	{
		return -1;
	}

	int width = src.cols;
	int height = src.rows;

	cv::Mat srcLeft = src(cv::Range(0, height), cv::Range(0, OVERLAPREGION));
	cv::imshow("Partition Image", srcLeft);

	cv::Mat srcRight= src(cv::Range(0, height), cv::Range(OVERLAPREGION, width));
	cv::imshow("Partition Image", srcRight);

	cv::Mat dst;
	cv::hconcat(srcLeft, srcRight, dst);

	cv::imshow("result Iamge", dst);
	cv::imwrite("merge.png", dst);

	//FindGap(src, srcPartition);
	cv::waitKey(0);
	return 0;

}