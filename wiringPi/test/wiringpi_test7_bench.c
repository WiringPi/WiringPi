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
float fExpectTimedigitalWrite = 0.1;
float fExpectTimedigitalRead  = 0.1;
float fExpectTimepinMode      = 0.1;
float fWriteReadDelayFactor   = 1.0;
float fWriteReadFactor        = 1.0;
float fPi4ExpectTimedigitalWrite = 0.020;
int GPIO = 19;
int GPIOIN = 26;
int RaspberryPiModel = -1;


double ReportElapedTime(const char* msg, int multiop, const float fExpectTime, struct timeval t1, struct timeval t2) {
    double elapsedTime, fTimePerOperation, fFreq;

	elapsedTime = (t2.tv_sec-t1.tv_sec)+(t2.tv_usec-t1.tv_usec)/1000000.0;
	fTimePerOperation = elapsedTime*1000000.0/ToggleValue/multiop;
	fFreq = 1.0f/(fTimePerOperation*2); //ToggleValue/elapsedTime/1000000.0;
	printf("  % 9d took %.3f s, Time per operation %.3f us (toggle Freq %.3f MHz) \n",
	  ToggleValue, elapsedTime, fTimePerOperation, fFreq);
  CheckSameFloat(msg, fTimePerOperation, fExpectTime, fExpectTime*0.2f);

  return fTimePerOperation;
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
    case PI_MODEL_B:     //ARM=800MHz
    case PI_MODEL_BP:
    case PI_MODEL_AP:
    case PI_MODEL_CM:
      fExpectTimedigitalWrite = 0.132; //us;
      fExpectTimedigitalRead  = 0.171; //us
      fExpectTimepinMode      = 0.334; //us
      break;
    case PI_MODEL_ZERO:
    case PI_MODEL_ZERO_W: //ARM=1000MHz
      fExpectTimedigitalWrite = 0.104; //us;
      fExpectTimedigitalRead  = 0.135; //us
      fExpectTimepinMode      = 0.360; //us
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
    case PI_MODEL_4B:     //ARM=1500MHz
    case PI_MODEL_400:
    case PI_MODEL_CM4:
    case PI_MODEL_CM4S:
      fExpectTimedigitalWrite = 0.020; //us
      fExpectTimedigitalRead  = 0.038; //us
      fExpectTimepinMode      = 0.121; //us
      fWriteReadDelayFactor   = 1.86;
      break;
    default:
      if (piRP1Model()) {
        // So far expect all Pi5 / RP1 hardware has same performance
        printf("Raspberry Pi with RP1 chip found\n");
        fExpectTimedigitalWrite = 0.025; //us
        fExpectTimedigitalRead  = 0.323; //us
        fExpectTimepinMode      = 0.200; //us
        fWriteReadDelayFactor   = 3.2;
      } else {
        printf("Unknown Raspberry Pi found, exit\n");
        return(EXIT_FAILURE);
      }
      break;
  }
  fWriteReadFactor = fExpectTimedigitalRead/fExpectTimedigitalWrite;
  ToggleValue /= (fExpectTimedigitalWrite/fPi4ExpectTimedigitalWrite);

	printf("WiringPi GPIO operation time test program (using GPIO %d/%d)\n", GPIO, GPIOIN);
	pinMode(GPIO, OUTPUT);

	printf("\n% 3d million times digitalWrite ...\n", ToggleValue/1000000);
	gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		digitalWrite(GPIO, HIGH);
    digitalWrite(GPIO, LOW);
	}
	gettimeofday(&t2, NULL);
  double OpTimeWrite = ReportElapedTime("digitalWrite", 2, fExpectTimedigitalWrite, t1, t2);

	digitalWrite(GPIO, LOW);
	pinMode(GPIOIN, INPUT);

  ToggleValue /=(fExpectTimedigitalRead/fExpectTimedigitalWrite);
  printf("\n% 3d million times digitalRead ...\n", ToggleValue/1000000);
	gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		digitalRead(GPIOIN);
    digitalRead(GPIOIN);
	}
	gettimeofday(&t2, NULL);
  double OpTimeRead = ReportElapedTime("digitalRead", 2, fExpectTimedigitalRead, t1, t2);
    
  if (!piRP1Model()) {
    ToggleValue /= 4;
  } else {
    ToggleValue *= 1.5;
  }
  printf("\n% 3d million times pinMode...\n", ToggleValue/1000000);
  gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		pinMode(GPIO, OUTPUT);
    pinMode(GPIOIN, INPUT);
	}
	gettimeofday(&t2, NULL);
  ReportElapedTime("pinMode", 2, fExpectTimepinMode, t1, t2);

  printf("\nToggle % 3d million times pinMode and digitalWrite ..\n", ToggleValue/1000000);
  gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		pinMode(GPIO, OUTPUT);
		digitalWrite(GPIO, LOW);
		digitalWrite(GPIO, HIGH);
		pinMode(GPIO, INPUT);
	}
	gettimeofday(&t2, NULL);
  ReportElapedTime("pinMode and digitalWrite", 2, fExpectTimepinMode+fExpectTimedigitalWrite, t1, t2);

  if (!piRP1Model()) {
    ToggleValue *= 1.5;
  } else {
    ToggleValue /= 5;
  }
  pinMode(GPIO, OUTPUT);
  printf("\nToggle % 3d million times digitalRead and digitalWrite ..\n", ToggleValue/1000000);
  gettimeofday(&t1, NULL);
	for (int loop=1; loop<ToggleValue; loop++) {
		digitalWrite(GPIO, HIGH);
		digitalRead(GPIOIN);
  	digitalWrite(GPIO, LOW);
		digitalRead(GPIOIN);  
	}
	gettimeofday(&t2, NULL);
  double fTimePerOperation = ReportElapedTime("digitalRead and digitalWrite", 2, (fExpectTimedigitalWrite+fExpectTimedigitalRead)*fWriteReadDelayFactor, t1, t2);

  printf("\n");
  CheckSameFloat("digitalWrite vs. digitalRead factor", OpTimeRead/OpTimeWrite, fWriteReadFactor, 0.3);
  CheckSameFloat("digitalWrite and digitalRead alternating factor", fTimePerOperation/(fExpectTimedigitalWrite+fExpectTimedigitalRead), fWriteReadDelayFactor, 0.2);
  if (piRP1Model()) {
    printf("\nRasperry Pi with RP1 chip:\n");
    printf("  * digitalRead has very slow speed, much slower then digitalWrite, factor %.1f (typical Pi4 ~1.9)\n", fWriteReadFactor);
    printf("  * Alternating read/write operation has slow speed, factor %.2f (typical Pi4 ~1.86) to single operation time\n", fWriteReadDelayFactor);
  }

	digitalWrite(GPIO, LOW);
	pinMode(GPIO, INPUT);

	return(EXIT_SUCCESS);
}

