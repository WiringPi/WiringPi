/*
 * wiringPi:
 *	Arduino compatable (ish) Wiring library for the Raspberry Pi
 *	Copyright (c) 2012 Gordon Henderson
 *
 *	Thanks to code samples from Gert Jan van Loo and the
 *	BCM2835 ARM Peripherals manual, however it's missing
 *	the clock section /grr/mutter/
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

// Revisions:
//	19 Jul 2012:
//		Moved to the LGPL
//		Added an abstraction layer to the main routines to save a tiny
//		bit of run-time and make the clode a little cleaner (if a little
//		larger)
//		Added waitForInterrupt code
//		Added piHiPri code
//
//	 9 Jul 2012:
//		Added in support to use the /sys/class/gpio interface.
//	 2 Jul 2012:
//		Fixed a few more bugs to do with range-checking when in GPIO mode.
//	11 Jun 2012:
//		Fixed some typos.
//		Added c++ support for the .h file
//		Added a new function to allow for using my "pin" numbers, or native
//			GPIO pin numbers.
//		Removed my busy-loop delay and replaced it with a call to delayMicroseconds
//
//	02 May 2012:
//		Added in the 2 UART pins
//		Change maxPins to numPins to more accurately reflect purpose

// Pad drive current fiddling

#undef	DEBUG_PADS

#include <stdio.h>
#include <stdint.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "wiringPi.h"

// Function stubs

void (*pinMode)           (int pin, int mode) ;
void (*pullUpDnControl)   (int pin, int pud) ;
void (*digitalWrite)      (int pin, int value) ;
void (*pwmWrite)          (int pin, int value) ;
void (*setPadDrive)       (int group, int value) ;
int  (*digitalRead)       (int pin) ;
int  (*waitForInterrupt)  (int pin, int mS) ;
void (*delayMicroseconds) (unsigned int howLong) ;
void (*pwmSetMode)        (int mode) ;
void (*pwmSetRange)       (unsigned int range) ;


#ifndef	TRUE
#define	TRUE	(1==1)
#define	FALSE	(1==2)
#endif

// BCM Magic

#define	BCM_PASSWORD		0x5A000000


// Port function select bits

#define	FSEL_INPT		0b000
#define	FSEL_OUTP		0b001
#define	FSEL_ALT0		0b100
#define	FSEL_ALT0		0b100
#define	FSEL_ALT1		0b101
#define	FSEL_ALT2		0b110
#define	FSEL_ALT3		0b111
#define	FSEL_ALT4		0b011
#define	FSEL_ALT5		0b010

// Access from ARM Running Linux
//	Take from Gert/Doms code. Some of this is not in the manual
//	that I can find )-:

#define BCM2708_PERI_BASE	                   0x20000000
#define GPIO_PADS		(BCM2708_PERI_BASE + 0x100000)
#define CLOCK_BASE		(BCM2708_PERI_BASE + 0x101000)
#define GPIO_BASE		(BCM2708_PERI_BASE + 0x200000)
#define GPIO_TIMER		(BCM2708_PERI_BASE + 0x00B000)
#define GPIO_PWM		(BCM2708_PERI_BASE + 0x20C000)

#define	PAGE_SIZE		(4*1024)
#define	BLOCK_SIZE		(4*1024)

// PWM

#define	PWM_CONTROL 0
#define	PWM_STATUS  1
#define	PWM0_RANGE  4
#define	PWM0_DATA   5
#define	PWM1_RANGE  8
#define	PWM1_DATA   9

#define	PWMCLK_CNTL	40
#define	PWMCLK_DIV	41

#define	PWM1_MS_MODE    0x8000  // Run in MS mode
#define	PWM1_USEFIFO    0x2000  // Data from FIFO
#define	PWM1_REVPOLAR   0x1000  // Reverse polarity
#define	PWM1_OFFSTATE   0x0800  // Ouput Off state
#define	PWM1_REPEATFF   0x0400  // Repeat last value if FIFO empty
#define	PWM1_SERIAL     0x0200  // Run in serial mode
#define	PWM1_ENABLE     0x0100  // Channel Enable

#define	PWM0_MS_MODE    0x0080  // Run in MS mode
#define	PWM0_USEFIFO    0x0020  // Data from FIFO
#define	PWM0_REVPOLAR   0x0010  // Reverse polarity
#define	PWM0_OFFSTATE   0x0008  // Ouput Off state
#define	PWM0_REPEATFF   0x0004  // Repeat last value if FIFO empty
#define	PWM0_SERIAL     0x0002  // Run in serial mode
#define	PWM0_ENABLE     0x0001  // Channel Enable

// Timer

#define	TIMER_LOAD	(0x400 >> 2)
#define	TIMER_VALUE	(0x404 >> 2)
#define	TIMER_CONTROL	(0x408 >> 2)
#define	TIMER_IRQ_CLR	(0x40C >> 2)
#define	TIMER_IRQ_RAW	(0x410 >> 2)
#define	TIMER_IRQ_MASK	(0x414 >> 2)
#define	TIMER_RELOAD	(0x418 >> 2)
#define	TIMER_PRE_DIV	(0x41C >> 2)
#define	TIMER_COUNTER	(0x420 >> 2)

// Locals to hold pointers to the hardware

static volatile uint32_t *gpio ;
static volatile uint32_t *pwm ;
static volatile uint32_t *clk ;
static volatile uint32_t *pads ;
static volatile uint32_t *timer ;

static volatile uint32_t *timerIrqRaw ;

// The BCM2835 has 54 GPIO pins.
//	BCM2835 data sheet, Page 90 onwards.
//	There are 6 control registers, each control the functions of a block
//	of 10 pins.
//	Each control register has 10 sets of 3 bits per GPIO pin:
//
//	000 = GPIO Pin X is an input
//	001 = GPIO Pin X is an output
//	100 = GPIO Pin X takes alternate function 0
//	101 = GPIO Pin X takes alternate function 1
//	110 = GPIO Pin X takes alternate function 2
//	111 = GPIO Pin X takes alternate function 3
//	011 = GPIO Pin X takes alternate function 4
//	010 = GPIO Pin X takes alternate function 5
//
// So the 3 bits for port X are:
//	X / 10 + ((X % 10) * 3)

// sysFds:
//	Map a file descriptor from the /sys/class/gpio/gpioX/value

static int sysFds [64] ;

// Doing it the Arduino way with lookup tables...
//	Yes, it's probably more innefficient than all the bit-twidling, but it
//	does tend to make it all a bit clearer. At least to me!

// pinToGpio:
//	Take a Wiring pin (0 through X) and re-map it to the BCM_GPIO pin

static int pinToGpio [64] =
{
  17, 18, 21, 22, 23, 24, 25, 4,	// From the Original Wiki - GPIO 0 through 7
   0,  1,				// I2C  - SDA0, SCL0
   8,  7,				// SPI  - CE1, CE0
  10,  9, 11, 				// SPI  - MOSI, MISO, SCLK
  14, 15,				// UART - Tx, Rx

// Padding:

          -1, -1, -1,-1,-1,-1,-1, -1, -1, -1, -1, -1, -1, -1, -1,	// ... 31
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// ... 47
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// ... 63
} ;

// gpioToGPFSEL:
//	Map a BCM_GPIO pin to it's control port. (GPFSEL 0-5)

static uint8_t gpioToGPFSEL [] =
{
  0,0,0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,
  4,4,4,4,4,4,4,4,4,4,
  5,5,5,5,5,5,5,5,5,5,
} ;

// gpioToShift
//	Define the shift up for the 3 bits per pin in each GPFSEL port

static uint8_t gpioToShift [] =
{
  0,3,6,9,12,15,18,21,24,27,
  0,3,6,9,12,15,18,21,24,27,
  0,3,6,9,12,15,18,21,24,27,
  0,3,6,9,12,15,18,21,24,27,
  0,3,6,9,12,15,18,21,24,27,
} ;

// gpioToGPSET:
//	(Word) offset to the GPIO Set registers for each GPIO pin

static uint8_t gpioToGPSET [] =
{
   7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
   8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
} ;

// gpioToGPCLR:
//	(Word) offset to the GPIO Clear registers for each GPIO pin

static uint8_t gpioToGPCLR [] =
{
  10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
  11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
} ;

// gpioToGPLEV:
//	(Word) offset to the GPIO Input level registers for each GPIO pin

static uint8_t gpioToGPLEV [] =
{
  13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
  14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
} ;

#ifdef notYetReady
// gpioToEDS
//	(Word) offset to the Event Detect Status

static uint8_t gpioToEDS [] =
{
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
} ;

// gpioToREN
//	(Word) offset to the Rising edgde ENable register

static uint8_t gpioToREN [] =
{
  19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,
  20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
} ;

// gpioToFEN
//	(Word) offset to the Falling edgde ENable register

static uint8_t gpioToFEN [] =
{
  22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,
  23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
} ;
#endif

// gpioToPUDCLK
//	(Word) offset to the Pull Up Down Clock regsiter

#define	GPPUD	37

static uint8_t gpioToPUDCLK [] =
{
  38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,
  39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,
} ;

// gpioToPwmALT
//	the ALT value to put a GPIO pin into PWM mode

static uint8_t gpioToPwmALT [] =
{
          0,         0,         0,         0,         0,         0,         0,         0,	//  0 ->  7
          0,         0,         0,         0, FSEL_ALT0, FSEL_ALT0,         0,         0, 	//  8 -> 15
          0,         0, FSEL_ALT5, FSEL_ALT5,         0,         0,         0,         0, 	// 16 -> 23
          0,         0,         0,         0,         0,         0,         0,         0,	// 24 -> 31
          0,         0,         0,         0,         0,         0,         0,         0,	// 32 -> 39
  FSEL_ALT0, FSEL_ALT0,         0,         0,         0, FSEL_ALT0,         0,         0,	// 40 -> 47
          0,         0,         0,         0,         0,         0,         0,         0,	// 48 -> 55
          0,         0,         0,         0,         0,         0,         0,         0,	// 56 -> 63
} ;

static uint8_t gpioToPwmPort [] =
{
          0,         0,         0,         0,         0,         0,         0,         0,	//  0 ->  7
          0,         0,         0,         0, PWM0_DATA, PWM1_DATA,         0,         0, 	//  8 -> 15
          0,         0, PWM0_DATA, PWM1_DATA,         0,         0,         0,         0, 	// 16 -> 23
          0,         0,         0,         0,         0,         0,         0,         0,	// 24 -> 31
          0,         0,         0,         0,         0,         0,         0,         0,	// 32 -> 39
  PWM0_DATA, PWM1_DATA,         0,         0,         0, PWM1_DATA,         0,         0,	// 40 -> 47
          0,         0,         0,         0,         0,         0,         0,         0,	// 48 -> 55
          0,         0,         0,         0,         0,         0,         0,         0,	// 56 -> 63

} ;


// Time for easy calculations

static unsigned long long epoch ;

//////////////////////////////////////////////////////////////////////////////////


/*
 * pinMode:
 *	Sets the mode of a pin to be input, output or PWM output
 *********************************************************************************
 */

