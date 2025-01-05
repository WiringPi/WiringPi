// WiringPi test program: Kernel char device interface / sysfs successor
// Compile: gcc -Wall wiringpi_test1_device.c -o wiringpi_test1_device -lwiringPi

#include "wpi_test.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>


int GPIO = 19;
int GPIOIN = 26;
const int ToggleValue = 4;


static volatile int globalCounter;
volatile long long gStartTime, gEndTime;


static void wfi (void) { 
  struct timeval now;

  gettimeofday(&now, 0);
  if (0==gStartTime) {
    gStartTime = now.tv_sec*1000000LL + now.tv_usec;
  } else {
    gEndTime = now.tv_sec*1000000LL + now.tv_usec;
  }
  globalCounter++;
}


double StartSequence (int Enge, int OUTpin) {
  int expected;
  double timeExpected;

  gStartTime = 0;
  gEndTime = 0;
  globalCounter = 0;
  printf("Start\n");
  digitalWrite(OUTpin, HIGH); 
  delay(200);
  digitalWrite(OUTpin, LOW);
  delay(100); 
  digitalWrite(OUTpin, HIGH); 
  delay(200);
  digitalWrite(OUTpin, LOW);
  delay(100);
  printf("Stop\n");
  int globalCounterCopy = globalCounter; 

  if (INT_EDGE_BOTH == Enge) {
    expected = 4;  
    timeExpected = 500;
  } else {
    expected = 2;  
    timeExpected = 300;
  }

  if (globalCounter==expected) {
    double fTime = (gEndTime - gStartTime) / 1000000.0;
    printf("IRQ worked %g sec (~%gs expected)", fTime, timeExpected/1000.0);
    double diff = fTime-(timeExpected/1000.0); 
    if(diff<0.05 && diff>0) {
      printf("          -> %spassed%s\n", COLORGRN, COLORDEF);
    } else {
      printf("          -> %sfailed%s\n", COLORRED, COLORDEF);
    }

    return fTime;
  } else {
    printf("IRQ not worked got %d iterations (%d exprected)\n\n", globalCounterCopy, expected);
    return 0;
  }
}


double DurationTime(int Enge, int OUTpin, int IRQpin) {
  struct timeval now;
  double fTime = 0.0;

  gStartTime = 0;
  gEndTime = 0;
  globalCounter = 0;
  printf("Start\n");

  if (INT_EDGE_RISING == Enge) {
    digitalWrite(OUTpin, LOW); 
    wiringPiISR (IRQpin, INT_EDGE_RISING, &wfi) ;
    sleep(1);
    gettimeofday(&now, 0);
    gStartTime = now.tv_sec*1000000LL + now.tv_usec;
    digitalWrite(OUTpin, HIGH);  
    delay(20);
    digitalWrite(OUTpin, LOW);  
   } else if (INT_EDGE_FALLING == Enge) {
    digitalWrite(OUTpin, HIGH); 
    wiringPiISR (IRQpin, INT_EDGE_FALLING, &wfi) ;
    sleep(1);
    gettimeofday(&now, 0);
    gStartTime = now.tv_sec*1000000LL + now.tv_usec;    
    digitalWrite(OUTpin, LOW);  
  }

  sleep(1);
  fTime = (gEndTime - gStartTime);
  printf("IRQ detect time %g usec", fTime);
  if (fTime<2000 && fTime>0) {
    printf("                          -> %spassed%s\n", COLORGRN, COLORDEF);
  } else {
    printf("                          -> %sfailed%s\n", COLORRED, COLORDEF);
  }
  wiringPiISRStop (IRQpin) ;

  return fTime;
}


int main (void) {

	int major, minor;

	wiringPiVersion(&major, &minor);

	printf("WiringPi GPIO test program 1 (using GPIO%d (output) and GPIO%d (input))\n", GPIO, GPIOIN);
	printf(" testing irq\n");

	printf("\nISR test (WiringPi %d.%d)\n", major, minor);

	wiringPiSetupGpio() ;
	if (!piBoard40Pin()) {
		GPIO = 23;
		GPIOIN = 24;
	}
	int IRQpin = GPIOIN ;
	int OUTpin = GPIO ;
	
	pinMode(IRQpin, INPUT);
	pinMode(OUTpin, OUTPUT);
	digitalWrite (OUTpin, LOW) ;


	printf("Testing IRQ @ GPIO%d with trigger @ GPIO%d rising\n", IRQpin, OUTpin);
	wiringPiISR (IRQpin, INT_EDGE_RISING, &wfi) ;
	sleep(1);
	StartSequence (INT_EDGE_RISING, OUTpin);
	printf("Testing close\n");

	wiringPiISRStop (IRQpin) ;

	printf("Testing IRQ @ GPIO%d with trigger @ GPIO%d falling\n", IRQpin, OUTpin);
	wiringPiISR (IRQpin, INT_EDGE_FALLING, &wfi) ;
	sleep(1);
	StartSequence (INT_EDGE_FALLING, OUTpin);
	printf("Testing close\n");
	wiringPiISRStop (IRQpin) ;

	printf("Testing IRQ @ GPIO%d with trigger @ GPIO%d both\n", IRQpin, OUTpin);
	wiringPiISR (IRQpin, INT_EDGE_BOTH, &wfi) ;
	sleep(1);
	StartSequence (INT_EDGE_BOTH, OUTpin);
	printf("Testing close\n");
	wiringPiISRStop (IRQpin) ;

	for (int count=0; count<2; count++) {
	printf("Measuring duration IRQ @ GPIO%d with trigger @ GPIO%d rising\n", IRQpin, OUTpin);
	DurationTime(INT_EDGE_RISING, OUTpin, IRQpin);

	printf("Measuring duration IRQ @ GPIO%d with trigger @ GPIO%d falling\n", IRQpin, OUTpin);
	DurationTime(INT_EDGE_FALLING, OUTpin, IRQpin);
	}
	pinMode(OUTpin, INPUT);

	return UnitTestState();
}
