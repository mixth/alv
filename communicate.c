#include </usr/local/include/wiringPi.h>
#include "source.h"

/********************************************
*       input pin                           *
* MSB >> 7     0 << LSB                     *
*		                            * 
*       output pin                          *
* MSB >> 2     3 << LSB	                    *
* 				            *
* 00 - error state                          *
* 01 - stright                              *
* 10 - right/left (depends on camera setup) *
* 11 - unknown result                       *
* 					    *
********************************************/

static int _setup = 1;

int communicate_setup () 
{
	if (wiringPiSetup() == -1)
		return -1;
		
	// For input communication
	pinMode(7, INPUT);
	pinMode(0, INPUT);
	
	// For output communication
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	
	_setup = 1;
}

int readInput ()
{
	if (!_setup)
	{
			if (communicate_setup() == -1)
				return -1;
	}
	
	int msb = digitalRead(7);
	int lsb = digitalRead(0);
	
	if (msb == 0 && lsb == 0)
		return 0;
	else if (msb == 0 && lsb == 1)
		return 1;
	else if (msb == 1 && lsb == 0)
		return 2;
	else
		return 3;
}

int sendOutput (int output)
{
	if (!_setup)
	{
			if (setup() == -1)
				return -1;
	}
	
	switch (output)
	{
			case 1:
				digitalWrite(2, 0);
				digitalWrite(3, 1);
				break;
			case 2:
				digitalWrite(2, 1);
				digitalWrite(3, 0);
				break;
			case 3:
				digitalWrite(2, 1);
				digitalWrite(3, 1);
				break;
			default:
				// Error state or else
				digitalWrite(2, 0);
				digitalWrite(3, 0);
				break;			
	}
	
	return 0;
}
