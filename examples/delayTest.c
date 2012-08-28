
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>

#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#define	CYCLES	1000
#define	DELAY	99

int main()
{
  int x ;
  struct timeval t1, t2 ;
  long long    t ;
  unsigned int max, min ;

  unsigned int values [CYCLES] ;

  max = 0 ;
  min = 1000000 ;

  if (wiringPiSetup () == -1)
    return 1 ;

  piHiPri (10) ;
  sleep (1) ;

// Baseline test

  gettimeofday (&t1, NULL) ;
  gettimeofday (&t2, NULL) ;

  t = t2.tv_usec - t1.tv_usec ;
  printf ("Baseline test: %lld\n", t);

  for (x = 0 ; x < CYCLES ; ++x)
  {
    gettimeofday (&t1, NULL) ;
    delayMicroseconds (DELAY) ;
    gettimeofday (&t2, NULL) ;
      
    t = t2.tv_usec - t1.tv_usec ;
    if (t > max) max = t ;
    if (t < min) min = t ;
    values [x] = t ;
  }

  printf ("Done: Max: %d, min: %d\n", max, min) ;

  for (x = 0 ; x < CYCLES ; ++x)
  {
    printf ("%4d", values [x]) ;
    if (values [x] > DELAY)
      printf (".") ;
    else if (values [x] < DELAY)
      printf ("-") ;
    else
      printf (" ") ;
    if (((x + 1) % 20) == 0)
      printf ("\n") ;
  }
  printf ("\n") ;

  return 0 ;
}
