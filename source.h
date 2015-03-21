#include <stdlib.h>
#include <stdio.h>
#include <cv.h>
#ifndef HEADER_FILE
#define HEADER_FILE
#ifdef __cplusplus
extern "C" {
#endif
	int MyProject(IplImage *rgb);
	void thresh_callback();
	int main_control(int input, int mode);
	float main_compass();
	int readInput ();
	int sendOutput (int output);
#ifdef __cplusplus
}
#endif
#endif
