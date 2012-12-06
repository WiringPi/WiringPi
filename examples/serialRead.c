
/*
 * serialRead.c:
 *	Example program to read bytes from the Serial line
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringSerial.h>

int main ()
{
  int fd ;

  if ((fd = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

// Loop, getting and printing characters

  for (;;)
  {
    putchar (serialGetchar (fd)) ;
    fflush (stdout) ;
  }
}
