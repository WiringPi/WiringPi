/*
 * isr_debounce.c:
 *	Wait for Interrupt test program  WiringPi >=3.13 - ISR method
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>

#include <time.h>

#define BOUNCETIME 300
#define TIMEOUT    10000
//*************************************
// BCM pins
// IRQpin : setup as input with internal pullup. Connected with push button to GND with 1K resistor in series.
// OUTpin : connected to a LED with 470 Ohm resistor in series to GND. Toggles LED with every push button pressed.
//*************************************
#define IRQpin     16
#define OUTpin     12 

int toggle = 0;

/*
 * myInterrupt:
 *********************************************************************************
 */

static void wfi (unsigned int gpio, long long int timestamp) { 
//  struct timeval now;
  long long int timenow, diff;
  struct timespec curr;

  if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1) {
        printf("clock_gettime error");
        return;
  }
  
  timenow = curr.tv_sec * 1000000LL + curr.tv_nsec/1000L; // convert to microseconds
  diff = timenow - timestamp;
  
  printf("gpio BCM = %d, IRQ timestamp = %lld microseconds, timenow = %lld, diff = %lld\n", gpio, timestamp, timenow, diff);
  if (toggle == 0) {
    digitalWrite (OUTpin, HIGH) ;
    toggle = 1;
  }
  else {
    digitalWrite (OUTpin, LOW) ; 
    toggle = 0;
  }
}



int main (void)
{
  int major, minor;
  long long int ret;
  
  wiringPiVersion(&major, &minor);

  printf("\nISR debounce test (WiringPi %d.%d)\n\n", major, minor);
  
  wiringPiSetupGpio() ;

  pinMode(IRQpin, INPUT);
  
  // pull up/down mode (PUD_OFF, PUD_UP, PUD_DOWN) => down
  pullUpDnControl(IRQpin, PUD_UP);

  pinMode(OUTpin, OUTPUT);
  digitalWrite (OUTpin, LOW) ;
  
  // test waitForInterrupt

  ret = waitForInterruptInit (IRQpin, INT_EDGE_FALLING);
  if (ret < 0) {
    printf("waitForInterruptInit returned error %d\n", ret);
    pinMode(OUTpin, INPUT);
    return 0;
  }
  
  printf("Testing waitForInterrupt IRQ @ GPIO%d, timeout is %d\n", IRQpin, TIMEOUT);  
  ret = waitForInterrupt(IRQpin, TIMEOUT, 0);
  if (ret < 0) {
    printf("waitForInterrupt returned error %lld\n", ret);
    wiringPiISRStop (IRQpin) ;  
    pinMode(OUTpin, INPUT);
    return 0;    
  }
  else if (ret == 0) {
    printf("waitForInterrupt timed out %lld\n\n", ret);
    wiringPiISRStop (IRQpin) ;  
  }   
  else {
    printf("waitForInterrupt: falling edge fired at %lld microseconds\n\n", ret);    
    wiringPiISRStop (IRQpin) ;  
  }
  
  printf("Testing IRQ @ GPIO%d with trigger @ GPIO%d falling edge and bouncetime %d ms. Toggle LED @ OUTpin on IRQ.\n\n", IRQpin, IRQpin, BOUNCETIME, OUTpin);
  printf("To stop program hit return key\n\n");
  
  wiringPiISR (IRQpin, INT_EDGE_FALLING, &wfi, BOUNCETIME) ; 
 
//  sleep(30);
  getc(stdin);
  
  wiringPiISRStop (IRQpin) ;  
  pinMode(OUTpin, INPUT);

  return 0 ;  
}