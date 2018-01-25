#include<iostream>    
#include<opencv2/opencv.hpp>    
#include "angle.h"

int main()
{
	AngleProc* angleProc = AngleProc::Create();
	double angle;

	angleProc->Open(0, "out_camera_data.xml");
	angleProc->Angle(angle);
}