void pinModeGpio (int pin, int mode)
{
  static int pwmRunning  = FALSE ;
  int fSel, shift, alt ;

  pin &= 63 ;

  fSel    = gpioToGPFSEL [pin] ;
  shift   = gpioToShift  [pin] ;

  /**/ if (mode == INPUT)
    *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) ; // Sets bits to zero = input
  else if (mode == OUTPUT)
    *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) | (1 << shift) ;
  else if (mode == PWM_OUTPUT)
  {
    if ((alt = gpioToPwmALT [pin]) == 0)	// Not a PWM pin
      return ;

// Set pin to PWM mode

    *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) | (alt << shift) ;

// We didn't initialise the PWM hardware at setup time - because it's possible that
//	something else is using the PWM - e.g. the Audio systems! So if we use PWM
//	here, then we're assuming that nothing else is, otherwise things are going
//	to sound a bit funny...

    if (!pwmRunning)
    {

      *(pwm + PWM_CONTROL) = 0 ;			// Stop PWM
      delayMicroseconds (10) ;
	
//	Gert/Doms Values
      *(clk + PWMCLK_DIV)  = BCM_PASSWORD | (32<<12) ;	// set pwm div to 32 (19.2/32 = 600KHz)
      *(clk + PWMCLK_CNTL) = BCM_PASSWORD | 0x11 ;	// Source=osc and enable

      delayMicroseconds (10) ;

      *(pwm + PWM0_RANGE) = 0x400 ; delayMicroseconds (10) ;
      *(pwm + PWM1_RANGE) = 0x400 ; delayMicroseconds (10) ;

// Enable PWMs

      *(pwm + PWM0_DATA) = 512 ;
      *(pwm + PWM1_DATA) = 512 ;

// Balanced mode (default)

      *(pwm + PWM_CONTROL) = PWM0_ENABLE | PWM1_ENABLE ;

      pwmRunning = TRUE ;
    }

  }

