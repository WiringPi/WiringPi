/*
 * vumeter.c:
 *	Simple VU meter
 *
 * Heres the theory:
 *	We will sample at 4000 samples/sec and put the data into a
 *	low-pass filter with a depth of 1000 samples. This will give
 *	us 1/4 a second of lag on the signal, but I think it might
 *	produce a more pleasing output.
 *
 *	The input of the microphone should be at mid-pont with no
 *	sound input, but we might have to sample that too, to get
 *	our reference zero...
 *
 *	Copyright (c) 2013 Gordon Henderson
 ***********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <wiringPi.h>
#include <gertboard.h>

#ifndef	TRUE
#define	TRUE	(1==1)
#define	FALSE	(!TRUE)
#endif

#define	B_SIZE	1000
#define	S_SIZE	 128

static int buffer [B_SIZE] ;
static int bPtr = 0 ;

/*
 * ledPercent:
 *	Output the given value as a percentage on the LEDs
 *********************************************************************************
 */

static void ledPercent (int percent)
{
  unsigned int output = 0 ;

  if (percent > 11) output |= 0x01 ;
  if (percent > 22) output |= 0x02 ;
  if (percent > 33) output |= 0x04 ;
  if (percent > 44) output |= 0x08 ;
  if (percent > 55) output |= 0x10 ;
  if (percent > 66) output |= 0x20 ;
  if (percent > 77) output |= 0x40 ;
  if (percent > 88) output |= 0x80 ;

  digitalWriteByte (output) ;
}

static unsigned int tPeriod, tNextSampleTime ;

/*
 * sample:
 *	Get a sample from the Gertboard. If not enough time has elapsed
 *	since the last sample, then wait...
 *********************************************************************************
 */

static void sample (void)
{
  unsigned int tFuture ;

// Calculate the future sample time

  tFuture = tPeriod + tNextSampleTime ;

// Wait until the next sample time

  while (micros () < tNextSampleTime)
    ;
  
  buffer [bPtr] = gertboardAnalogRead (0) ;

  tNextSampleTime = tFuture ;
}


int main ()
{
  int quietLevel, min, max ;
  int i, sum ;
  unsigned int tStart, tEnd ;

  printf ("\n") ;
  printf ("Gertboard demo: VU Meter\n") ;
  printf ("========================\n") ;

  wiringPiSetup     () ;
  gertboardSPISetup () ;

  ledPercent (0) ;
  for (i = 0 ; i < 8 ; ++i)
    pinMode (i, OUTPUT) ;

  for (bPtr = 0 ; bPtr < B_SIZE ; ++bPtr)
    buffer [bPtr] = 99 ;

  tPeriod = 1000000 / 1000 ;

  printf ("Shhhh.... ") ; fflush (stdout) ;
  delay (1000) ;
  printf ("Sampling quiet... ") ; fflush (stdout) ;

  tStart = micros () ;

  tNextSampleTime = micros () ;
  for (bPtr = 0 ; bPtr < B_SIZE ; ++bPtr)
    sample () ;

  tEnd = micros () ;

  quietLevel = 0 ;
  max =    0 ;
  min = 1024 ;
  for (i = 0 ; i < B_SIZE ; ++i)
  {
    quietLevel += buffer [i] ;
    if (buffer [i] > max) max = buffer [i] ;
    if (buffer [i] < min) min = buffer [i] ;
  }
  quietLevel /= B_SIZE ;

  printf ("Done. Quiet level is: %d [%d:%d] [%d:%d]\n", quietLevel, min, max, quietLevel - min, max - quietLevel) ;

  printf ("Time taken for %d reads: %duS\n", B_SIZE, tEnd - tStart) ;

  for (bPtr = 0 ;;)
  {
    sample () ;
    sum = 0 ;
    for (i = 0 ; i < S_SIZE ; ++i)
      sum += buffer [i] ;
    sum /= S_SIZE ;
    sum = abs (quietLevel - sum) ;
    sum = (sum * 1000) / quietLevel ;
    ledPercent (sum) ;
    if (++bPtr > S_SIZE)
      bPtr = 0 ;
  }


  return 0 ;
}
