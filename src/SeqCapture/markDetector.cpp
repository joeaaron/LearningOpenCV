#include "markDetector.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include "Marker.h"

using namespace std;
using namespace cv;

#define FLT_MAX 3.402823466e+38F

#define TEST 1          //0:RUN�� 1��DEBUG

void findMarkerContours(const Mat& _imgThreshold, vector<vector<Point> >& _contours, int _minContourPointsAllowed)
{
	Mat imgTemp = _imgThreshold.clone();
	vector<Vec4i>hierarchy;
	vector<vector<Point>> allContours;
	findContours(imgTemp, allContours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);

	_contours.clear();
	for (size_t i = 0; i < allContours.size(); i++)
	{
		int contourSize = allContours[i].size();
		if (contourSize > _minContourPointsAllowed)
		{
			_contours.push_back(allContours[i]);
		}
	}
}

void findMarkerCandidates(const vector<vector<Point> >& _contours, vector<Marker>& _detectedMarkers)
{
	vector<Point> approxCurve;
	vector<Marker> markerPossible;
	float m_minContourLengthAllowed = 100.0f;

	for (size_t i = 0; i < _contours.size(); i++)
	{
		approxPolyDP(_contours[i], approxCurve, double(_contours[i].size())*0.05, true);
		if (approxCurve.size() != 4)
			continue;

		if (!isContourConvex(approxCurve))
			continue;
		float minDist = FLT_MAX;
		for (int i = 0; i < 4; i++)
		{
			Point vecTemp = approxCurve[i] - approxCurve[(i + 1) % 4];
			float distSquared = vecTemp.dot(vecTemp);
			minDist = std::min(minDist, distSquared);
		}

		if (minDist > m_minContourLengthAllowed)
		{
			Marker markerTemp;
			for (int i = 0; i < 4; i++)
			{
				markerTemp.m_points.push_back(Point2f(approxCurve[i].x, approxCurve[i].y));
			}
			markerPossible.push_back(markerTemp);
		}
	}

	for (size_t i = 0; i < markerPossible.size(); i++)
	{
		Point v1 = markerPossible[i].m_points[1] - markerPossible[i].m_points[0];
		Point v2 = markerPossible[i].m_points[2] - markerPossible[i].m_points[0];
		double theta = (v1.x * v2.y) - (v1.y * v2.x);

		if (theta < 0.0)
		{
			std::swap(markerPossible[i].m_points[1], markerPossible[i].m_points[3]);
		}
	}

	vector<pair<int, int> > tooNearCandidates;
	for (size_t i = 0; i < markerPossible.size(); i++)
	{
		for (size_t j = i + 1; j < markerPossible.size(); j++)
		{
			float distSquared = 0.0f;
			for (int k = 0; k < 4; k++)
			{
				Point vec = markerPossible[i].m_points[k] - markerPossible[j].m_points[k];
				distSquared += vec.dot(vec);
			}
			if (distSquared < 400)
			{
				tooNearCandidates.push_back(pair<int, int>(i, j));
			}
		}
	}

	vector<bool> markerRemoveIndex(markerPossible.size(), false);
	for (size_t i = 0; i < tooNearCandidates.size(); i++)
	{
		float length1 = markerPossible[tooNearCandidates[i].first].calPerimeter();
		float length2 = markerPossible[tooNearCandidates[i].second].calPerimeter();
		markerRemoveIndex[(length1 > length2) ? tooNearCandidates[i].second : tooNearCandidates[i].first] = true;
	}

	_detectedMarkers.clear();
	for (size_t i = 0; i < markerPossible.size(); i++)
	{
		if (!markerRemoveIndex[i])
		{
			_detectedMarkers.push_back(markerPossible[i]);
		}
	}
}

void detectMarkers(const Mat& _imgGray, vector<Marker>& _detectedMarkers)
{
	Mat canonicalImg;
	vector<Marker> goodMarkers;
	vector<Point2f> m_markerCorners2d;
	Size m_markerSize = Size(100, 100);

	m_markerCorners2d.push_back(Point2f(0, 0));
	m_markerCorners2d.push_back(Point2f(m_markerSize.width - 1, 0));
	m_markerCorners2d.push_back(Point2f(m_markerSize.width - 1, m_markerSize.height - 1));
	m_markerCorners2d.push_back(Point2f(0, m_markerSize.height - 1));

	for (size_t i = 0; i < _detectedMarkers.size(); i++)
	{
		Mat M = getPerspectiveTransform(_detectedMarkers[i].m_points, m_markerCorners2d);
		warpPerspective(_imgGray, canonicalImg, M, m_markerSize);
	
		int nRotations;
		int idMarker = Marker::decode(canonicalImg, nRotations);
		if (idMarker != -1)
		{
			_detectedMarkers[i].m_id = idMarker;
			
			std::rotate(_detectedMarkers[i].m_points.begin(), _detectedMarkers[i].m_points.begin() + 4 - nRotations, _detectedMarkers[i].m_points.end());
			goodMarkers.push_back(_detectedMarkers[i]);
		}
	}
	
	if (goodMarkers.size() > 0)
	{
		vector<Point2f> preciseCorners(4 * goodMarkers.size());
		for (size_t i = 0; i < goodMarkers.size(); i++)
		{
			for (int j = 0; j < 4; j++)
			{
				preciseCorners[4 * i + j] = goodMarkers[i].m_points[j];
			}
		}

		cornerSubPix(_imgGray, preciseCorners, Size(5, 5), Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER, 30, 0.1));

		for (size_t i = 0; i < goodMarkers.size(); i++)
		{
			for (int j = 0; j < 4; j++)
			{
				goodMarkers[i].m_points[j] = preciseCorners[4 * i + j];
			}
		}
	}
	_detectedMarkers = goodMarkers;

}