// When we change mode of any pin, we remove the pull up/downs
//	Or we used to... Hm. Commented out now because for some wieird reason,
//	it seems to block subsequent attempts to set the pull up/downs and I've
//	not quite gotten to the bottom of why this happens
//	The down-side is that the pull up/downs are rememberd in the SoC between
//	power cycles, so it's going to be a good idea to explicitly set them in
//	any new code.
//
//  pullUpDnControl (pin, PUD_OFF) ;

}

void pinModeWPi (int pin, int mode)
{
  pinModeGpio (pinToGpio [pin & 63], mode) ;
}

void pinModeSys (int pin, int mode)
{
  return ;
}


/*
 * pwmControl:
 *	Allow the user to control some of the PWM functions
 *********************************************************************************
 */

void pwmSetModeWPi (int mode)
{
  if (mode == PWM_MODE_MS)
    *(pwm + PWM_CONTROL) = PWM0_ENABLE | PWM1_ENABLE | PWM0_MS_MODE | PWM1_MS_MODE ;
  else
    *(pwm + PWM_CONTROL) = PWM0_ENABLE | PWM1_ENABLE ;
}

void pwmSetModeSys (int mode)
{
  return ;
}


void pwmSetRangeWPi (unsigned int range)
{
  *(pwm + PWM0_RANGE) = range ; delayMicroseconds (10) ;
  *(pwm + PWM1_RANGE) = range ; delayMicroseconds (10) ;
}

