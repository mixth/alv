#include </usr/include/opencv2/imgproc/imgproc.hpp>
#include </usr/include/opencv2/core/core.hpp>
#include </usr/include/opencv2/opencv.hpp>
#include <iostream>

#include <boost/thread.hpp>
#include "source.h"
#include "test.cpp"
#include "worker/worker.cpp"
#include "worker/writer.cpp"
#include "worker/avoider.cpp"
#include "worker/controller.cpp"
#include "worker/follower.cpp"

#define WIDTH 640
#define HEIGHT 360

const int line_x = HEIGHT / 2;

using namespace cv;
using namespace std;
static bool _setup = true;
int GRAY_MIN;
int avoid_result = -1;
int follow_result = -1;
int GRAY_MAX = 255;

Mat src, mask;
float angle;

boost::mutex
Worker::_locker;

Avoider avoid(&mask, &src, &avoid_result, &GRAY_MIN, &GRAY_MAX, &angle);
Follower follow(&mask, &follow_result);
Controller control(&avoid_result, &follow_result);

int setup(Mat src){
	int a[256] = {};
	int max = 0;
	int index = 0;
	_setup = false;
	angle = main_compass();
	if (angle<0)
		angle = 360 + angle;
	src = src.rowRange(src.rows - line_x, src.rows);
	for (int i = 0; i < src.rows*src.cols; i++){
		a[(int)src.data[i]]++;
	}
	for (int i = 0; i< 256; i++){
		if (a[i] >= max){
			max = a[i];
			index = i;
		}
	}
	//	GRAY_MIN = 100;
	GRAY_MIN = index - 30;
	printf("angle : %.2f\n", angle);
	printf("GRAY_MIN : %d\n", GRAY_MIN);

	return angle;
}

int MyProject(IplImage *y){
	Writer write(y, &mask);
	boost::thread writeThread (write);
	writeThread.join();
	
	if (_setup)
	{
		if (setup(mask) == -200)
			return -1;
		boost::thread avoidThread (avoid);
		boost::thread controlThread (control);
		boost::thread followThread (follow);
		cout << "fired avoidThread, controlThread, followThread" << endl;
	}
}
