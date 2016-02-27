/*
 * 7segments.c:
 *	Simple test program to see if we can drive a 7-segment LED
 *	display using the GPIO and little else on the Raspberry Pi
 *
 *	Copyright (c) 2013 Gordon Henderson
 ***********************************************************************
 */

#undef	PHOTO_HACK

#include <wiringPi.h>

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

/*
 *  Segment mapping
 *
 *	 --a--
 *	|     |
 *	f     b
 *	|     |
 *	 --g--
 *	|     |
 *	e     c
 *	|     |
 *	 --d--  p
 */

// GPIO Pin Mapping

static int digits   [6] = {  7, 11, 10, 13, 12, 14    } ;
static int segments [7] = {  6,  5,  4,  3,  2,  1, 0 } ;


static const int segmentDigits [] =
{
// a  b  c  d  e  f  g     Segments
// 6  5  4  3  2  1  0,	// wiringPi pin No.

   1, 1, 1, 1, 1, 1, 0,	// 0
   0, 1, 1, 0, 0, 0, 0,	// 1
   1, 1, 0, 1, 1, 0, 1,	// 2
   1, 1, 1, 1, 0, 0, 1,	// 3
   0, 1, 1, 0, 0, 1, 1,	// 4
   1, 0, 1, 1, 0, 1, 1,	// 5
   1, 0, 1, 1, 1, 1, 1,	// 6
   1, 1, 1, 0, 0, 0, 0,	// 7
   1, 1, 1, 1, 1, 1, 1,	// 8
   1, 1, 1, 1, 0, 1, 1,	// 9
   1, 1, 1, 0, 1, 1, 1,	// A
   0, 0, 1, 1, 1, 1, 1,	// b
   1, 0, 0, 1, 1, 1, 0,	// C
   0, 1, 1, 1, 1, 0, 1,	// d
   1, 0, 0, 1, 1, 1, 1,	// E
   1, 0, 0, 0, 1, 1, 1,	// F
   0, 0, 0, 0, 0, 0, 0,	// blank
} ;
 

// display:
//	A global variable which is written to by the main program and
//	read from by the thread that updates the display. Only the first
//	6 characters are used.

char display [8] ;


/*
 * displayDigits:
 *	This is our thread that's run concurrently with the main program.
 *	Essentially sit in a loop, parsing and displaying the data held in
 *	the "display" global.
 *********************************************************************************
 */

PI_THREAD (displayDigits)
{
  int digit, segment ;
  int index, d, segVal ;

  piHiPri (50) ;

  for (;;)
  {
    for (digit = 0 ; digit < 6 ; ++digit)
    {
      for (segment = 0 ; segment < 7 ; ++segment)
      {
	d = toupper (display [digit]) ;
	/**/ if ((d >= '0') && (d <= '9'))	// Digit
	  index = d - '0' ;
	else if ((d >= 'A') && (d <= 'F'))	// Hex
	  index = d - 'A' + 10 ;
	else
	  index = 16 ;				// Blank

	segVal = segmentDigits [index * 7 + segment] ;

	digitalWrite (segments [segment], segVal) ;
      }
      digitalWrite (digits [digit], 1) ;
      delay (2) ;
      digitalWrite (digits [digit], 0) ;
    }
  }
}


/*
 * setup:
 *	Initialise the hardware and start the thread
 *********************************************************************************
 */

void setup (void)
{
  int i, c ;

  wiringPiSetup () ;

// 7 segments

  for (i = 0 ; i < 7 ; ++i)
    { digitalWrite (segments [i], 0) ; pinMode (segments [i], OUTPUT) ; }

// 6 digits

  for (i = 0 ; i < 6 ; ++i)
    { digitalWrite (digits [i], 0) ;   pinMode (digits [i],   OUTPUT) ; }

  strcpy (display, "      ") ;
  piThreadCreate (displayDigits) ;
  delay (10) ; // Just to make sure it's started

// Quick countdown LED test sort of thing

  c = 999999 ;
  for (i = 0 ; i < 10 ; ++i)
  {
    sprintf (display, "%06d", c) ;
    delay (400) ;
    c -= 111111 ;
  }

  strcpy (display, "      ") ;
  delay (400) ;

#ifdef PHOTO_HACK
  sprintf (display, "%s", "123456") ;
  for (;;)
    delay (1000) ;
#endif

}


/*
 * teenager:
 *	No explanation needed. (Nor one given!)
 *********************************************************************************
 */

void teenager (void)
{
  char *message = "      feedbeef      babe      cafe      b00b      " ;
  int i ;

  for (i = 0 ; i < strlen (message) - 4 ; ++i)
  {
    strncpy (display, &message [i], 6) ;
    delay (200) ;
  }
  delay (1000) ;
  for (i = 0 ; i < 3 ; ++i)
  {
    strcpy (display, "    ") ;
    delay (150) ;
    strcpy (display, " b00b ") ;
    delay (250) ;
  }
  delay (1000) ;
  strcpy (display, "      ") ;
  delay (1000) ;
}


/*
 *********************************************************************************
 * main:
 *	Let the fun begin
 *********************************************************************************
 */

int main (void)
{
  struct tm *t ;
  time_t     tim ;

  setup    () ;
  teenager () ;

  tim = time (NULL) ;
  for (;;)
  {
    while (time (NULL) == tim)
      delay (5) ;

    tim = time (NULL) ;
    t   = localtime (&tim) ;

    sprintf (display, "%02d%02d%02d", t->tm_hour, t->tm_min, t->tm_sec) ;

    delay (500) ;
  }

  return 0 ;
}
