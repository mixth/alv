#include </usr/include/opencv2/core/core.hpp>
#include </usr/include/opencv2/opencv.hpp>
#include </usr/include/opencv2/highgui/highgui.hpp>
#include "source.h"
#include <sstream>

using namespace cv;
using namespace std;

/*int test_save_image (IplImage *p, int count)
{
	Mat out;
	out = Mat(p);
	ostringstream oss;
	oss << "test-" << count << "-image.jpg";
	imwrite(oss.str(), out);
	return 0;
}*/

int test_save_image (Mat p, int count)
{
	Mat out;
	p.copyTo(out);
	ostringstream oss;
	oss << "test-" << count << "-image.jpg";
	imwrite(oss.str(), out);
	return 0;
}