void pwmSetRangeSys (unsigned int range)
{
  return ;
}


#ifdef notYetReady
/*
 * pinED01:
 * pinED10:
 *	Enables edge-detect mode on a pin - from a 0 to a 1 or 1 to 0
 *	Pin must already be in input mode with appropriate pull up/downs set.
 *********************************************************************************
 */

void pinEnableED01Pi (int pin)
{
  pin = pinToGpio [pin & 63] ;
}
#endif



/*
 * digitalWrite:
 *	Set an output bit
 *********************************************************************************
 */

void digitalWriteWPi (int pin, int value)
{
  pin = pinToGpio [pin & 63] ;

  if (value == LOW)
    *(gpio + gpioToGPCLR [pin]) = 1 << (pin & 31) ;
  else
    *(gpio + gpioToGPSET [pin]) = 1 << (pin & 31) ;
}

void digitalWriteGpio (int pin, int value)
{
  pin &= 63 ;

  if (value == LOW)
    *(gpio + gpioToGPCLR [pin]) = 1 << (pin & 31) ;
  else
    *(gpio + gpioToGPSET [pin]) = 1 << (pin & 31) ;
}

void digitalWriteSys (int pin, int value)
{
  pin &= 63 ;

  if (sysFds [pin] != -1)
  {
    if (value == LOW)
      write (sysFds [pin], "0\n", 2) ;
    else
      write (sysFds [pin], "1\n", 2) ;
  }
}


/*
 * pwnWrite:
 *	Set an output PWM value
 *********************************************************************************
 */

void pwmWriteGpio (int pin, int value)
{
  int port ;

  pin  = pin & 63 ;
  port = gpioToPwmPort [pin] ;

  *(pwm + port) = value ;
}

void pwmWriteWPi (int pin, int value)
{
  pwmWriteGpio (pinToGpio [pin & 63], value) ;
}

void pwmWriteSys (int pin, int value)
{
  return ;
}


/*
 * setPadDrive:
 *	Set the PAD driver value
 *********************************************************************************
 */

void setPadDriveWPi (int group, int value)
{
  uint32_t wrVal ;

  if ((group < 0) || (group > 2))
    return ;

  wrVal = BCM_PASSWORD | 0x18 | (value & 7) ;
  *(pads + group + 11) = wrVal ;

#ifdef	DEBUG_PADS
  printf ("setPadDrive: Group: %d, value: %d (%08X)\n", group, value, wrVal) ;
  printf ("Read : %08X\n", *(pads + group + 11)) ;
#endif
}

void setPadDriveGpio (int group, int value)
{
  setPadDriveWPi (group, value) ;
}

void setPadDriveSys (int group, int value)
{
  return ;
}


/*
 * digitalRead:
 *	Read the value of a given Pin, returning HIGH or LOW
 *********************************************************************************
 */

int digitalReadWPi (int pin)
{
  pin = pinToGpio [pin & 63] ;

  if ((*(gpio + gpioToGPLEV [pin]) & (1 << (pin & 31))) != 0)
    return HIGH ;
  else
    return LOW ;
}

