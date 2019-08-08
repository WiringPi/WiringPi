/*
 * wiringPi.h:
 *	Arduino like Wiring library for the Raspberry Pi.
 *	Copyright (c) 2012-2017 Gordon Henderson
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

#ifndef	__WIRING_PI_H__
#define	__WIRING_PI_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>


#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(!TRUE)
#endif

#define PIN_NUM_CALC_SYSFD(x) (x>225 ? x - libwiring.pinBase : x)

// GCC warning suppressor
#define	UNU	__attribute__((unused))

#define ENV_DEBUG	"WIRINGPI_DEBUG"
#define ENV_CODES   "WIRINGPI_CODES"
#define ENV_GPIOMEM	"WIRINGPI_GPIOMEM"

#define MODEL_UNKNOWN		0
#define MODEL_KHADAS_VIM1	1
#define MODEL_KHADAS_VIM2	2
#define MODEL_KHADAS_VIM3	3
#define	MODEL_KHADAS_EDGE	4

#define MAKER_UNKNOWN		0
#define MAKER_AMLOGIC		1
#define MAKER_ROCKCHIP		2

// wiringPi modes

#define	MODE_PINS		  	 0
#define	MODE_GPIO		 	 1
#define	MODE_GPIO_SYS	  	 2
#define	MODE_PHYS			 3
#define MODE_PIFACE			 4
#define	MODE_UNINITIALISED	-1

// Pin modes

#define	INPUT			 0
#define	OUTPUT			 1
#define	PWM_OUTPUT		 2
#define	GPIO_CLOCK		 3
#define	SOFT_PWM_OUTPUT		 4
#define	SOFT_TONE_OUTPUT	 5
#define	PWM_TONE_OUTPUT		 6

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

//Module names
#define AML_MODULE_I2C		"aml_i2c"

extern const char *piModelNames    [16] ;
extern const char *piRevisionNames [16] ;
extern const char *piMakerNames    [16] ;
extern const int   piMemorySize    [ 8] ;


//	Intended for the GPIO program Use at your own risk.

// Threads

#define	PI_THREAD(X)	void *X (UNU void *dummy)

// Failure modes

#define	WPI_FATAL	(1==1)
#define	WPI_ALMOST	(1==2)

#if !defined(ANDROID)
#define PAGE_SIZE	(4*1024)
#endif
#define BLOCK_SIZE	(4*1024)

#define MSG_ERR 	-1
#define MSG_WARN	-2

struct libkhadas
{
	/* H/W model info */
	int model, rev, mem, maker;

	/* wiringPi init mode */
	int mode;

	/* wiringPi core func */
	int (*getModeToGpio)    (int mode, int pin);
	void    (*setPadDrive)      (int pin, int value);
	int (*getPadDrive)      (int pin);
	void    (*pinMode)      (int pin, int mode);
	int (*getAlt)       (int pin);
	int (*getPUPD)      (int pin);
	void    (*pullUpDnControl)  (int pin, int pud);
	int (*digitalRead)      (int pin);
	void    (*digitalWrite)     (int pin, int value);
	int (*analogRead)       (int pin);
	void    (*digitalWriteByte) (const int value);
	unsigned int (*digitalReadByte) (void);

	/* ISR Function pointer */
	void    (*isrFunctions[256])(void);

	/* GPIO sysfs file discripter */
	int     sysFds[256];

	/* GPIO pin base number */
	int pinBase;

	// Time for easy calculations
	uint64_t epochMilli, epochMicro ;
};


union reg_bitfield {
	unsigned int wvalue;
	struct {
		unsigned int 	bit0  : 1;
		unsigned int 	bit1  : 1;
		unsigned int 	bit2  : 1;
		unsigned int 	bit3  : 1;
		unsigned int 	bit4  : 1;
		unsigned int 	bit5  : 1;
		unsigned int 	bit6  : 1;
		unsigned int 	bit7  : 1;
		unsigned int 	bit8  : 1;
		unsigned int 	bit9  : 1;
		unsigned int 	bit10 : 1;
		unsigned int 	bit11 : 1;
		unsigned int 	bit12 : 1;
		unsigned int 	bit13 : 1;
		unsigned int 	bit14 : 1;
		unsigned int 	bit15 : 1;
		unsigned int 	bit16 : 1;
		unsigned int 	bit17 : 1;
		unsigned int 	bit18 : 1;
		unsigned int 	bit19 : 1;
		unsigned int 	bit20 : 1;
		unsigned int 	bit21 : 1;
		unsigned int 	bit22 : 1;
		unsigned int 	bit23 : 1;
		unsigned int 	bit24 : 1;
		unsigned int 	bit25 : 1;
		unsigned int 	bit26 : 1;
		unsigned int 	bit27 : 1;
		unsigned int 	bit28 : 1;
		unsigned int 	bit29 : 1;
		unsigned int 	bit30 : 1;
		unsigned int 	bit31 : 1;

	} bits;
};


