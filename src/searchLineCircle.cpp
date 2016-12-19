#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <math.h>

//配列の添字　いくつくらい検出するか大体を予想して適宜変更する
#define NUM 30

void FindLines2(void);
void FindCircles(void);


cv::Mat dst_img, work_img, sort_img, fin_img, choose_img;

int main()
{
	//平行線検出
	FindLines2();

	//同心円検出
	//FindCircles();

	cv::waitKey(0);
	return 0;
}

void FindLines2() {

	/*検知した角度や傾き、切片を入れる変数*/
	double ang_box[NUM];
	double tilt_box[NUM];
	double b[NUM];

	//カウントする変数
	int i = 0, k = 0, l = 0;
	int count = 0;

	//検出した始点終点を格納する変数
	cv::Point startPoint[NUM], goalPoint[NUM];

	//原画像の読み込み
	cv::Mat src_img = cv::imread("image/test5.png");

	//結果を描画する画像を準備
	dst_img = src_img.clone();
	sort_img = src_img.clone();
	fin_img = src_img.clone();
	choose_img = src_img.clone();

	//結果を描画するために真っ白な画像の生成
	fin_img = cv::Scalar(255, 255, 255);

	//原画像の出力
	cv::namedWindow("src", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("src", src_img);

	//グレースケールに変換
	cv::cvtColor(src_img, work_img, CV_BGR2GRAY);

	//エッジ検出
	cv::Canny(work_img, work_img, 100, 100, 3);

	//確率的ハフ変換
	std::vector<cv::Vec4i> lines;

	cv::HoughLinesP(work_img, lines, 1, CV_PI / 180, 50, 60, 10);

	std::vector<cv::Vec4i>::iterator it = lines.begin();

	//検知した後、始点終点角度を変数に入れる
	for (; it != lines.end(); ++it) {
		cv::Vec4i l = *it;
		cv::line(dst_img, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, CV_AA);

		//角度や座標を格納していく
		double ang = atan2(double(l[3] - l[1]), double(l[2] - l[0]));

		ang = ang * 180 / 3.1415926535;

		//0～360以内にする
		if (ang < 0) {
			ang = ang + 360;
		}

		double tilt = tan(ang) *-1;

		ang_box[i] = ang;

		tilt_box[i] = tilt;

		b[i] = l[1] - tilt * l[0];

		//std::cout << "始点座標(x,y) = " << cv::Point(l[0],l[1]) << std::endl;

		//std::cout << "終点座標(x,y) = " << cv::Point(l[2],l[3]) << std::endl;

		//それぞれの始点終点座標を配列に格納
		startPoint[i] = cv::Point(l[0], l[1]);

		goalPoint[i] = cv::Point(l[2], l[3]);

		//std::cout << "ang = " << (int)ang << std::endl;

		//std::cout << "傾き = " << tilt << std::endl;

		//std::cout << " b = " << l[1] - tilt * l[0] << std::endl;

		//円のいくつ検出したかカウントする
		i++;

	}

	double length, start_len, goal_len, sg_len;
	int check[NUM] = { 0 };

	/*	始点終点中点、現時点の終点と次回の始点、傾きが近いもので近くにある線を消す	*/
	for (k = 0; k < i; k++) {
		for (l = k; l < i; l++) {
			start_len = sqrt(pow((startPoint[k].x - startPoint[l].x), 2.0) + pow((startPoint[k].y - startPoint[l].y), 2.0));
			goal_len = sqrt(pow((goalPoint[k].x - goalPoint[l].x), 2.0) + pow((goalPoint[k].y - goalPoint[l].y), 2.0));
			sg_len = sqrt(pow((goalPoint[k].x - startPoint[l].x), 2.0) + pow((goalPoint[k].y - startPoint[l].y), 2.0));
			//std::cout << "始点同士距離 = "<< start_len << std::endl;
			//std::cout << "終点同士距離 = "<< goal_len << std::endl;
			if ((((0 < start_len && start_len < 75) && (0 < goal_len && goal_len < 75)) || (sg_len < 70) && (tilt_box[l] / tilt_box[k]) < 1.2
				&& (tilt_box[l] / tilt_box[k]) > 0.8) && (b[k] - b[l] < 30 || b[l] - b[k] < 30)) {
				//vote++;
				check[k] = 1;
			}
		}
	}

	count = 0;
	std::cout << "-----------線引き--------------" << std::endl;

	double sort_tilt[NUM] = { 0 }, sort_b[NUM] = { 0 };
	cv::Point sort_start[NUM] = { 0 }, sort_goal[NUM] = { 0 };

	for (int k = 0; k < i; k++) {
		if (check[k] == 0) {
			std::cout << "[傾き 切片 始点　終点] = [" << tilt_box[k] << "," << b[k] << "," << startPoint[k] << "," << goalPoint[k] << "]" << std::endl;
			cv::line(choose_img, startPoint[k], goalPoint[k], cv::Scalar(255, 0, 0), 1, CV_AA);
			sort_tilt[count] = tilt_box[k];
			sort_b[count] = b[k];
			sort_start[count] = startPoint[k];
			sort_goal[count] = goalPoint[k];
			count++;
		}
	}

	std::cout << "残った線" << std::endl;
	for (k = 0; k < count; k++) {
		std::cout << "[傾き 切片 始点　終点] = [" << sort_tilt[k] << "," << sort_b[k] << "," << sort_start[k] << "," << sort_goal[k] << "]" << std::endl;
	}

	/*	正負が同じもので傾きが似たものを探して計算して線を引く	*/
	/*	線を一つ固定し、その線に対して他の線が条件に合うか見ていく	*/

	cv::Point fin_s, fin_g;

	for (k = 0; k < count; k++) {
		for (l = k + 1; l < count; l++) {
			if (sort_tilt[k] / sort_tilt[l] < 2.5 && sort_tilt[k] / sort_tilt[l] > 0.5 && (sort_b[k] * sort_b[l]) > 0) {

				//条件をパスした線の組み合わせに関して始点終点の平均を取る
				fin_s.x = (sort_start[k].x + sort_start[l].x) / 2;
				fin_s.y = (sort_start[k].y + sort_start[l].y) / 2;

				fin_g.x = ((sort_goal[k].x + sort_goal[l].x)) / 2;
				fin_g.y = ((sort_goal[k].y + sort_goal[l].y)) / 2;

				//線を引く
				cv::line(fin_img, fin_s, fin_g, cv::Scalar(0, 0, 255), 1, CV_AA);
			}
		}
	}

	//結果の描画
	cv::namedWindow("HoughLinesP", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("HoughLinesP", dst_img);

	cv::namedWindow("sort", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("sort", sort_img);

	cv::namedWindow("choose", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("choose", choose_img);

	cv::namedWindow("fin", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("fin", fin_img);

}

void FindCircles() {

	//画像を格納する変数の用意
	cv::Mat src_img = cv::imread("image/test9.png", 1);
	cv::Mat src_gray;
	cv::Mat dst_img, work_img, diff_img, np_img, gray_img, mask_img, result_img, final_img;

	mask_img = cv::Scalar(0, 0, 0);
	dst_img = src_img.clone();
	fin_img = src_img.clone();
	result_img = src_img.clone();
	final_img = src_img.clone();

	//結果を描画するために真っ白な画像の用意
	dst_img = cv::Scalar(255, 255, 255);
	result_img = cv::Scalar(255, 255, 255);
	final_img = cv::Scalar(255, 255, 255);

	//inpaint関数やハフ変換を行うために画像をグレースケールにする
	cv::cvtColor(src_img, src_gray, CV_RGB2GRAY);
	cv::cvtColor(src_img, work_img, CV_BGR2GRAY);

	//カウント、半径、中心座標を格納する変数
	int i = 0, k = 0, l = 0, m = 0;
	int rad_box[NUM] = { 0 }, rad_box2[NUM] = { 0 };
	cv::Point cen_box[NUM] = { 0 }, cen_box2[NUM] = { 0 };

	// Hough変換のための前処理（画像の平滑化を行なわないと誤検出が発生しやすい）
	cv::GaussianBlur(work_img, work_img, cv::Size(11, 11), 3, 3);

	// Hough変換による円の検出と検出した円の描画
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(work_img, circles, CV_HOUGH_GRADIENT, 1, 5, 40, 100, 20, 100);

	std::vector<cv::Vec3f>::iterator it = circles.begin();
	for (; it != circles.end(); ++it) {
		cv::Point center(cv::saturate_cast<int>((*it)[0]), cv::saturate_cast<int>((*it)[1]));
		int radius = cv::saturate_cast<int>((*it)[2]);

		//cv::circle(dst_img, center, 2, cv::Scalar(255,0,0), -1, 8);

		cv::circle(dst_img, center, radius, cv::Scalar(0, 0, 255), 8, 8, 0);

		//半径と中心座標を格納
		rad_box[i] = radius;
		cen_box[i].x = center.x;
		cen_box[i].y = center.y;

		//検出個数のカウント
		i++;
	}

	//検出した円の半径と中心座標を表示
	for (k = 0; k < i; k++) {
		std::cout << "[半径,中心] = {" << rad_box[k] << "," << cen_box[k] << "}" << std::endl;
	}

	//inpaint関数を用いるためにグレースケール変換
	cv::cvtColor(dst_img, gray_img, CV_RGB2GRAY);

	//マスク処理を行うためにネガポジ反転をしておく
	np_img = ~gray_img;

	//不要な円を削除
	cv::inpaint(src_gray, np_img, fin_img, 15, cv::INPAINT_NS);

	//円検出
	cv::HoughCircles(fin_img, circles, CV_HOUGH_GRADIENT, 1, 5, 30, 90, 1, 100);

	std::vector<cv::Vec3f>::iterator it2 = circles.begin();
	for (; it2 != circles.end(); ++it2) {
		cv::Point center2(cv::saturate_cast<int>((*it2)[0]), cv::saturate_cast<int>((*it2)[1]));
		int radius2 = cv::saturate_cast<int>((*it2)[2]);

		cv::circle(result_img, center2, radius2, cv::Scalar(0, 0, 255), 8, 8, 0);

		//検出した円の半径と中心座標を格納する
		rad_box2[l] = radius2;
		cen_box2[l].x = center2.x;
		cen_box2[l].y = center2.y;

		//検出個数のカウント
		l++;
	}

	//見やすいように仕切りを作る
	std::cout << "------------------------------------------------" << std::endl;

	//同心円であるかを判定し、同心円なら半径と中心座標を表示し中心座標を描画
	for (k = 0; k < i; k++) {
		for (m = 0; m < l; m++) {
			if (cen_box[k].x - cen_box2[m].x <= 5 && cen_box[k].y - cen_box2[m].y <= 5) {
				std::cout << "[半径,中心] = {" << rad_box2[k] << "," << cen_box2[k] << "}" << std::endl;
				cv::circle(final_img, cen_box2[m], 1, cv::Scalar(0, 0, 0), 8, 8, 0);
			}
		}
	}

	/*画像の表示*/
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