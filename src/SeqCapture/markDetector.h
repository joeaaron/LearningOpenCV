#ifndef ZOOMLION_MARKER_H
#define ZOOMLION_MARKER_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "Marker.h"

using namespace std;

class MarkProc
{
public:
	virtual void ProcessFrame(const cv::Mat& _frame, vector<Marker>& markers) = 0;
	static MarkProc* Create();
};
#endif