// wiringPiNodeStruct:
//	This describes additional device nodes in the extended wiringPi
//	2.0 scheme of things.
//	It's a simple linked list for now, but will hopefully migrate to 
//	a binary tree for efficiency reasons - but then again, the chances
//	of more than 1 or 2 devices being added are fairly slim, so who
//	knows....

struct wiringPiNodeStruct
{
  int     pinBase ;
  int     pinMax ;

  int          fd ;	// Node specific
  unsigned int data0 ;	//  ditto
  unsigned int data1 ;	//  ditto
  unsigned int data2 ;	//  ditto
  unsigned int data3 ;	//  ditto

           void   (*pinMode)          (struct wiringPiNodeStruct *node, int pin, int mode) ;
           void   (*pullUpDnControl)  (struct wiringPiNodeStruct *node, int pin, int mode) ;
           int    (*digitalRead)      (struct wiringPiNodeStruct *node, int pin) ;
//unsigned int    (*digitalRead8)     (struct wiringPiNodeStruct *node, int pin) ;
           void   (*digitalWrite)     (struct wiringPiNodeStruct *node, int pin, int value) ;
//         void   (*digitalWrite8)    (struct wiringPiNodeStruct *node, int pin, int value) ;
           void   (*pwmWrite)         (struct wiringPiNodeStruct *node, int pin, int value) ;
           int    (*analogRead)       (struct wiringPiNodeStruct *node, int pin) ;
           void   (*analogWrite)      (struct wiringPiNodeStruct *node, int pin, int value) ;

  struct wiringPiNodeStruct *next ;
} ;

extern struct wiringPiNodeStruct *wiringPiNodes ;

// Export variables for the hardware pointers

//extern volatile unsigned int *_wiringPiGpio ;
//extern volatile unsigned int *_wiringPiPwm ;
//extern volatile unsigned int *_wiringPiClk ;
//extern volatile unsigned int *_wiringPiPads ;
//extern volatile unsigned int *_wiringPiTimer ;
//extern volatile unsigned int *_wiringPiTimerIrqRaw ;


// Function prototypes
//	c++ wrappers thanks to a comment by Nick Lott
//	(and others on the Raspberry Pi forums)

#ifdef __cplusplus
extern "C" {
#endif

// Data

// Internal WiringPi functions

extern int wiringPiFailure (int fatal, const char *message, ...) ;
extern int msg (int type, const char *message, ...);
extern int moduleLoaded (char *);

// Core wiringPi functions

extern struct wiringPiNodeStruct *wiringPiFindNode (int pin) ;
extern struct wiringPiNodeStruct *wiringPiNewNode  (int pinBase, int numPins) ;

extern void wiringPiVersion	(int *major, int *minor) ;
extern int  wiringPiSetup       (void) ;
extern int  wiringPiSetupSys    (void) ;
extern int  wiringPiSetupGpio   (void) ;
extern int  wiringPiSetupPhys   (void) ;

extern 			int	 getModeToGpio		(int wpiPin);
extern 			void setPadDrive		(int pin, int value);
extern 			int  getPadDrive		(int pin);
extern			int  getAlt				(int pin);
extern			int  getPUPD			(int pin);
extern 			void pinMode			(int pin,int mode);
extern 			void pullUpDnControl	(int pin, int pud);
extern 			int  digitalRead		(int pin);
extern 			void digitalWrite		(int pin, int value);
extern unsigned int  digitalReadByte	(void);
extern			void digitalWriteByte	(const int vaule);
extern			void pwmWrite			(int pin, int value);
extern 			int  analogRead			(int pin);

//Hardware Specific stuffs
extern			int  piGpioLayout		(void);
extern 			int  piBoardRev			(void);
extern 			void piBoardId			(int *model, int *rev, int *mem, int *maker, int *warranty);
extern 			int  wpiPinToGpio 		(int wpiPin);
extern 			int  physPinToGpio		(int physPin);

//Unsupported
extern 			void pinModeAlt			(int pin, int mode);
extern 			void analogWrite		(int pin, int value);
extern 			void pwmToneWrite		(int pin, int freq);
extern			void pwmSetMode			(int mode);
extern 			void pwmSetRange		(unsigned int range);
extern			void pwmSetClock		(int divisor);
extern			void gpioClockSet		(int pin, int freq);
extern unsigned int  digitalReadByte	(void);
extern unsigned int  digitalReadByte2	(void);
extern			void digitalWriteByte	(int value);
extern 			void digitalWriteByte2	(int value);

//Interrupt
extern			int  waitForInterrupt	(int pin, int mS);
extern			int  wiringPiISR		(int pin, int mode, void (*function)(void));

//Threads
extern			int  piThreadCreate		(void *(*fn)(void *));
extern			void piLock				(int key);
extern			void piUnlock			(int key);

//Schedulling priority
extern		int  piHiPri		(const int pri);

//From Arduino land
extern			void delay				(unsigned int howLang);
extern			void delayMicroseconds	(unsigned int howLang);
extern unsigned int  millis				(void);
extern unsigned int  micros				(void);

#ifdef __cplusplus
}
#endif

#endif  /* __WIRING_H__ */
