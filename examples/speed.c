
/*
 * speed.c:
 *	Simple program to measure the speed of the various GPIO
 *	access mechanisms.
 */

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define	FAST_COUNT	10000000
#define	SLOW_COUNT	 1000000


int main (void)
{
  int i ;
  uint32_t start, end, count, sum, perSec ;

  printf ("Raspberry Pi wiringPi speed test program\n") ;

// Start the standard way

  if (wiringPiSetup () == -1)
    exit (1) ;

  printf ("Native wiringPi method: (%8d iterations)\n", FAST_COUNT) ;

  pinMode (0, OUTPUT) ;

  sum = 0 ;
  for (i = 0 ; i < 3 ; ++i)
  {
    printf ("  Pass: %d: ", i) ;
    fflush (stdout) ;

    start = millis () ;
    for (count = 0 ; count < FAST_COUNT ; ++count)
      digitalWrite (0, 1) ;
    end = millis () ;
    printf (" %8dmS\n", end - start) ;
    sum += (end - start) ;
  }
  digitalWrite (0, 0) ;
  printf ("   Average: %8dmS", sum / 3) ;
  perSec = (int)(double)FAST_COUNT / (double)((double)sum / 3.0) * 1000.0 ;
  printf (": %6d/sec\n", perSec) ;


  printf ("Native GPIO method: (%8d iterations)\n", FAST_COUNT) ;

  if (wiringPiSetupGpio () == -1)
    exit (1) ;

  pinMode (17, OUTPUT) ;

  sum = 0 ;
  for (i = 0 ; i < 3 ; ++i)
  {
    printf ("  Pass: %d: ", i) ;
    fflush (stdout) ;

    start = millis () ;
    for (count = 0 ; count < 10000000 ; ++count)
      digitalWrite (17, 1) ;
    end = millis () ;
    printf (" %8dmS\n", end - start) ;
    sum += (end - start) ;
  }
  digitalWrite (17, 0) ;
  printf ("   Average: %8dmS", sum / 3) ;
  perSec = (int)(double)FAST_COUNT / (double)((double)sum / 3.0) * 1000.0 ;
  printf (": %6d/sec\n", perSec) ;


// Switch to SYS mode:

  if (wiringPiSetupSys () == -1)
    exit (1) ;

  printf ("/sys/class/gpio method: (%8d iterations)\n", SLOW_COUNT) ;

  sum = 0 ;
  for (i = 0 ; i < 3 ; ++i)
  {
    printf ("  Pass: %d: ", i) ;
    fflush (stdout) ;

    start = millis () ;
    for (count = 0 ; count < SLOW_COUNT ; ++count)
      digitalWrite (17, 1) ;
    end = millis () ;
    printf (" %8dmS\n", end - start) ;
    sum += (end - start) ;
  }
  digitalWrite (17, 0) ;
  printf ("   Average: %8dmS", sum / 3) ;
  perSec = (int)(double)SLOW_COUNT / (double)((double)sum / 3.0) * 1000.0 ;
  printf (": %6d/sec\n", perSec) ;

  return 0 ;
}
