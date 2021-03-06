#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <math.h>

//塘双の耶忖　いくつくらい�奮�するか寄悶を嚠�襪靴鈍m卷�筝�する0
#define NUM 30

void FindLines2(void);
void FindCircles(void);


//cv::Mat dst_img, work_img, sort_img, fin_img, choose_img;

int main()
{
	//峠佩�濕豌�
	FindLines2();

	//揖伉垈殊霞
  // FindCircles();

	cv::waitKey(0);
	return 0;
}

void FindLines2() {
	cv::Mat dst_img, work_img, sort_img, fin_img, choose_img;
	/*殊霞叔業才買弍業��*/
	double ang_box[NUM];
	double tilt_box[NUM];
	double b[NUM];

	//延楚柴方
	int i = 0, k = 0, l = 0;
	int count = 0;

	//喘延楚栖贋刈殊霞議軟泣才嶮泣
	cv::Point startPoint[NUM], goalPoint[NUM];

	cv::Mat src_img = cv::imread("./data/line_circle/test5.jpg");

	dst_img = src_img.clone();
	sort_img = src_img.clone();
	fin_img = src_img.clone();
	choose_img = src_img.clone();

	fin_img = cv::Scalar(255, 255, 255);

	cv::namedWindow("src", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("src", src_img);

	cv::cvtColor(src_img, work_img, CV_BGR2GRAY);

	cv::Canny(work_img, work_img, 100, 100, 3);

	//古楕Hough延算
	std::vector<cv::Vec4i> lines;

	cv::HoughLinesP(work_img, lines, 1, CV_PI / 180, 50, 60, 10);

	std::vector<cv::Vec4i>::iterator it = lines.begin();

	//蝕兵殊霞朔��繍軟兵叔業才嶄泣叔業贋秘延楚
	for (; it != lines.end(); ++it) {
		cv::Vec4i l = *it;
		cv::line(dst_img, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, CV_AA);

		//贋刈叔業才恫炎
		double ang = atan2(double(l[3] - l[1]), double(l[2] - l[0]));

		ang = ang * 180 / 3.1415926535;

		if (ang < 0) {
			ang = ang + 360;
		}

		double tilt = tan(ang) *-1;

		ang_box[i] = ang;

		tilt_box[i] = tilt;

		b[i] = l[1] - tilt * l[0];

		//std::cout << "兵泣恙��(x,y) = " << cv::Point(l[0],l[1]) << std::endl;

		//std::cout << "�K泣恙��(x,y) = " << cv::Point(l[2],l[3]) << std::endl;

		//喘方怏贋刈耽訳�澣墜雉禳蓉婬穽�炎
		startPoint[i] = cv::Point(l[0], l[1]);

		goalPoint[i] = cv::Point(l[2], l[3]);

		//std::cout << "ang = " << (int)ang << std::endl;

		//std::cout << "�Aき = " << tilt << std::endl;

		//std::cout << " b = " << l[1] - tilt * l[0] << std::endl;

		i++;

	}

	double length, start_len, goal_len, sg_len;
	int check[NUM] = { 0 };

	for (k = 0; k < i; k++) {
		for (l = k; l < i; l++) {
			start_len = sqrt(pow((startPoint[k].x - startPoint[l].x), 2.0) + pow((startPoint[k].y - startPoint[l].y), 2.0));
			goal_len = sqrt(pow((goalPoint[k].x - goalPoint[l].x), 2.0) + pow((goalPoint[k].y - goalPoint[l].y), 2.0));
			sg_len = sqrt(pow((goalPoint[k].x - startPoint[l].x), 2.0) + pow((goalPoint[k].y - startPoint[l].y), 2.0));
			//std::cout << "軟兵泣岻寂議鉦宣 = "<< start_len << std::endl;
			//std::cout << "嶄泣岻寂議鉦宣 = "<< goal_len << std::endl;
			if ((((0 < start_len && start_len < 75) && (0 < goal_len && goal_len < 75)) || (sg_len < 70) && (tilt_box[l] / tilt_box[k]) < 1.2
				&& (tilt_box[l] / tilt_box[k]) > 0.8) && (b[k] - b[l] < 30 || b[l] - b[k] < 30)) {
				//vote++;
				check[k] = 1;
			}
		}
	}

	count = 0;
	std::cout << "-----------蛍侯憲--------------" << std::endl;

	double sort_tilt[NUM] = { 0 }, sort_b[NUM] = { 0 };
	cv::Point sort_start[NUM] = { 0 }, sort_goal[NUM] = { 0 };

	for (int k = 0; k < i; k++) {
		if (check[k] == 0) {
			std::cout << "[買弍叔業 舜鉦 兵泣　�K泣] = [" << tilt_box[k] << "," << b[k] << "," << startPoint[k] << "," << goalPoint[k] << "]" << std::endl;
			cv::line(choose_img, startPoint[k], goalPoint[k], cv::Scalar(255, 0, 0), 1, CV_AA);
			sort_tilt[count] = tilt_box[k];
			sort_b[count] = b[k];
			sort_start[count] = startPoint[k];
			sort_goal[count] = goalPoint[k];
			count++;
		}
	}

	std::cout << "凪噫議��" << std::endl;
	for (k = 0; k < count; k++) {
		std::cout << "[買弍叔業 舜鉦 兵泣　�K泣] = [" << sort_tilt[k] << "," << sort_b[k] << "," << sort_start[k] << "," << sort_goal[k] << "]" << std::endl;
	}


	cv::Point fin_s, fin_g;

	for (k = 0; k < count; k++) {
		for (l = k + 1; l < count; l++) {
			if (sort_tilt[k] / sort_tilt[l] < 2.5 && sort_tilt[k] / sort_tilt[l] > 0.5 && (sort_b[k] * sort_b[l]) > 0) {

				fin_s.x = (sort_start[k].x + sort_start[l].x) / 2;
				fin_s.y = (sort_start[k].y + sort_start[l].y) / 2;

				fin_g.x = ((sort_goal[k].x + sort_goal[l].x)) / 2;
				fin_g.y = ((sort_goal[k].y + sort_goal[l].y)) / 2;

				cv::line(fin_img, fin_s, fin_g, cv::Scalar(0, 0, 255), 1, CV_AA);
			}
		}
	}

	//潤惚夕紙崙
	cv::namedWindow("HoughLinesP", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("HoughLinesP", dst_img);

	cv::namedWindow("sort", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("sort", sort_img);

	cv::namedWindow("choose", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("choose", choose_img);

	cv::namedWindow("fin", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("fin", fin_img);

}

#if 0

void FindCircles() {

	//蕗苧延楚贋刈夕��
	cv::Mat src_img = cv::imread("./data/line_circle/test9.png", 1);
	cv::Mat src_gray;
	cv::Mat dst_img, work_img, diff_img, np_img, gray_img, mask_img, result_img, final_img;

	mask_img = cv::Scalar(0, 0, 0);
	dst_img = src_img.clone();
	fin_img = src_img.clone();
	result_img = src_img.clone();
	final_img = src_img.clone();

	//潤惚夕喘歓易�駛惶塚赦餞棺�
	dst_img = cv::Scalar(255, 255, 255);
	result_img = cv::Scalar(255, 255, 255);
	final_img = cv::Scalar(255, 255, 255);

	//參子業夕�饑間莞�inpaint孔嬬才Hough延算
	cv::cvtColor(src_img, src_gray, CV_RGB2GRAY);
	cv::cvtColor(src_img, work_img, CV_BGR2GRAY);

	//喘延楚贋刈磯抄才嶄伉恫炎
	int i = 0, k = 0, l = 0, m = 0;
	int rad_box[NUM] = { 0 }, rad_box2[NUM] = { 0 };
	cv::Point cen_box[NUM] = { 0 }, cen_box2[NUM] = { 0 };

	// Hough延算念議圓侃尖�┯瀚稿住�契峭窟伏列殊��
	cv::GaussianBlur(work_img, work_img, cv::Size(11, 11), 3, 3);

	// 喘hough延算殊霞垈
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(work_img, circles, CV_HOUGH_GRADIENT, 1, 5, 40, 100, 20, 100);

	std::vector<cv::Vec3f>::iterator it = circles.begin();
	for (; it != circles.end(); ++it) {
		cv::Point center(cv::saturate_cast<int>((*it)[0]), cv::saturate_cast<int>((*it)[1]));
		int radius = cv::saturate_cast<int>((*it)[2]);

		//cv::circle(dst_img, center, 2, cv::Scalar(255,0,0), -1, 8);

		cv::circle(dst_img, center, radius, cv::Scalar(0, 0, 255), 8, 8, 0);

		//贋刈磯抄才嶄伉恫炎
		rad_box[i] = radius;
		cen_box[i].x = center.x;
		cen_box[i].y = center.y;

		//殊霞欺議垈議倖方
		i++;
	}

	//補竃殊霞欺議垈議磯抄才嶄伉恫炎
	for (k = 0; k < i; k++) {
		std::cout << "[磯抄,嶄伉] = {" << rad_box[k] << "," << cen_box[k] << "}" << std::endl;
	}

	//斤恷嶮夕�饅�佩子業延算
	cv::cvtColor(dst_img, gray_img, CV_RGB2GRAY);

	//夕�餬�廬圭宴序佩凉庁侃尖
	np_img = ~gray_img;

	//評茅音駅勣議垈
	cv::inpaint(src_gray, np_img, fin_img, 15, cv::INPAINT_NS);

	//殊霞竃垈
	cv::HoughCircles(fin_img, circles, CV_HOUGH_GRADIENT, 1, 5, 30, 90, 1, 100);

	std::vector<cv::Vec3f>::iterator it2 = circles.begin();
	for (; it2 != circles.end(); ++it2) {
		cv::Point center2(cv::saturate_cast<int>((*it2)[0]), cv::saturate_cast<int>((*it2)[1]));
		int radius2 = cv::saturate_cast<int>((*it2)[2]);

		cv::circle(result_img, center2, radius2, cv::Scalar(0, 0, 255), 8, 8, 0);

		rad_box2[l] = radius2;
		cen_box2[l].x = center2.x;
		cen_box2[l].y = center2.y;

		l++;
	}

	//幹秀宴噐臥心議蛍侯憲
	std::cout << "------------------------------------------------" << std::endl;

	//揖伉垈登協
	for (k = 0; k < i; k++) {
		for (m = 0; m < l; m++) {
			if (cen_box[k].x - cen_box2[m].x <= 5 && cen_box[k].y - cen_box2[m].y <= 5) {
				std::cout << "[磯抄,嶄伉] = {" << rad_box2[k] << "," << cen_box2[k] << "}" << std::endl;
				cv::circle(final_img, cen_box2[m], 1, cv::Scalar(0, 0, 0), 8, 8, 0);
			}
		}
	}

	/*夕�餤脹輅�*/
	cv::namedWindow("src", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("src", src_img);

	cv::namedWindow("HoughCircles", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("HoughCircles", dst_img);

	cv::namedWindow("fin", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("fin", fin_img);

	cv::namedWindow("np", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("np", np_img);


	cv::namedWindow("result", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("result", result_img);

	cv::namedWindow("final", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("final", final_img);
}
#endif