int digitalReadGpio (int pin)
{
  pin &= 63 ;

  if ((*(gpio + gpioToGPLEV [pin]) & (1 << (pin & 31))) != 0)
    return HIGH ;
  else
    return LOW ;
}

int digitalReadSys (int pin)
{
  char c ;

  pin &= 63 ;

  if (sysFds [pin] == -1)
    return 0 ;

  lseek (sysFds [pin], 0L, SEEK_SET) ;
  read  (sysFds [pin], &c, 1) ;
  return (c == '0') ? 0 : 1 ;
}


/*
 * pullUpDownCtrl:
 *	Control the internal pull-up/down resistors on a GPIO pin
 *	The Arduino only has pull-ups and these are enabled by writing 1
 *	to a port when in input mode - this paradigm doesn't quite apply
 *	here though.
 *********************************************************************************
 */

void pullUpDnControlGpio (int pin, int pud)
{
  pin &= 63 ;
  pud &=  3 ;

  *(gpio + GPPUD)              = pud ;			delayMicroseconds (5) ;
  *(gpio + gpioToPUDCLK [pin]) = 1 << (pin & 31) ;	delayMicroseconds (5) ;
  
  *(gpio + GPPUD)              = 0 ;			delayMicroseconds (5) ;
  *(gpio + gpioToPUDCLK [pin]) = 0 ;			delayMicroseconds (5) ;
}

void pullUpDnControlWPi (int pin, int pud)
{
  pullUpDnControlGpio (pinToGpio [pin & 63], pud) ;
}

void pullUpDnControlSys (int pin, int pud)
{
  return ;
}


/*
 * waitForInterrupt:
 *	Wait for Interrupt on a GPIO pin.
 *	This is actually done via the /sys/class/gpio interface regardless of
 *	the wiringPi access mode in-use. Maybe sometime it might get a better
 *	way for a bit more efficiency.
 *********************************************************************************
 */

int waitForInterruptSys (int pin, int mS)
{
  int fd, x ;
  char buf [8] ;
  struct pollfd polls ;

  if ((fd = sysFds [pin & 63]) == -1)
    return -2 ;

// Do a dummy read

  x = read (fd, buf, 6) ;
  if (x < 0)
    return x ;

// And seek

  lseek (fd, 0, SEEK_SET) ;

// Setup poll structure

  polls.fd     = fd ;
  polls.events = POLLPRI ;	// Urgent data!

// Wait for it ...

  return poll (&polls, 1, mS) ;
}

int waitForInterruptWPi (int pin, int mS)
{
  return waitForInterruptSys (pinToGpio [pin & 63], mS) ;
}

int waitForInterruptGpio (int pin, int mS)
{
  return waitForInterruptSys (pin, mS) ;
}




/*
 * delay:
 *	Wait for some number of milli seconds
 *********************************************************************************
 */

void delay (unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, &dummy) ;
}


/*
 * delayMicroseconds:
 *	This is somewhat intersting. It seems that on the Pi, a single call
 *	to nanosleep takes some 80 to 130 microseconds anyway, so while
 *	obeying the standards (may take longer), it's not always what we
 *	want!
 *
 *	So what I'll do now is if the delay is less than 100uS we'll do it
 *	in a hard loop, watching a built-in counter on the ARM chip. This is
 *	somewhat sub-optimal in that it uses 100% CPU, something not an issue
 *	in a microcontroller, but under a multi-tasking, multi-user OS, it's
 *	wastefull, however we've no real choice )-:
 *********************************************************************************
 */

void delayMicrosecondsSys (unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = 0 ;
  sleeper.tv_nsec = (long)(howLong * 1000) ;

  nanosleep (&sleeper, &dummy) ;
}

void delayMicrosecondsHard (unsigned int howLong)
{
  *(timer + TIMER_LOAD)    = howLong ;
  *(timer + TIMER_IRQ_CLR) = 0 ;

  while (*timerIrqRaw == 0)
    ;
}

void delayMicrosecondsWPi (unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  /**/ if (howLong ==   0)
    return ;
  else if (howLong  < 100)
    delayMicrosecondsHard (howLong) ;
  else
  {
    sleeper.tv_sec  = 0 ;
    sleeper.tv_nsec = (long)(howLong * 1000) ;
    nanosleep (&sleeper, &dummy) ;
  }
}


