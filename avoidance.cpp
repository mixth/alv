#include </usr/include/opencv2/imgproc/imgproc.hpp>
#include </usr/include/opencv2/core/core.hpp>
#include </usr/include/opencv2/opencv.hpp>

#include <iostream>
#include <boost/thread.hpp>

#define WIDTH 640
#define HEIGHT 360

using namespace cv;
using namespace std;

class Avoider : public Worker
{
	private:
		double THRESHOLD_SIZE;
		double RANGE_THRESHOLD;
		double _TURN;
		int line_x;
		int _IMU_TURN;


		int *GRAY_MAX, *GRAY_MIN;
		float *angle;
		Mat _255;
		Mat erodeElement;
		Mat dilateElement;
		vector< vector< Point > > contours;
		vector< Point > _aproxPoly;
		vector< Point > edge;
		Rect boundRect;
		Mat *mask;
		Mat src;
		int *avoid_result;
		static boost::mutex locker;
		static bool _sort(Point pt1, Point pt2) { return (pt1.x < pt2.x); }
		void thresh_callback(){
			findContours(src, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
			edge.push_back(Point(0, 0));
			for (int i = 0; i < contours.size(); i++){
				if (contours[i].size() < THRESHOLD_SIZE)continue;
				approxPolyDP(Mat(contours[i]), _aproxPoly, 3, true);
				boundRect = boundingRect(_aproxPoly);
				edge.push_back(Point(boundRect.x, boundRect.x + boundRect.width));
			}
			sort(edge.begin(), edge.end(), _sort);
			edge.push_back(Point(WIDTH, WIDTH));
			for (int i = 1; i < edge.size(); i++){
				if (edge[i - 1].y>edge[i].x){
					edge[i - 1].y = (edge[i - 1].y>edge[i].y) ? edge[i - 1].y : edge[i].y;
					edge.erase(edge.begin() + i);
					i--;
				}
			}
			contours.clear();
		}

		int findRange(){
			double min = WIDTH, temp;
			if (edge.size() == 2){
				//call IMU here
				edge.clear();
				float current_angle = main_compass();
				cout << "IMU " << current_angle << endl;
				//cout << "angle " << *angle << endl;
				int right, left;
				if (current_angle<0)
					current_angle = 360 + current_angle;
				if ((*angle)<current_angle){
					left = 360 - current_angle + (*angle);
					right = current_angle - (*angle);
				}
				else{
					left = (*angle) - current_angle;
					right = 360 - (*angle) + current_angle;
				}
				if (right <= left && right >= _IMU_TURN)
					return 4;
				else if (left<right && left >= _IMU_TURN)
					return 5;
				else
					return 1;
			}
			else{
				for (int i = 0; i < edge.size() - 1; i++){
					if (edge[i + 1].x - edge[i].y<RANGE_THRESHOLD)
						continue;
					temp = (edge[i].y + edge[i + 1].x - WIDTH) / 2;
					if (abs(min)>abs(temp))
						min = temp;
				}
			}
			edge.clear();
			printf("%d ",(int)min);
			if (min == WIDTH) return 6;//stop
			if (-_TURN<min && min<_TURN)return 1;//straingth
			if (_TURN <= min)return 4;//right
			if (min <= -_TURN)return 5;//left

		}
		
	public:
		Avoider(Mat *_mask, Mat *_src, int *_avoid_result, int *_GRAY_MIN, int *_GRAY_MAX, float *_angle)
		{
			THRESHOLD_SIZE = 50;//size of contour
			RANGE_THRESHOLD = 100;//distance between edge of contour
			_TURN = 75; //center of image
			line_x = HEIGHT / 2;
			_IMU_TURN = 10; //center of compass
			_255 = Mat::ones(line_x, WIDTH, CV_8U) * 255;
			erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
			dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));
			src = *_src;
			mask = _mask;
			avoid_result = _avoid_result;
			GRAY_MIN = _GRAY_MIN;
			GRAY_MAX = _GRAY_MAX;
			angle = _angle;
		}
		void operator () ()
		{
			//threshold(mask->rowRange(mask->rows - line_x, mask->rows), src, GRAY_MIN, GRAY_MAX, THRESH_BINARY);
			/*<morphOps()>*/
			//erode(src, src, erodeElement);
			while (true)
			{

				_locker.lock();
				threshold(mask->rowRange(mask->rows - line_x, mask->rows), src, *GRAY_MIN, *GRAY_MAX, THRESH_BINARY);
				_locker.unlock();
				
				/*<morphOps()>*/
				erode(src, src, erodeElement);
				erode(src, src, erodeElement);
				dilate(src, src, dilateElement);
				dilate(src, src, dilateElement);
				/*</morphOps()>*/
				src = _255 - src;

				thresh_callback();
				*avoid_result = findRange();
				cout << "avoid_result = " << *avoid_result << endl;
			}
		}
};
