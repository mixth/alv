#include <cv.h>
#include <highgui.h>

#include "source.h"
#include "RaspiCamCV.h"

int main ()
{
	RaspiCamCvCapture *capture = raspiCamCvCreateCameraCapture(0);
	//cvNamedWindow("RaspiCamTest", 1);
	do
	{
		IplImage *image = raspiCamCvQueryFrame(capture);
		//cvShowImage("RaspiCamTest", image);
		if(MyProject(image) < 0)
			break;
	} while (1);
	//cvDestroyWindow("RaspiCamTest");
	raspiCamCvReleaseCapture(&capture);
	return 0;
}
