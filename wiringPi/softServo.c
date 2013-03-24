/*
 * softServo.c:
 *	Provide N channels of software driven PWM suitable for RC
 *	servo motors.
 *	Copyright (c) 2012 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

//#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#include "wiringPi.h"
#include "softServo.h"

// RC Servo motors are a bit of an oddity - designed in the days when 
//	radio control was experimental and people were tryin to make
//	things as simple as possible as it was all very expensive...
//
// So... To drive an RC Servo motor, you need to send it a modified PWM
//	signal - it needs anything from 1ms to 2ms - with 1ms meaning
//	to move the server fully left, and 2ms meaning to move it fully
//	right. Then you need a long gap before sending the next pulse.
//	The reason for this is that you send a multiplexed stream of these
//	pulses up the radio signal into the reciever which de-multiplexes
//	them into the signals for each individual servo. Typically there
//	might be 8 channels, so you need at least 8 "slots" of 2mS pulses
//	meaning the entire frame must fit into a 16mS slot - which would
//	then be repeated...
//
// In practice we have a total slot width of about 20mS - so we're sending 50
//	updates per second to each servo.
//
// In this code, we don't need to be too fussy about the gap as we're not doing
//	the multipexing, but it does need to be at least 10mS, and preferably 16
//	from what I've been able to determine.

// WARNING:
//	This code is really experimental. It was written in response to some people
//	asking for a servo driver, however while it works, there is too much
//	jitter to successfully drive a small servo - I have tried it with a micro
//	servo and it worked, but the servo ran hot due to the jitter in the signal
//	being sent to it.
//
//	If you want servo control for the Pi, then use the servoblaster kernel
//	module.

#define	MAX_SERVOS	8

static int pinMap     [MAX_SERVOS] ;	// Keep track of our pins
static int pulseWidth [MAX_SERVOS] ;	// microseconds


/*
 * softServoThread:
 *	Thread to do the actual Servo PWM output
 *********************************************************************************
 */

static PI_THREAD (softServoThread)
{
  register int i, j, k, m, tmp ;
  int lastDelay, pin, servo ;

  int myDelays [MAX_SERVOS] ;
  int myPins   [MAX_SERVOS] ;

  struct timeval  tNow, tStart, tPeriod, tGap, tTotal ;
  struct timespec tNs ;

  tTotal.tv_sec  =    0 ;
  tTotal.tv_usec = 8000 ;

  piHiPri (50) ;

  for (;;)
  {
    gettimeofday (&tStart, NULL) ;

    memcpy (myDelays, pulseWidth, sizeof (myDelays)) ;
    memcpy (myPins,   pinMap,     sizeof (myPins)) ;

// Sort the delays (& pins), shortest first

    for (m = MAX_SERVOS / 2 ; m > 0 ; m /= 2 )
      for (j = m ; j < MAX_SERVOS ; ++j)
	for (i = j - m ; i >= 0 ; i -= m)
	{
	  k = i + m ;
	  if (myDelays [k] >= myDelays [i])
	    break ;
	  else // Swap
	  {
	    tmp = myDelays [i] ; myDelays [i] = myDelays [k] ; myDelays [k] = tmp ;
	    tmp = myPins   [i] ; myPins   [i] = myPins   [k] ; myPins   [k] = tmp ;
	  }
	}

// All on

    lastDelay = 0 ;
    for (servo = 0 ; servo < MAX_SERVOS ; ++servo)
    {
      if ((pin = myPins [servo]) == -1)
	continue ;

      digitalWrite (pin, HIGH) ;
      myDelays [servo] = myDelays [servo] - lastDelay ;
      lastDelay += myDelays [servo] ;
    }

// Now loop, turning them all off as required

    for (servo = 0 ; servo < MAX_SERVOS ; ++servo)
    {
      if ((pin = myPins [servo]) == -1)
	continue ;

      delayMicroseconds (myDelays [servo]) ;
      digitalWrite (pin, LOW) ;
    }

// Wait until the end of an 8mS time-slot

    gettimeofday (&tNow, NULL) ;
    timersub (&tNow, &tStart, &tPeriod) ;
    timersub (&tTotal, &tPeriod, &tGap) ;
    tNs.tv_sec  = tGap.tv_sec ;
    tNs.tv_nsec = tGap.tv_usec * 1000 ;
    nanosleep (&tNs, NULL) ;
  }

  return NULL ;
}


/*
 * softServoWrite:
 *	Write a Servo value to the given pin
 *********************************************************************************
 */

void softServoWrite (int servoPin, int value)
{
  int servo ;

  servoPin &= 63 ;

  /**/ if (value < -250)
    value = -250 ;
  else if (value > 1250)
    value = 1250 ;

  for (servo = 0 ; servo < MAX_SERVOS ; ++servo)
    if (pinMap [servo] == servoPin)
      pulseWidth [servo] = value + 1000 ; // uS
}


/*
 * softServoSetup:
 *	Setup the software servo system
 *********************************************************************************
 */

int softServoSetup (int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7)
{
  int servo ;

  if (p0 != -1) { pinMode (p0, OUTPUT) ; digitalWrite (p0, LOW) ; }
  if (p1 != -1) { pinMode (p1, OUTPUT) ; digitalWrite (p1, LOW) ; }
  if (p2 != -1) { pinMode (p2, OUTPUT) ; digitalWrite (p2, LOW) ; }
  if (p3 != -1) { pinMode (p3, OUTPUT) ; digitalWrite (p3, LOW) ; }
  if (p4 != -1) { pinMode (p4, OUTPUT) ; digitalWrite (p4, LOW) ; }
  if (p5 != -1) { pinMode (p5, OUTPUT) ; digitalWrite (p5, LOW) ; }
  if (p6 != -1) { pinMode (p6, OUTPUT) ; digitalWrite (p6, LOW) ; }
  if (p7 != -1) { pinMode (p7, OUTPUT) ; digitalWrite (p7, LOW) ; }

  pinMap [0] = p0 ;
  pinMap [1] = p1 ;
  pinMap [2] = p2 ;
  pinMap [3] = p3 ;
  pinMap [4] = p4 ;
  pinMap [5] = p5 ;
  pinMap [6] = p6 ;
  pinMap [7] = p7 ;

  for (servo = 0 ; servo < MAX_SERVOS ; ++servo)
    pulseWidth [servo] = 1500 ;		// Mid point
  
  return piThreadCreate (softServoThread) ;
}
