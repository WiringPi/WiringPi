
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softTone.h>

#define RANGE		100
#define	NUM_LEDS	 12

int scale [8] = { 262, 294, 330, 349, 392, 440, 494, 525 } ;

int main ()
{
  int i, j ;
  char buf [80] ;

  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "oops: %s\n", strerror (errno)) ;
    return 1 ;
  }

  softToneCreate (3) ;

  for (;;)
  {
    for (i = 0 ; i < 8 ; ++i)
    {
      printf ("%3d\n", i) ;
      softToneWrite (3, scale [i]) ;
      delay (500) ;
    }
  }

}
