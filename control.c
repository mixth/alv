#include </usr/local/include/wiringPi.h>
#include "source.h"
#define _delay 3
/*
void control(int left, int right){
        if (left==right){
                switch(left){
                case  10:digitalWrite(1,1);digitalWrite(4,0);digitalWrite(5,1);digitalWrite(6,0);break;
                case -10:digitalWrite(1,0);digitalWrite(4,1);digitalWrite(5,0);digitalWrite(6,1);break;
                }
                return;
        }
        if (left==10||left==-10){
                digitalWrite(1,(left==10)? 1:0);digitalWrite(4,(left==10)? 0:1);
                if (right==0){
                        digitalWrite(5,0);digitalWrite(6,0);
                        return;
                }
                int i;
                for (i = 0; i < _delay; ++i){
                        digitalWrite(5,(right>0)? 1:0);digitalWrite(6,(right>0)? 0:1);
                        delay(abs(right));
                        digitalWrite(5,0);digitalWrite(6,0);
                        delay(10-abs(right));
                }
                return;
        }
        if (right==10||right==-10){
                digitalWrite(5,(right==10)? 1:0);digitalWrite(6,(right==10)? 0:1);
                if (left==0){
                        digitalWrite(1,0);digitalWrite(4,0);
                        delay(_delay*10);
                        return;
                }
                int i;
                for (i = 0; i < _delay; ++i){
                        digitalWrite(1,(left>0)? 1:0);digitalWrite(4,(left>0)? 0:1);
                        delay(abs(left));
                        digitalWrite(1,0);digitalWrite(4,0);
                        delay(10-abs(left));
                }
                return;
        }
}
*/
static int _setup=0;
int setup(){
        if(wiringPiSetup () == -1)
                return -1;
        pinMode( 1, OUTPUT);//straigth //leftup
//        pinMode( 4, OUTPUT);//right     //left
        pinMode( 5, OUTPUT);//left      //rightup
//        pinMode( 6, OUTPUT);//stop      //right
        _setup=1;
        return 0;
}
int main_control(int input){
        if(!_setup)
                if(setup()==-1)
                        return -1;
        digitalWrite(1,0);digitalWrite(5,0);
        switch(input){
                case 1:
			printf("straigth\n");
			digitalWrite(1,1);
			digitalWrite(5,1);
			delay(100);
			break;
                case 4:
			printf("right\n");
			digitalWrite(1,0);
			digitalWrite(5,1);
			delay(200);
			break;
                case 5:
			printf("left\n");
			digitalWrite(1,1);
			digitalWrite(5,0);
			delay(200);
			break;
                case 6:
			printf("stop\n");
			return -1;
			break;
        }
	digitalWrite(1, 0);
	digitalWrite(5, 0);
        return 0;
}

