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
float fExpectTimedigitalWrite = 1/10;
float fExpectTimedigitalRead = 1/5;
float fExpectTimepinMode = 1/4;
float fWriteReadDelayFactor = 1.77;
int GPIO = 19;
int GPIOIN = 26;
int RaspberryPiModel = -1;


void ReportElapedTime(const char* msg, int multiop, const float fExpectTime, struct timeval t1, struct timeval t2) {
    double elapsedTime, fTimePerOperation, fFreq;

	elapsedTime = (t2.tv_sec-t1.tv_sec)+(t2.tv_usec-t1.tv_usec)/1000000.0;
	fTimePerOperation = elapsedTime*1000000.0/ToggleValue/multiop;
	fFreq = 1.0f/(fTimePerOperation*2); //ToggleValue/elapsedTime/1000000.0;
	printf("  % 9d took %.3f s, Time per operation %.3f us (toggle Freq %.3f MHz) \n",
	  ToggleValue, elapsedTime, fTimePerOperation, fFreq);
    CheckSameFloat(msg, fTimePerOperation, fExpectTime, fExpectTime*0.2f);
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
      case PI_MODEL_B:  //ARM=800MHz: 3.8/1.5
      case PI_MODEL_BP:
      case PI_MODEL_AP:
      case PI_MODEL_CM:
        ToggleValue /= 7; 
        fExpectTimedigitalWrite = 1/(3.8*2); //MHz;
        fExpectTimepinMode = 1/(1.5*2);
        break;
      case PI_MODEL_ZERO:
      case PI_MODEL_ZERO_W: //ARM=1000MHz: 4.8/2.0
        ToggleValue /= 5; 
        fExpectTimedigitalWrite = 1/(4.8*2); //MHz;
        fExpectTimepinMode = 1/(2.0*2);
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
        fExpectTimedigitalWrite = 1/(2*24.5); //MHz;
        fExpectTimepinMode = 1/(2*4.1);
        break;
      case PI_MODEL_5:
        ToggleValue = ToggleValue*0.8; 
        fExpectTimedigitalWrite = 1/(2*20.0); //MHz;
        fExpectTimepinMode = 1/(2*2.5);
         break;
    }

    fExpectTimedigitalRead = fExpectTimedigitalWrite*2;



	printf("WiringPi GPIO speed test program (using GPIO %d) and toggle %d times\n", GPIO, ToggleValue);
  printf(" testing digital Wirte and pinMode\n");	

	pinMode(GPIO, OUTPUT);

	printf("% 3d million times digitalWrite ...\n", ToggleValue/1000000);
	gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		digitalWrite(GPIO, HIGH);
    digitalWrite(GPIO, LOW);
	}
	gettimeofday(&t2, NULL);
  ReportElapedTime("digitalWrite", 2, fExpectTimedigitalWrite, t1, t2);

	digitalWrite(GPIO, LOW);
	pinMode(GPIOIN, INPUT);

  ToggleValue /=2;
  printf("\n");
  printf("% 3d million times digitalRead ...\n", ToggleValue/1000000);
	gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		digitalRead(GPIOIN);
    digitalRead(GPIOIN);
	}
	gettimeofday(&t2, NULL);
  ReportElapedTime("digitalRead", 2, fExpectTimedigitalRead, t1, t2);
    
  ToggleValue /= 4;
  printf("\n");
  printf("% 3d million times pinMode...\n", ToggleValue/1000000);
  gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		pinMode(GPIO, OUTPUT);
    pinMode(GPIOIN, INPUT);
	}
	gettimeofday(&t2, NULL);
  ReportElapedTime("pinMode", 2, fExpectTimepinMode, t1, t2);

  printf("\n");
  printf("Toggle % 3d million times pinMode and digitalWrite ..\n", ToggleValue/1000000);
  gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		pinMode(GPIO, OUTPUT);
		digitalWrite(GPIO, LOW);
		digitalWrite(GPIO, HIGH);
		pinMode(GPIO, INPUT);
	}
	gettimeofday(&t2, NULL);
  ReportElapedTime("pinMode and digitalWrite", 2, fExpectTimepinMode+fExpectTimedigitalWrite, t1, t2);

  ToggleValue *= 1.5;
  printf("\n");
  pinMode(GPIO, OUTPUT);
  printf("Toggle % 3d million times digitalRead and digitalWrite ..\n", ToggleValue/1000000);
  gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		digitalWrite(GPIO, HIGH);
		digitalRead(GPIOIN);
  	digitalWrite(GPIO, LOW);
		digitalRead(GPIOIN);  
	}
	gettimeofday(&t2, NULL);
  ReportElapedTime("digitalRead and digitalWrite", 2, fExpectTimedigitalWrite+fExpectTimedigitalRead*fWriteReadDelayFactor, t1, t2);
  double elapsedTime = (t2.tv_sec-t1.tv_sec)+(t2.tv_usec-t1.tv_usec)/1000000.0;
  double fTimePerOperation = elapsedTime*1000000.0/ToggleValue/2;
  CheckSameFloat("Write <=> Read delay factor", fTimePerOperation/(fExpectTimedigitalWrite+fExpectTimedigitalRead), fWriteReadDelayFactor, 0.2);

	digitalWrite(GPIO, LOW);
	pinMode(GPIO, INPUT);

	return(EXIT_SUCCESS);
}

