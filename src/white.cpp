//C++和OpenCV：聚类白色像素算法
//http://cn.voidcc.com/question/p-dgpyhtea-beh.html
#include <opencv2\opencv.hpp> 
#include <vector> 
#include <algorithm> 

using namespace std;
using namespace cv;

struct EuclideanDistanceFunctor
{
	int _dist2;
	EuclideanDistanceFunctor(int dist) : _dist2(dist*dist) {}

	bool operator()(const Point& lhs, const Point& rhs) const
	{
		return ((lhs.x - rhs.x)*(lhs.x - rhs.x) + (lhs.y - rhs.y)*(lhs.y - rhs.y)) < _dist2;
	}
};

int main()
{
	// Load the image (grayscale) 
	Mat1b img = imread("white.png", IMREAD_GRAYSCALE);

	// Get all non black points 
	vector<Point> pts;
	findNonZero(img, pts);

	// Define the distance between clusters 
	int euclidean_distance = 20;

	// Apply partition 
	// All pixels within the the given distance will belong to the same cluster 

	vector<int> labels;

	// With functor 
	//int n_labels = partition(pts, labels, EuclideanDistanceFunctor(euclidean_distance)); 

	// With lambda function 
	int th2 = euclidean_distance * euclidean_distance;
	int n_labels = partition(pts, labels, [th2](const Point& lhs, const Point& rhs) {
		return ((lhs.x - rhs.x)*(lhs.x - rhs.x) + (lhs.y - rhs.y)*(lhs.y - rhs.y)) < th2;
	});


	// Store all points in same cluster, and compute centroids 
	vector<vector<Point>> clusters(n_labels);
	vector<Point> centroids(n_labels, Point(0, 0));

	for (int i = 0; i < pts.size(); ++i)
	{
		clusters[labels[i]].push_back(pts[i]);
		centroids[labels[i]] += pts[i];
	}
	for (int i = 0; i < n_labels; ++i)
	{
		centroids[i].x /= clusters[i].size();
		centroids[i].y /= clusters[i].size();
	}

	// Draw results 

	// Build a vector of random color, one for each class (label) 
	vector<Vec3b> colors;
	for (int i = 0; i < n_labels; ++i)
	{
		colors.push_back(Vec3b(rand() & 255, rand() & 255, rand() & 255));
	}

	// Draw the points 
	Mat3b res(img.rows, img.cols, Vec3b(0, 0, 0));
	for (int i = 0; i < pts.size(); ++i)
	{
		res(pts[i]) = colors[labels[i]];
	}

	// Draw centroids 
	for (int i = 0; i < n_labels; ++i)
	{
		circle(res, centroids[i], 3, Scalar(colors[i][0], colors[i][1], colors[i][2]), CV_FILLED);
		circle(res, centroids[i], 6, Scalar(255 - colors[i][0], 255 - colors[i][1], 255 - colors[i][2]));
	}


	imshow("Clusters", res);
	waitKey();

	return 0;
}