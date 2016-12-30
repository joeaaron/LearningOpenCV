#include "SeqCapture.h"

using namespace std;

int main(void)
{
	string path = "G:\\JA\\demo\\MetalFrameDetection\\MachineVision\\MachineVision\\pictures";

	CSeqCapture cap(path, "jpg");

	Mat img;
	while (cap >> img)
	{
		imshow("IMG", img);
		waitKey(3000);
	}

	waitKey(0);
	return 0;
}