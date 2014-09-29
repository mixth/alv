#include </usr/include/opencv2/imgproc/imgproc.hpp>
#include </usr/include/opencv2/core/core.hpp>
#include </usr/include/opencv2/opencv.hpp>
#include <iostream>
#include "test.cpp"
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

class Symbol {

public:
	Mat img;
	string name;

};

Symbol symbols;

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

void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center) {
	std::vector<cv::Point2f> top, bot;

	for (unsigned int i = 0; i < corners.size(); i++) {
		if (corners[i].y < center.y)
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}

	cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
	cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
	cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
	cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

	corners.clear();
	corners.push_back(tl);
	corners.push_back(tr);
	corners.push_back(br);
	corners.push_back(bl);
}

int readRefImages() {

	symbols.img = imread("alv.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!symbols.img.data)
		return -1;
	threshold(symbols.img, symbols.img, 100, 255, 0);
	symbols.name = "ALV";
	return 0;
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

void CannyThreshold(int, void*) {}

void signDetection(Mat input)
{
	Mat new_image;
	Mat greyImg;
	input.copyTo(greyImg);
	//test_save_image(greyImg, 1);


	int lowThreshold;
	//createTrackbar("Min Threshold:", "A", &lowThreshold, 100, CannyThreshold);
	lowThreshold = 25;
	//threshold(greyImg, greyImg, 100, 255, 0);

	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	GaussianBlur(greyImg, greyImg, Size(9, 9), 2, 2);

	//test_save_image(greyImg, 2);
	/// Detect edges using canny
	Canny(greyImg, canny_output, lowThreshold, lowThreshold*3, 3);
	//test_save_image(canny_output, 3);

	//	imshow("B",canny_output);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE,
			CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<Point> approxRect;

	for (size_t i = 0; i < contours.size(); i++) {
		approxPolyDP(contours[i], approxRect,
				arcLength(Mat(contours[i]), true) * 0.05, true);
		if (approxRect.size() == 4) {
			float area = contourArea(contours[i]);

			if (area > 2500) {
				cout << "Found reasonable size of approxPolyDP" << endl;
				std::vector<cv::Point2f> corners;

				vector<Point>::iterator vertex;
				vertex = approxRect.begin();
				//vertex++;
				//circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);
				vertex++;
				//circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);
				vertex++;
				//circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);
				vertex++;
				//circle(camera, *vertex, 2, Scalar(0, 0, 255), -1, 8, 0);
				corners.push_back(*vertex);

				Moments mu;
				mu = moments(contours[i], false);
				Point2f center(mu.m10 / mu.m00, mu.m01 / mu.m00);

				sortCorners(corners, center);

				// Define the destination image
				Mat correctedImg = ::Mat::zeros(200, 200, CV_8UC3);

				// Corners of the destination image
				std::vector<cv::Point2f> quad_pts;
				quad_pts.push_back(Point2f(0, 0));
				quad_pts.push_back(Point2f(correctedImg.cols, 0));
				quad_pts.push_back(
						Point2f(correctedImg.cols, correctedImg.rows));
				quad_pts.push_back(Point2f(0, correctedImg.rows));

				// Get transformation matrix
				Mat transmtx = getPerspectiveTransform(corners, quad_pts);

				// Apply perspective transformation
				warpPerspective(input, correctedImg, transmtx,
						correctedImg.size());

				Mat correctedImgBin;
				correctedImg.copyTo(correctedImgBin);

				//cvtColor(correctedImg, correctedImgBin, CV_RGB2GRAY);
				
				//equalizeHist(correctedImgBin, correctedImgBin);

				correctedImgBin.copyTo(new_image);

				threshold(correctedImgBin, correctedImgBin, 140, 255, 0);

				//imshow("B", correctedImgBin);

				double minVal,maxVal,medVal;


				minMaxLoc(new_image, &minVal, &maxVal);

				medVal=(maxVal-minVal)/2;

				threshold(new_image, new_image, medVal, 255, 0);

				//imshow("C", new_image);


				Mat diffImg;

				int match, minDiff, diff;
				minDiff = 12000;

					//diffImg = symbols.img-correctedImgBin;

					bitwise_xor(new_image, symbols.img, diffImg,
							noArray());

					diff = countNonZero(diffImg);
					if (diff < minDiff) {
						cout << "Matched";
					}

				//break;

			}

		}
	}	
}
int MyProject(IplImage *y){
	mask = Mat(y);
	if(_setup)
        if(setup(mask)==-200)
            return -1;
    signDetection(mask.rowRange(0, mask.rows-line_x));
    threshold(mask.rowRange(mask.rows-line_x,mask.rows), src, GRAY_MIN, GRAY_MAX, THRESH_BINARY);
    /*<morphOps()>*/
    erode(src, src, erodeElement);
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
