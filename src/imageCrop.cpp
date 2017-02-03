#include <iostream>  
#include "opencv2/core.hpp"  
#include "opencv2/imgproc.hpp"  
#include "opencv2/highgui.hpp"  
  
using namespace std;  
using namespace cv;  

static string window_name = "Draw a Rect to crop";  
static Mat src;  //源图片  
bool  isDrag = false;  
Point point1; //矩形的第一个点  
Point point2; //矩形的第二个点  

int ImageCrop(InputArray src, OutputArray dst, Rect rect)
{
    Mat input = src.getMat();
    if (input.empty()){
        return -1;
    }

    //计算剪切区域：剪切Rect与源图像所在Rect的交集
    Rect srcRect(0, 0, input.cols, input.rows);
    rect = rect & srcRect;
    if(rect.width <= 0 || rect.height <= 0) return -1;

    //创建结果图像
    dst.create(Size(rect.width, rect.height), src.type());
    Mat output = dst.getMat();
    if(output.empty()) return -1;

    try{
        input(rect).copyTo(output);
        return 0;
    }catch(...){
        return -1;
    }
}

static void callbackMouseEvent(int mouseEvent, int x, int y, int flags, void* param)  
{
    switch(mouseEvent)
    {
        case CV_EVENT_LBUTTONDOWN:
			point1 = Point(x, y);
			point2 = Point(x, y);
			isDrag = true;
			break;
        case CV_EVENT_MOUSEMOVE:
			if (isDrag)
			{
				point2 = Point(x, y);
				Mat dst = src.clone();
				Rect rect(point1, point2);
				rectangle(dst, rect, Scalar(0, 0, 255));
				imshow(window_name, dst);
			}
        break;
        case CV_EVENT_LBUTTONUP:  
			if (isDrag) 
			{  
				isDrag = false;  
				Rect rect (point1, point2);     //得到矩形  
				ImageCrop(src, src, rect);      //图像剪切  
				imshow(window_name, src); 
			}  
			break;  
        
    }
    return;
}

int main()  
{  
    //read image file  
    src = imread("lena.jpg");  
    if ( !src.data ) {  
        cout << "error read image" << endl;  
        return -1;  
    }  
  
    //create window  
    namedWindow(window_name);  
    imshow(window_name, src);  
  
    //set mouse event call back  
    setMouseCallback(window_name, callbackMouseEvent, NULL );  
  
    waitKey();  
  
    return 0;  
  
}  