
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softPwm.h>

#define RANGE		100
#define	NUM_LEDS	 12

int ledMap [NUM_LEDS] = { 0, 1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13 } ;

int values [NUM_LEDS] = { 0, 17, 32, 50, 67, 85, 100, 85, 67, 50, 32, 17 } ;

int main ()
{
  int i, j ;
  char buf [80] ;

  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "oops: %s\n", strerror (errno)) ;
    return 1 ;
  }

  for (i = 0 ; i < NUM_LEDS ; ++i)
  {
    softPwmCreate (ledMap [i], 0, RANGE) ;
    printf ("%3d, %3d, %3d\n", i, ledMap [i], values [i]) ;
  }

  fgets (buf, 80, stdin) ;

// Bring all up one by one:

  for (i = 0 ; i < NUM_LEDS ; ++i)
    for (j = 0 ; j <= 100 ; ++j)
    {
      softPwmWrite (ledMap [i], j) ;
      delay (10) ;
    }

  fgets (buf, 80, stdin) ;

// Down fast

  for (i = 100 ; i > 0 ; --i)
  {
    for (j = 0 ; j < NUM_LEDS ; ++j)
      softPwmWrite (ledMap [j], i) ;
    delay (10) ;
  }

  fgets (buf, 80, stdin) ;

  for (;;)
  {
    for (i = 0 ; i < NUM_LEDS ; ++i)
      softPwmWrite (ledMap [i], values [i]) ;

    delay (50) ;

    i = values [0] ;
    for (j = 0 ; j < NUM_LEDS - 1 ; ++j)
      values [j] = values [j + 1] ;
    values [NUM_LEDS - 1] = i ;
  }
}
