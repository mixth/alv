#include <boost/thread.hpp>

#define WIDTH 640
#define HEIGHT 360

using namespace std;

class Symbol {

public:
	Mat img;
	string name;

};

class Follower : public Worker
{
	public:
	Follower (Mat *_mask, int *_follow_result, int *_has_controller, int *_left_right, float *_left_const, float *_right_const)
	{
		_setup = true;
		mask = _mask;
		follow_result = _follow_result;
		has_controller = _has_controller;
		left_right = _left_right;
		left_const = _left_const;
		right_const = _right_const;
		line_x = HEIGHT / 2;
		readRefImages();
	}
	void operator () ()
	{
		if (_setup)
		{
			// Since we are using only two Raspi, left one's upperBound = 1, right one's lowerBound = 0
			if (*left_right == 0)
			{
				lowerBound = WIDTH * (*left_const);
				upperBound = WIDTH;
			}
			else if (*left_right == 1)
			{
				lowerBound = 0;
				upperBound = WIDTH * (*right_const);
			}
			else
			{
				cout << "error getting left_right variable" << endl;
			}
			_setup = false;
		}
		while (true)
		{
			_locker.lock();
			Mat temp;
			mask->rowRange(0, mask->rows - line_x).copyTo(temp);
			_locker.unlock();
			//cout << "Follower read from mask" << endl;
			signDetection(temp);
		}
	}
	
	private:
	Mat *mask;
	int *follow_result;
	int *has_controller;
	int *left_right;
	float *right_const, *left_const;
	int line_x;
	bool _setup;
	const int static reasonableArea = 2500;
	const double static matchingRatio = 0.56;
	float lowerBound, upperBound;
	Symbol symbols;
	
	int readRefImages() {
		symbols.img = imread("invader-space-invert-resize.jpg", CV_LOAD_IMAGE_GRAYSCALE);
		if (!symbols.img.data)
			return -1;
		threshold(symbols.img, symbols.img, 100, 255, 0);
		symbols.name = "ALV";
		return 0;
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
		
		int expectedOut = 0;
		
		for (size_t i = 0; i < contours.size(); i++) {
			approxPolyDP(contours[i], approxRect,
					arcLength(Mat(contours[i]), true) * 0.05, true);
			if (approxRect.size() == 4) {
				float area = contourArea(contours[i]);

				if (area > reasonableArea) {
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
						double matching = (double) 1 - ((double) diff / (double) 40000);
						//cout << "matching = " << matching << endl;
						if (matching >= matchingRatio) {
							cout << "Matched" << endl;
							if (center.x <= lowerBound)
							{
								//cout << "Follower decided to go left" << endl;
								expectedOut = 1;
							}
							else if (center.x >= upperBound)
							{
								//cout << "Follower decided to go right" << endl;
								expectedOut = 3;
							}
							else
							{
								//cout << "Follower decided to go straight" << endl;
								expectedOut = 2;
							}
						}

					//break;

				}

			}
		}

		(*follow_result) = expectedOut;
		
		// if this node doesn't has_controller, it will need to send out the result
		if (!(*has_controller))
		{
			int send = 0;
			switch (expectedOut)
			{
				case 0:
					send = 3;
					break;
				case 2:
					send = 1;
					break;
				case 1:
					send = 2;
					break;
				case 3:
					send = 2;
					break;
				default:
					send = 0;
			}
			sendOutput(send);
		}
		/*
		if (expectedOut == 0)
		{
			test_save_image(input, 1);
		}
		*/
	}
};