void estimatePosition(vector<Marker>& _detectedMarkers)
{
	vector<Point3f> m_markerCorners3d;
	Mat m_camMat;
	Mat m_distCoeff;

	m_camMat = (Mat_<float>(3, 3) << 0, 0, 0,
		0, 0, 0,
		0, 0, 0);
	m_distCoeff = (Mat_<float>(4, 1) << 0, 0, 0, 0);

	m_markerCorners3d.push_back(cv::Point3f(-0.5f, -0.5f, 0));
	m_markerCorners3d.push_back(cv::Point3f(+0.5f, -0.5f, 0));
	m_markerCorners3d.push_back(cv::Point3f(+0.5f, +0.5f, 0));
	m_markerCorners3d.push_back(cv::Point3f(-0.5f, +0.5f, 0));

	for (size_t i = 0; i < _detectedMarkers.size(); i++)
	{
		Mat Rvec;
		Vec3f Tvec;
		Mat raux, taux;
		solvePnP(m_markerCorners3d, _detectedMarkers[i].m_points, m_camMat, m_distCoeff, raux, taux);
		raux.convertTo(Rvec, CV_32F);
		taux.convertTo(Tvec, CV_32F);

		// 		Mat_<float> rotMat(3,3);
		Matx33f rotMat;
		Rodrigues(Rvec, rotMat);

		_detectedMarkers[i].m_rotation = rotMat.t();
		_detectedMarkers[i].m_translation = -Tvec;
	}
}

bool findMarkers(const Mat& _frame, vector<Marker>& _detectedMarkers)
{
	Mat m_imgGray;
	Mat m_imgThreshold;
	vector<vector<Point> > m_contours;

	cvtColor(_frame, m_imgGray, CV_BGR2GRAY);
	threshold(m_imgGray, m_imgThreshold, 128, 255, cv::THRESH_BINARY_INV);

#if TEST
	imshow("threshold", m_imgThreshold);
	adaptiveThreshold(m_imgGray, m_imgThreshold, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 7, 7);
	imshow("threshold", m_imgThreshold);
#endif

	findMarkerContours(m_imgThreshold, m_contours, m_imgGray.cols / 4);

#if TEST
	vector<Vec4i> hierarchy;
	Mat contourImg = Mat::zeros(_frame.size(), CV_8UC3);
	for(int i=0; i<m_contours.size(); i++)
	{
	 	drawContours(contourImg, m_contours, i, Scalar(255,255,255), 2, 8, hierarchy, 0, Point());
	}
	imshow("contours", contourImg);
#endif

	findMarkerCandidates(m_contours, _detectedMarkers);

#if TEST
	Mat markerCandidateImg = Mat::zeros(_frame.size(), CV_8UC3);
	for (int i = 0; i < _detectedMarkers.size(); i++)
	{
		int sizeNum = _detectedMarkers[i].m_points.size();
		for (int j = 0; j < sizeNum; j++)
		{
			line(markerCandidateImg, _detectedMarkers[i].m_points[j], _detectedMarkers[i].m_points[(j + 1) % sizeNum], Scalar(255, 255, 255), 2, 8);
		}
	}
	imshow("markerCandidate", markerCandidateImg);
#endif

	detectMarkers(m_imgGray, _detectedMarkers);
#if TEST
	Mat markerImg = Mat::zeros(_frame.size(), CV_8UC3);
	for (int i = 0; i < _detectedMarkers.size(); i++)
	{
		int sizeNum = _detectedMarkers[i].m_points.size();
		for (int j = 0; j < sizeNum; j++)
		{
			line(markerImg, _detectedMarkers[i].m_points[j], _detectedMarkers[i].m_points[(j + 1) % sizeNum], Scalar(255, 255, 255), 2, 8);
		}
	}
	imshow("marker", markerImg);
#endif
	estimatePosition(_detectedMarkers);

	return false;
}

class mark_t : public MarkProc
{
//private:
//	vector<Marker> m_markers;
	
public:
	mark_t(){}

	void ProcessFrame(const Mat& _frame, vector<Marker>& markers)
	{
		markers.clear(); 
		findMarkers(_frame, markers);
	}

	~mark_t(){}

};

MarkProc* MarkProc::Create()
{
	return new mark_t();
}