/*
 * millis:
 *	Return a number of milliseconds as an unsigned int.
 *********************************************************************************
 */

unsigned int millis (void)
{
  struct timeval tv ;
  unsigned long long t1 ;

  gettimeofday (&tv, NULL) ;

  t1 = (tv.tv_sec * 1000000 + tv.tv_usec) / 1000 ;

  return (uint32_t)(t1 - epoch) ;
}


/*
 * wiringPiSetup:
 *	Must be called once at the start of your program execution.
 *
 * Default setup: Initialises the system into wiringPi Pin mode and uses the
 *	memory mapped hardware directly.
 *********************************************************************************
 */

int wiringPiSetup (void)
{
  int      fd ;
  uint8_t *gpioMem, *pwmMem, *clkMem, *padsMem, *timerMem ;
  struct timeval tv ;

            pinMode =           pinModeWPi ;
    pullUpDnControl =   pullUpDnControlWPi ;
       digitalWrite =      digitalWriteWPi ;
           pwmWrite =          pwmWriteWPi ;
        setPadDrive =       setPadDriveWPi ;
        digitalRead =       digitalReadWPi ;
   waitForInterrupt =  waitForInterruptWPi ;
  delayMicroseconds = delayMicrosecondsWPi ;
         pwmSetMode =        pwmSetModeWPi ;
        pwmSetRange =       pwmSetRangeWPi ;
  
// Open the master /dev/memory device

  if ((fd = open ("/dev/mem", O_RDWR | O_SYNC) ) < 0)
  {
    fprintf (stderr, "wiringPiSetup: Unable to open /dev/mem: %s\n", strerror (errno)) ;
    return -1 ;
  }

// GPIO:

// Allocate 2 pages - 1 ...

  if ((gpioMem = malloc (BLOCK_SIZE + (PAGE_SIZE-1))) == NULL)
  {
    fprintf (stderr, "wiringPiSetup: malloc failed: %s\n", strerror (errno)) ;
    return -1 ;
  }

// ... presumably to make sure we can round it up to a whole page size

  if (((uint32_t)gpioMem % PAGE_SIZE) != 0)
    gpioMem += PAGE_SIZE - ((uint32_t)gpioMem % PAGE_SIZE) ;

  gpio = (uint32_t *)mmap((caddr_t)gpioMem, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, fd, GPIO_BASE) ;

  if ((int32_t)gpio < 0)
  {
    fprintf (stderr, "wiringPiSetup: mmap failed: %s\n", strerror (errno)) ;
    return -1 ;
  }

// PWM

  if ((pwmMem = malloc (BLOCK_SIZE + (PAGE_SIZE-1))) == NULL)
  {
    fprintf (stderr, "wiringPiSetup: pwmMem malloc failed: %s\n", strerror (errno)) ;
    return -1 ;
  }

  if (((uint32_t)pwmMem % PAGE_SIZE) != 0)
    pwmMem += PAGE_SIZE - ((uint32_t)pwmMem % PAGE_SIZE) ;

  pwm = (uint32_t *)mmap(pwmMem, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, fd, GPIO_PWM) ;

  if ((int32_t)pwm < 0)
  {
    fprintf (stderr, "wiringPiSetup: mmap failed (pwm): %s\n", strerror (errno)) ;
    return -1 ;
  }
 
// Clock control (needed for PWM)

  if ((clkMem = malloc (BLOCK_SIZE + (PAGE_SIZE-1))) == NULL)
  {
    fprintf (stderr, "wiringPiSetup: clkMem malloc failed: %s\n", strerror (errno)) ;
    return -1 ;
  }

  if (((uint32_t)clkMem % PAGE_SIZE) != 0)
    clkMem += PAGE_SIZE - ((uint32_t)clkMem % PAGE_SIZE) ;

  clk = (uint32_t *)mmap(clkMem, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, fd, CLOCK_BASE) ;

  if ((int32_t)clk < 0)
  {
    fprintf (stderr, "wiringPiSetup: mmap failed (clk): %s\n", strerror (errno)) ;
    return -1 ;
  }
 
// The drive pads

  if ((padsMem = malloc (BLOCK_SIZE + (PAGE_SIZE-1))) == NULL)
  {
    fprintf (stderr, "wiringPiSetup: padsMem malloc failed: %s\n", strerror (errno)) ;
    return -1 ;
  }

  if (((uint32_t)padsMem % PAGE_SIZE) != 0)
    padsMem += PAGE_SIZE - ((uint32_t)padsMem % PAGE_SIZE) ;

  pads = (uint32_t *)mmap(padsMem, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, fd, GPIO_PADS) ;

  if ((int32_t)pads < 0)
  {
    fprintf (stderr, "wiringPiSetup: mmap failed (pads): %s\n", strerror (errno)) ;
    return -1 ;
  }

#ifdef	DEBUG_PADS
  printf ("Checking pads @ 0x%08X\n", (unsigned int)pads) ;
  printf (" -> %08X %08X %08X\n", *(pads + 11), *(pads + 12), *(pads + 13)) ;
#endif

// The system timer

  if ((timerMem = malloc (BLOCK_SIZE + (PAGE_SIZE-1))) == NULL)
  {
    fprintf (stderr, "wiringPiSetup: timerMem malloc failed: %s\n", strerror (errno)) ;
    return -1 ;
  }

  if (((uint32_t)timerMem % PAGE_SIZE) != 0)
    timerMem += PAGE_SIZE - ((uint32_t)timerMem % PAGE_SIZE) ;

  timer = (uint32_t *)mmap(timerMem, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, fd, GPIO_TIMER) ;

  if ((int32_t)timer < 0)
  {
    fprintf (stderr, "wiringPiSetup: mmap failed (timer): %s\n", strerror (errno)) ;
    return -1 ;
  }

// Set the timer to free-running, 1MHz.
//	0xF9 is 249, the timer divide is base clock / (divide+1)
//	so base clock is 250MHz / 250 = 1MHz.

  *(timer + TIMER_CONTROL) = 0x0000280 ;
  *(timer + TIMER_PRE_DIV) = 0x00000F9 ;
  timerIrqRaw = timer + TIMER_IRQ_RAW ;

// Initialise our epoch for millis()

  gettimeofday (&tv, NULL) ;
  epoch = (tv.tv_sec * 1000000 + tv.tv_usec) / 1000 ;

  return 0 ;
}


