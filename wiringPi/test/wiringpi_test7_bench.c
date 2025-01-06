// WiringPi test program: benchmark
// Compile: gcc -Wall wiringpi_test7_bench.c -o wiringpi_test7_bench -lwiringPi

#include "wpi_test.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


int ToggleValue = 240000000;
float fExpectMHzdigitalWrite = 10;
float fExpectMHzpinMode = 4;
int GPIO = 19;
int GPIOIN = 26;
int RaspberryPiModel = -1;


void ReportElaped(const char* msg, const float fExpectMHz, struct timeval t1, struct timeval t2) {
    double elapsedTime, fTimePerOperation, fFreq;

	elapsedTime = (t2.tv_sec-t1.tv_sec)+(t2.tv_usec-t1.tv_usec)/1000000.0;
	fTimePerOperation = elapsedTime*1000000.0/ToggleValue;
	fFreq = ToggleValue/elapsedTime/1000000.0;
	printf("  % 9d toggle took %.3f s, Time per toggle %.3f us, Freq %.3f MHz \n",
	  ToggleValue, elapsedTime, fTimePerOperation, fFreq);
    CheckSameFloat(msg, fFreq, fExpectMHz, fExpectMHz*0.2f);
}


int main (void) {
	struct timeval t1, t2;

	if (wiringPiSetupGpio()  == -1) {
		printf("wiringPiSetupGpio failed\n\n");
		exit(EXIT_FAILURE);
	}

	int rev, mem, maker, overVolted;
	piBoardId(&RaspberryPiModel, &rev, &mem, &maker, &overVolted);
	CheckNotSame("Model: ", RaspberryPiModel, -1);
	if (!piBoard40Pin()) {
	    GPIO = 23;
		GPIOIN = 24;
	}

    switch(RaspberryPiModel) {
      case PI_MODEL_A:
      case PI_MODEL_B:
      case PI_MODEL_BP:
      case PI_MODEL_AP:
      case PI_MODEL_ZERO:
      case PI_MODEL_ZERO_W:
      case PI_MODEL_CM:
        ToggleValue /= 8; 
        break;
      case PI_MODEL_2:
        ToggleValue /= 4;
        break;
      case PI_MODEL_3B:
      case PI_MODEL_CM3:
      case PI_MODEL_3BP:
      case PI_MODEL_3AP:
      case PI_MODEL_CM3P:
      case PI_MODEL_ZERO_2W:
        ToggleValue /= 2; 
        break;
      case PI_MODEL_4B:
      case PI_MODEL_400:
      case PI_MODEL_CM4:
      case PI_MODEL_CM4S:
        ToggleValue = ToggleValue; 
        fExpectMHzdigitalWrite = 24.5; //MHz;
        fExpectMHzpinMode = 4.1;
        break;
      case PI_MODEL_5:
        ToggleValue *= 2; 
         break;
    }



	printf("WiringPi GPIO speed test program (using GPIO %d) and toggle %d times\n", GPIO, ToggleValue);
    printf(" testing digital Wirte and pinMode\n");	

	pinMode(GPIO, OUTPUT);

	printf("toggle % 3d million times pin value ...\n", ToggleValue/1000000);
	gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		digitalWrite(GPIO, LOW);
		digitalWrite(GPIO, HIGH);
	}
	gettimeofday(&t2, NULL);
    ReportElaped("GPIO digitalWrite max. frequency ", fExpectMHzdigitalWrite, t1, t2);

	digitalWrite(GPIO, LOW);
	pinMode(GPIO, INPUT);
    
    ToggleValue /= 10;
    printf("\n\n");
    printf("toggle % 3d million times pin mode...\n", ToggleValue/1000000);
    gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		pinMode(GPIO, OUTPUT);
		pinMode(GPIO, INPUT);
	}
	gettimeofday(&t2, NULL);
    ReportElaped("GPIO pinMode max. frequency ", fExpectMHzpinMode, t1, t2);


	return(EXIT_SUCCESS);
}

