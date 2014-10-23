#include </usr/include/opencv2/imgproc/imgproc.hpp>
#include </usr/include/opencv2/core/core.hpp>
#include </usr/include/opencv2/opencv.hpp>
#include <iostream>
#include "source.h"
#define WIDTH 640
#define HEIGHT 360

using namespace cv;
using namespace std;

int GRAY_MIN;
const int GRAY_MAX = 255;
const double THRESHOLD_SIZE = 50;//size of contour
const double RANGE_THRESHOLD = 100;//distance between edge of contour
const double _TURN = 75; //center of image
const int line_x = HEIGHT / 2;
const int _IMU_TURN = 10; //center of compass
static bool _setup = true;
static float angle;
Mat src,mask;
static Mat _255 = Mat::ones(line_x,WIDTH, CV_8U)*255;
static Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
static Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));
vector< vector< Point > > contours;
vector< Point > _aproxPoly;
vector< Point > edge;
Rect boundRect;

static bool _sort (Point pt1, Point pt2) { return (pt1.x < pt2.x);}
void thresh_callback(){
    findContours(src, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    edge.push_back(Point(0,0));
    for (int i = 0; i < contours.size(); i++){
        if (contours[i].size() < THRESHOLD_SIZE)continue;
        approxPolyDP(Mat(contours[i]), _aproxPoly, 3, true);
        boundRect = boundingRect(_aproxPoly);
        edge.push_back(Point(boundRect.x,boundRect.x+boundRect.width));
    }
    sort(edge.begin(),edge.end(),_sort);
    edge.push_back(Point(WIDTH,WIDTH));
    for (int i = 1; i < edge.size(); i++){
        if (edge[i-1].y>edge[i].x){
            edge[i-1].y = (edge[i-1].y>edge[i].y)? edge[i-1].y:edge[i].y;
            edge.erase(edge.begin()+i);
            i--;
        }
    }
    contours.clear();
}
int findRange(){
    double min=WIDTH,temp;
    if (edge.size()==2){
        //call IMU here
        edge.clear();
        float current_angle = main_compass();
printf("IMU %5.2f",current_angle);
        int right,left;
        if (current_angle<0)
            current_angle=360+current_angle;
        if (angle<current_angle){
        	left = 360 - current_angle +angle;
        	right = current_angle - angle;
        }
        else{
        	left = angle - current_angle;
        	right = 360 - angle +current_angle;
        }
        if (right<=left && right>=_IMU_TURN)
        	return 4;
        else if(left<right && left>=_IMU_TURN)
        	return 5;
        else
        	return 1;
    }
    else{
    	for (int i = 0; i < edge.size()-1; i++){
    		if (edge[i+1].x-edge[i].y<RANGE_THRESHOLD)
    			continue;
            temp=(edge[i].y+edge[i+1].x-WIDTH)/2;
            if (abs(min)>abs(temp))
                min=temp;
        }
    }
    edge.clear();
//    printf("%d ",(int)min);
    if (min==WIDTH) return 6;//stop
    if (-_TURN<min && min<_TURN)return 1;//straingth
    if (_TURN<=min)return 4;//right
    if (min<=-_TURN)return 5;//left

}
int setup(Mat src){
	int a[256]={};
	int max=0;
	int index=0;
	_setup=false;
	angle=main_compass();
	if (angle<0)
		angle=360+angle;
	src = src.rowRange(src.rows-line_x,src.rows);
	for (int i =0;i < src.rows*src.cols;i++){
		a[(int)src.data[i]]++;
	}
	for(int i =0;i< 256;i++){
		if(a[i]>=max){
			max = a[i];
			index = i;
		}
	}
//	GRAY_MIN = 100;
	GRAY_MIN = index-30;
	printf("angle : %.2f\n", angle);
	printf("GRAY_MIN : %d\n", GRAY_MIN);
	return angle;
}

void colorDetection(Mat *imgOriginal)
{
	Mat imgHSV;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);
	Mat Thresholded;
	inRange(imgHSV, Scalar(LowH, LowS, LowV), Scalar(HighH, HighS, HighV), Thresholded);
	Mat erodeDilateElement = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	
	erode(Thresholded, Thresholded, erodeDilateElement);
	dilate(Thresholded, Thresholded, erodeDilateElement);


	dilate(Thresholded, Thresholded, erodeDilateElement);
	erode(Thresholded, Thresholded, erodeDilateElement);

	Moments oMoments = moments(Thresholded);
	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	if (dArea > 10000)
	{
	        int posX = dM10 / dArea;
	        int posY = dM01 / dArea;

                int error_gap = 100;
                Size imgSize = imgThresholded.size();
                if (posX > ((imgSize.width/2) + error_gap/2))
        		cout << "color: right" << endl;
                else if (posX < ((imgSize.width/2) + error_gap/2) && posX > ((imgSize.width/2) - error_gap/2))
                        cout << "color: stright" << endl;
                else
                        cout << "color: left" << endl;
	}

	imgHSV.release();
	Thresholded.release();
}
int MyProject(IplImage *y){
	mask = Mat(y);
	if(_setup)
        if(setup(mask)==-200)
            return -1;
    colorDetection(mask.rowRange(0, mask.rows-line_x);
    threshold(mask.rowRange(mask.rows-line_x,mask.rows), src, GRAY_MIN, GRAY_MAX, THRESH_BINARY);
    /*<morphOps()>*/
    erode(src, src, erodeElement);
    erode(src, src, erodeElement);
    dilate(src, src, dilateElement);
    dilate(src, src, dilateElement);
    /*</morphOps()>*/
    src = _255 - src;
	thresh_callback();
    if(main_control(findRange())<0)
	return -2;
	src.release();
    return 0;
}