/*
 * wiringPiSetupGpio:
 *	Must be called once at the start of your program execution.
 *
 * GPIO setup: Initialises the system into GPIO Pin mode and uses the
 *	memory mapped hardware directly.
 *********************************************************************************
 */

int wiringPiSetupGpio (void)
{
  int x = wiringPiSetup () ;

  if (x != 0)
    return x ;

            pinMode =           pinModeGpio ;
    pullUpDnControl =   pullUpDnControlGpio ;
       digitalWrite =      digitalWriteGpio ;
           pwmWrite =          pwmWriteGpio ;
        setPadDrive =       setPadDriveGpio ;
        digitalRead =       digitalReadGpio ;
   waitForInterrupt =  waitForInterruptGpio ;
  delayMicroseconds = delayMicrosecondsWPi ;	// Same
         pwmSetMode =        pwmSetModeWPi ;
        pwmSetRange =       pwmSetRangeWPi ;

  return 0 ;
}


/*
 * wiringPiSetupSys:
 *	Must be called once at the start of your program execution.
 *
 * Initialisation (again), however this time we are using the /sys/class/gpio
 *	interface to the GPIO systems - slightly slower, but always usable as
 *	a non-root user, assuming the devices are already exported and setup correctly.
 */

int wiringPiSetupSys (void)
{
  int pin ;
  struct timeval tv ;
  char fName [128] ;

            pinMode =           pinModeSys ;
    pullUpDnControl =   pullUpDnControlSys ;
       digitalWrite =      digitalWriteSys ;
           pwmWrite =          pwmWriteSys ;
        setPadDrive =       setPadDriveSys ;
        digitalRead =       digitalReadSys ;
   waitForInterrupt =  waitForInterruptSys ;
  delayMicroseconds = delayMicrosecondsSys ;
         pwmSetMode =        pwmSetModeSys ;
        pwmSetRange =       pwmSetRangeSys ;


// Open and scan the directory, looking for exported GPIOs, and pre-open
//	the 'value' interface to speed things up for later
  
  for (pin = 0 ; pin < 64 ; ++pin)
  {
    sprintf (fName, "/sys/class/gpio/gpio%d/value", pin) ;
    sysFds [pin] = open (fName, O_RDWR) ;
  }

// Initialise the epoch for mills() ...

  gettimeofday (&tv, NULL) ;
  epoch = (tv.tv_sec * 1000000 + tv.tv_usec) / 1000 ;

  return 0 ;
}
