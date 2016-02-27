/*
 * record.c:
 *	Record some audio via the Gertboard
 *
 *	Copyright (c) 2013 Gordon Henderson
 ***********************************************************************
 */

#include <stdio.h>
#include <sys/time.h>

#include <wiringPi.h>
#include <gertboard.h>

#define	B_SIZE	40000

int main ()
{
  int i ;
  struct timeval tStart, tEnd, tTaken ;
  unsigned char buffer [B_SIZE] ;

  printf ("\n") ;
  printf ("Gertboard demo: Recorder\n") ;
  printf ("========================\n") ;

// Always initialise wiringPi. Use wiringPiSys() if you don't need
//	(or want) to run as root

  wiringPiSetupSys () ;

// Initialise the Gertboard analog hardware at pin 100

  gertboardAnalogSetup (100) ;

  gettimeofday (&tStart, NULL) ;

  for (i = 0 ; i < B_SIZE ; ++i)
    buffer [i] = analogRead (100) >> 2 ;

  gettimeofday (&tEnd, NULL) ;
  
  timersub (&tEnd, &tStart, &tTaken) ;

  printf ("Time taken for %d  reads: %ld.%ld\n", B_SIZE, tTaken.tv_sec, tTaken.tv_usec) ;

  gettimeofday (&tStart, NULL) ;

  for (i = 0 ; i < B_SIZE ; ++i)
   analogWrite (100, buffer [i]) ;

  gettimeofday (&tEnd, NULL) ;
  
  timersub (&tEnd, &tStart, &tTaken) ;

  printf ("Time taken for %d writes: %ld.%ld\n", B_SIZE, tTaken.tv_sec, tTaken.tv_usec) ;

  return 0 ;
}

