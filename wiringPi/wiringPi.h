/*
 * wiringPi:
 *	Arduino compatable (ish) Wiring library for the Raspberry Pi
 *	Copyright (c) 2012 Gordon Henderson
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

// Handy defines

// Deprecated
#define	NUM_PINS	17

#define	WPI_MODE_PINS		 0
#define	WPI_MODE_GPIO		 1
#define	WPI_MODE_GPIO_SYS	 2
#define	WPI_MODE_PIFACE		 3
#define	WPI_MODE_UNINITIALISED	-1

// Pin modes

#define	INPUT			 0
#define	OUTPUT			 1
#define	PWM_OUTPUT		 2
#define	GPIO_CLOCK		 3

#define	LOW			 0
#define	HIGH			 1

// Pull up/down/none

#define	PUD_OFF			 0
#define	PUD_DOWN		 1
#define	PUD_UP			 2

// PWM

#define	PWM_MODE_MS		0
#define	PWM_MODE_BAL		1

// Interrupt levels

#define	INT_EDGE_SETUP		0
#define	INT_EDGE_FALLING	1
#define	INT_EDGE_RISING		2
#define	INT_EDGE_BOTH		3

// Threads

#define	PI_THREAD(X)	void *X (void *dummy)


// Function prototypes
//	c++ wrappers thanks to a comment by Nick Lott
//	(and others on the Raspberry Pi forums)

#ifdef __cplusplus
extern "C" {
#endif

// Basic wiringPi functions

extern int  wiringPiSetup       (void) ;
extern int  wiringPiSetupSys    (void) ;
extern int  wiringPiSetupGpio   (void) ;
extern int  wiringPiSetupPiFace (void) ;

extern int  piBoardRev          (void) ;
extern int  wpiPinToGpio        (int wpiPin) ;

extern int  wiringPiSetupPiFaceForGpioProg (void) ;	// Don't use this - for gpio program only

extern void (*pinMode)           (int pin, int mode) ;
extern int  (*getAlt)            (int pin) ;
extern void (*pullUpDnControl)   (int pin, int pud) ;
extern void (*digitalWrite)      (int pin, int value) ;
extern void (*digitalWriteByte)  (int value) ;
extern void (*gpioClockSet)      (int pin, int freq) ;
extern void (*pwmWrite)          (int pin, int value) ;
extern void (*setPadDrive)       (int group, int value) ;
extern int  (*digitalRead)       (int pin) ;
extern void (*pwmSetMode)        (int mode) ;
extern void (*pwmSetRange)       (unsigned int range) ;
extern void (*pwmSetClock)       (int divisor) ;

// Interrupts

extern int  (*waitForInterrupt) (int pin, int mS) ;
extern int  wiringPiISR         (int pin, int mode, void (*function)(void)) ;

// Threads

extern int  piThreadCreate (void *(*fn)(void *)) ;
extern void piLock         (int key) ;
extern void piUnlock       (int key) ;

// Schedulling priority

extern int piHiPri (int pri) ;


// Extras from arduino land

extern void         delay             (unsigned int howLong) ;
extern void         delayMicroseconds (unsigned int howLong) ;
extern unsigned int millis            (void) ;
extern unsigned int micros            (void) ;

#ifdef __cplusplus
}
#endif
