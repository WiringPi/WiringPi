
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <piNes.h>

#define	BLANK	"|      "

int main ()
{
  int joystick ;
  unsigned int buttons ;

  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "oops: %s\n", strerror (errno)) ;
    return 1 ;
  }

  if ((joystick = setupNesJoystick (2, 1, 0)) == -1)
  {
    fprintf (stdout, "Unable to setup joystick\n") ;
    return 1 ;
  }

  for (;;)
  {
    buttons = readNesJoystick (joystick) ;

    if ((buttons & NES_UP)     != 0) printf ("|  UP  " ) ; else printf (BLANK) ;
    if ((buttons & NES_DOWN)   != 0) printf ("| DOWN " ) ; else printf (BLANK) ;
    if ((buttons & NES_LEFT)   != 0) printf ("| LEFT " ) ; else printf (BLANK) ;
    if ((buttons & NES_RIGHT)  != 0) printf ("|RIGHT " ) ; else printf (BLANK) ;
    if ((buttons & NES_SELECT) != 0) printf ("|SELECT" ) ; else printf (BLANK) ;
    if ((buttons & NES_START)  != 0) printf ("|START " ) ; else printf (BLANK) ;
    if ((buttons & NES_A)      != 0) printf ("|  A   " ) ; else printf (BLANK) ;
    if ((buttons & NES_B)      != 0) printf ("|  B   " ) ; else printf (BLANK) ;
    printf ("|\n") ;
  }

  return 0 ;
}
