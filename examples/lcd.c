/*
 * lcd.c:
 *	Text-based LCD driver.
 *	This is designed to drive the parallel interface LCD drivers
 *	based in the Hitachi HD44780U controller and compatables.
 *
 * Copyright (c) 2012 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <lcd.h>

int main (void)
{
  int i, j ;
  int fd1, fd2 ; 

  char message1 [256] ;
  char message2 [256] ;
  char buf1 [30] ;
  char buf2 [30] ;

  struct tm *t ;
  time_t tim ;

  printf ("Raspberry Pi LCD test program\n") ;

  if (wiringPiSetup () == -1)
    exit (1) ;

  fd1 = lcdInit (4, 20, 4, 8,  9, 4,5,6,7,0,0,0,0) ;
  fd2 = lcdInit (2, 16, 4, 8, 10, 4,5,6,7,0,0,0,0) ;

//fd1 = lcdInit (4, 20, 8, 8,  9, 0,1,2,3,4,5,6,7) ;
//fd2 = lcdInit (2, 16, 8, 8, 10, 0,1,2,3,4,5,6,7) ;

  if (fd1 == -1)
  {
    printf ("lcdInit 1 failed\n") ;
    return 1 ;
  }

  if (fd2 == -1)
  {
    printf ("lcdInit 2 failed\n") ;
    return 1 ;
  }

  sleep (1) ;

  lcdPosition (fd1, 0, 0) ; lcdPuts (fd1, " Gordon Henderson") ;
  lcdPosition (fd1, 0, 1) ; lcdPuts (fd1, "  --------------") ;
/*
  lcdPosition (fd1, 0, 2) ; lcdPuts (fd1, "   00:00:00") ;
  lcdPosition (fd1, 0, 3) ; lcdPuts (fd1, "   DD:MM:YY") ;
*/

  lcdPosition (fd2, 0, 0) ; lcdPuts (fd2, "Gordon Henderson") ;
  lcdPosition (fd2, 0, 1) ; lcdPuts (fd2, "----------------") ;

  sleep (2) ;

  sprintf (message1, "%s", "                  http://projects.drogon.net/                    ") ;
  sprintf (message2, "%s", "                This is a long message to go into the smaller display just for a demonstration of what we can do.                ") ;

  for (;;)
  {
    i = 0 ;
    j = 0 ;
    for (;;)
    {
      strncpy (buf1, &message1 [i], 20) ;
      buf1 [20] = 0 ;
      lcdPosition (fd1, 0, 1) ;
      lcdPuts (fd1, buf1) ;
      ++i ;
      if (i == strlen (message1) - 20)
	i = 0 ;

      strncpy (buf2, &message2 [j], 16) ;
      buf2 [16] = 0 ;
      lcdPosition (fd2, 0, 1) ;
      lcdPuts (fd2, buf2) ;
      ++j ;
      if (j == strlen (message2) - 16)
	j = 0 ;

      tim = time (NULL) ;
      t = localtime (&tim) ;

      sprintf (buf1, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec) ;
      lcdPosition (fd1, 5, 2) ;
      lcdPuts (fd1, buf1) ;

      sprintf (buf1, "%02d/%02d/%02d", t->tm_mday, t->tm_mon + 1, t->tm_year+1900) ;
      lcdPosition (fd1, 4, 3) ;
      lcdPuts (fd1, buf1) ;

      delay (250) ;
    }
  }

  return 0 ;
}
