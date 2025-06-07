/*
 * wiringPi:
 *	Arduino look-a-like Wiring library for the Raspberry Pi
 *	Copyright (c) 2012-2025 Gordon Henderson and contributors
 *	Additional code for pwmSetClock by Chris Hall <chris@kchall.plus.com>
 *
 *	Thanks to code samples from Gert Jan van Loo and the
 *	BCM2835 ARM Peripherals manual, however it's missing
 *	the clock section /grr/mutter/
 ***********************************************************************
 * This file is part of wiringPi:
 *    https://github.com/WiringPi/WiringPi
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


#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <byteswap.h>
#include <sys/utsname.h>
#include <linux/gpio.h>
#include <dirent.h>
#include <inttypes.h>

#include "softPwm.h"
#include "softTone.h"

#include "wiringPi.h"
#include "../version.h"
#include "wiringPiLegacy.h"

// Environment Variables

#define	ENV_DEBUG	"WIRINGPI_DEBUG"
#define	ENV_CODES	"WIRINGPI_CODES"
#define	ENV_GPIOMEM	"WIRINGPI_GPIOMEM"


// Extend wiringPi with other pin-based devices and keep track of
//	them in this structure

struct wiringPiNodeStruct *wiringPiNodes = NULL ;

// BCM Magic

#define	BCM_PASSWORD		0x5A000000


// The BCM2835 has 54 GPIO pins.
//	BCM2835 data sheet, Page 90 onwards.
//	There are 6 control registers, each control the functions of a block
//	of 10 pins.
//	Each control register has 10 sets of 3 bits per GPIO pin - the ALT values
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

// Port function select bits

#define	FSEL_INPT		0b000 //0
#define	FSEL_OUTP		0b001 //1
#define	FSEL_ALT0		0b100 //4
#define	FSEL_ALT1		0b101 //5
#define	FSEL_ALT2		0b110 //6
#define	FSEL_ALT3		0b111 //7
#define	FSEL_ALT4		0b011 //3
#define	FSEL_ALT5		0b010 //2
//RP1 defines
#define	FSEL_ALT6		8
#define	FSEL_ALT7		9
#define	FSEL_ALT8		10
#define	FSEL_ALT9		11


//RP1 chip (@Pi5) - 3.1.1. Function select
#define RP1_FSEL_ALT0			0x00
#define RP1_FSEL_GPIO			0x05  //SYS_RIO
#define RP1_FSEL_NONE			0x09
#define RP1_FSEL_NONE_HW	0x1f  //default, mask

// maybe faster then piRP1Model
#define ISRP1MODEL (PI_MODEL_5==RaspberryPiModel || PI_MODEL_CM5==RaspberryPiModel|| PI_MODEL_500==RaspberryPiModel || PI_MODEL_CM5L==RaspberryPiModel)
//RP1 chip (@Pi5) RIO address
const unsigned int RP1_RIO_OUT = 0x0000;
const unsigned int RP1_RIO_OE  = (0x0004/4);
const unsigned int RP1_RIO_IN  = (0x0008/4);

//RP1 chip (@Pi5) RIO offset for set/clear value
const unsigned int RP1_SET_OFFSET = (0x2000/4);
const unsigned int RP1_CLR_OFFSET = (0x3000/4);

//RP1 chip (@Pi5) PDE/PDU pull-up/-down enable
const unsigned int RP1_PUD_UP = (1<<3);
const unsigned int RP1_PUD_DOWN = (1<<2);
const unsigned int RP1_INV_PUD_MASK = ~(RP1_PUD_UP | RP1_PUD_DOWN); //~0x0C

//RP1 chip (@Pi5) pin level, status register
const unsigned int RP1_STATUS_LEVEL_LOW  = 0x00400000;
const unsigned int RP1_STATUS_LEVEL_HIGH = 0x00800000;
const unsigned int RP1_STATUS_LEVEL_MASK = 0x00C00000;

const unsigned int RP1_DEBOUNCE_DEFAULT_VALUE = 4;
const unsigned int RP1_DEBOUNCE_MASK    = 0x7f;
const unsigned int RP1_DEBOUNCE_DEFAULT = (RP1_DEBOUNCE_DEFAULT_VALUE << 5);

const unsigned int RP1_IRQRESET = 0x10000000; //CTRL Bit 28

const unsigned int RP1_PAD_DEFAULT_0TO8      = (0x0B | 0x70);  //Slewfast, Schmitt, PullUp,   | 12mA, Input enable
const unsigned int RP1_PAD_DEFAULT_FROM9     = (0x07 | 0x70);  //Slewfast, Schmitt, PullDown, | 12mA, Input enable
const unsigned int RP1_PAD_IC_DEFAULT_0TO8  = 0x9A; //pull-up, Schmitt
const unsigned int RP1_PAD_IC_DEFAULT_FROM9 = 0x96; //pull-down, Schmitt

const unsigned int RP1_PAD_DRIVE_MASK   = 0x00000030;
const unsigned int RP1_INV_PAD_DRIVE_MASK = ~(RP1_PAD_DRIVE_MASK);

const unsigned int RP1_PWM0_GLOBAL_CTRL = 0;
const unsigned int RP1_PWM0_FIFO_CTRL   = 1;
const unsigned int RP1_PWM0_COMMON_RANGE= 2;
const unsigned int RP1_PWM0_COMMON_DUTY = 3;
const unsigned int RP1_PWM0_DUTY_FIFO   = 4;
const unsigned int RP1_PWM0_CHAN_START  = 5;
//offset channel
const unsigned int RP1_PWM0_CHAN_CTRL  = 0;
const unsigned int RP1_PWM0_CHAN_RANGE = 1;
const unsigned int RP1_PWM0_CHAN_PHASE = 2;
const unsigned int RP1_PWM0_CHAN_DUTY  = 3;
const unsigned int RP1_PWM0_CHAN_OFFSET= 4;

const unsigned int RP1_PWM0_CHAN0_RANGE = RP1_PWM0_CHAN_START+RP1_PWM0_CHAN_OFFSET*0+RP1_PWM0_CHAN_RANGE;
const unsigned int RP1_PWM0_CHAN1_RANGE = RP1_PWM0_CHAN_START+RP1_PWM0_CHAN_OFFSET*1+RP1_PWM0_CHAN_RANGE;
const unsigned int RP1_PWM0_CHAN2_RANGE = RP1_PWM0_CHAN_START+RP1_PWM0_CHAN_OFFSET*2+RP1_PWM0_CHAN_RANGE;
const unsigned int RP1_PWM0_CHAN3_RANGE = RP1_PWM0_CHAN_START+RP1_PWM0_CHAN_OFFSET*3+RP1_PWM0_CHAN_RANGE;

const unsigned int RP1_PWM_CTRL_SETUPDATE = 0x80000000; // Bit 32
const unsigned int RP1_PWM_TRAIL_EDGE_MS = 0x1;
const unsigned int RP1_PWM_FIFO_POP_MASK = 0x100; // Bit 8
const unsigned int RP1_CLK_PWM0_CTRL_DISABLE_MAGIC = 0x10000000;  // Default after boot
const unsigned int RP1_CLK_PWM0_CTRL_ENABLE_MAGIC  = 0x11000840;  // Reverse engineered, because of missing documentation, don't known meaning of of bits

const unsigned int CLK_PWM0_CTRL     = (0x00074/4);
const unsigned int CLK_PWM0_DIV_INT  = (0x00078/4);
const unsigned int CLK_PWM0_DIV_FRAC = (0x0007C/4);
const unsigned int CLK_PWM0_SEL	     = (0x00080/4);

//RP1 chip (@Pi5) address
const unsigned long long RP1_64_BASE_Addr = 0x1f000d0000;
const unsigned int RP1_BASE_Addr     = 0x40000000;
const unsigned int RP1_CLOCK_Addr    = 0x40018000;  // Adress is not mapped to gpiomem device, lower than RP1_IO0_Addr
const unsigned int RP1_PWM0_Addr     = 0x40098000;  // Adress is not mapped to gpiomem device, lower than RP1_IO0_Addr
const unsigned int RP1_IO0_Addr      = 0x400d0000;
const unsigned int RP1_SYS_RIO0_Addr = 0x400e0000;
const unsigned int RP1_PADS0_Addr    = 0x400f0000;


// Access from ARM Running Linux
//	Taken from Gert/Doms code. Some of this is not in the manual
//	that I can find )-:
//
// Updates in September 2015 - all now static variables (and apologies for the caps)
//	due to the Pi v2, v3, etc. and the new /dev/gpiomem interface

const char* gpiomem_global    = "/dev/mem";
const char* gpiomem_BCM       = "/dev/gpiomem";
const char* gpiomem_RP1       = "/dev/gpiomem0";
const int   gpiomem_RP1_Size  = 0x00030000;
// PCIe memory access, need to detect path / PCIe address
//dmesg: rp1 0000:01:00.0: bar1 len 0x400000, start 0x1f00000000, end 0x1f003fffff, flags, 0x40200
const char* pcie_path         = "/sys/bus/pci/devices";
//const char* pciemem_RP1_path  = "/sys/bus/pci/devices/0000:01:00.0";
//const char* pciemem_RP1       = "/sys/bus/pci/devices/0000:01:00.0/resource1";
char pciemem_RP1[512] = { '\0' };
const char* pciemem_RP1_bar   = "resource1";
const int   pciemem_RP1_Size  = 0x00400000;
//const unsigned short pciemem_RP1_Ventor= 0x1de4;
//const unsigned short pciemem_RP1_Device= 0x0001;
const char* pciemem_RP1_Ventor= "0x1de4";
const char* pciemem_RP1_Device= "0x0001";


static volatile unsigned int GPIO_PADS ;
static volatile unsigned int GPIO_CLOCK_ADR ;
static volatile unsigned int GPIO_BASE ;
static volatile unsigned int GPIO_TIMER ;
static volatile unsigned int GPIO_PWM ;
static volatile unsigned int GPIO_RIO ;

#define	PAGE_SIZE		(4*1024)
#define	BLOCK_SIZE		(4*1024)

static unsigned int usingGpioMem    = FALSE ;
static          int wiringPiSetuped = FALSE ;

// PWM
//	Word offsets into the PWM control region

#define	PWM_CONTROL 0
#define	PWM_STATUS  1
#define	PWM0_RANGE  4
#define	PWM0_DATA   5
#define	PWM1_RANGE  8
#define	PWM1_DATA   9

//	Clock regsiter offsets

#define	PWMCLK_CNTL	40
#define	PWMCLK_DIV	41

#define	PWM0_MS_MODE    0x0080  // Run in MS mode
#define	PWM0_USEFIFO    0x0020  // Data from FIFO
#define	PWM0_REVPOLAR   0x0010  // Reverse polarity
#define	PWM0_OFFSTATE   0x0008  // Ouput Off state
#define	PWM0_REPEATFF   0x0004  // Repeat last value if FIFO empty
#define	PWM0_SERIAL     0x0002  // Run in serial mode
#define	PWM0_ENABLE     0x0001  // Channel Enable

#define	PWM1_MS_MODE    0x8000  // Run in MS mode
#define	PWM1_USEFIFO    0x2000  // Data from FIFO
#define	PWM1_REVPOLAR   0x1000  // Reverse polarity
#define	PWM1_OFFSTATE   0x0800  // Ouput Off state
#define	PWM1_REPEATFF   0x0400  // Repeat last value if FIFO empty
#define	PWM1_SERIAL     0x0200  // Run in serial mode
#define	PWM1_ENABLE     0x0100  // Channel Enable

const int PWMCLK_DIVI_MAX = 0xFFF; // 3 Byte max size for Clock devider
const int OSC_FREQ_DEFAULT = 192; // x100kHz OSC
const int OSC_FREQ_BCM2711 = 540; // x100kHz OSC
const int OSC_FREQ_BCM2712 = 500; // x100kHz OSC  -  cat /sys/kernel/debug/clk/clk_summary | grep pwm0

// Timer
//	Word offsets

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

static volatile unsigned int *base ;
static volatile unsigned int *gpio ;
static volatile unsigned int *pwm ;
static volatile unsigned int *clk ;
static volatile unsigned int *pads ;
static volatile unsigned int *timer ;
static volatile unsigned int *timerIrqRaw ;
static volatile unsigned int *rio ;

// Export variables for the hardware pointers

volatile unsigned int *_wiringPiBase ;
volatile unsigned int *_wiringPiGpio ;
volatile unsigned int *_wiringPiPwm ;
volatile unsigned int *_wiringPiClk ;
volatile unsigned int *_wiringPiPads ;
volatile unsigned int *_wiringPiTimer ;
volatile unsigned int *_wiringPiTimerIrqRaw ;
volatile unsigned int *_wiringPiRio ;

// Data for use with the boardId functions.
//	The order of entries here to correspond with the PI_MODEL_X
//	and PI_VERSION_X defines in wiringPi.h
//	Only intended for the gpio command - use at your own risk!

// piGpioBase:
//	The base address of the GPIO memory mapped hardware IO

#define	GPIO_PERI_BASE_OLD  0x20000000
#define	GPIO_PERI_BASE_2835 0x3F000000
#define	GPIO_PERI_BASE_2711 0xFE000000
#define	GPIO_PERI_BASE_2712 0x00  //unknown - 32-bit mapped global mem access not supported for now

static volatile unsigned int piGpioBase = 0 ;

const char *piModelNames [PI_MODELS_MAX] =
{
  "Model A",	//  0
  "Model B",	//  1
  "Model A+",	//  2
  "Model B+",	//  3
  "Pi 2",	//  4
  "Alpha",	//  5
  "CM",		//  6
  "Unknown07",	// 07
  "Pi 3",	// 08
  "Pi Zero",	// 09
  "CM3",	// 10
  "Unknown11",	// 11
  "Pi Zero-W",	// 12
  "Pi 3B+",	// 13
  "Pi 3A+",	// 14
  "Unknown15",	// 15
  "CM3+",	// 16
  "Pi 4B",	// 17
  "Pi Zero2-W",	// 18
  "Pi 400",	// 19
  "CM4",	// 20
  "CM4S",	// 21
  "Unknown22",	// 22
  "Pi 5",	// 23
  "CM5",	// 24
  "Pi 500",	// 25
  "CM5 Lite",	// 26
} ;

const char *piProcessor [5] =
{
  "BCM2835",
  "BCM2836",
  "BCM2837",
  "BCM2711",
  "BCM2712",
} ;

const char *piRevisionNames [16] =
{
  "00",
  "01",
  "02",
  "03",
  "04",
  "05",
  "06",
  "07",
  "08",
  "09",
  "10",
  "11",
  "12",
  "13",
  "14",
  "15",
} ;

const char *piMakerNames [16] =
{
  "Sony UK",//	 0
  "Egoman",	//	 1
  "Embest",	//	 2
  "Sony Japan",//	 3
  "Embest",	//	 4
  "Stadium",//	 5
  "Unknown06",	//	 6
  "Unknown07",	//	 7
  "Unknown08",	//	 8
  "Unknown09",	//	 9
  "Unknown10",	//	10
  "Unknown11",	//	11
  "Unknown12",	//	12
  "Unknown13",	//	13
  "Unknown14",	//	14
  "Unknown15",	//	15
} ;

const int piMemorySize [8] =
{
   256,		//	 0
   512,		//	 1
  1024,		//	 2
  2048,		//	 3
  4096,		//	 4
  8192,		//	 5
 16384,		//	 6
     0,		//	 7
} ;

// Time for easy calculations

static uint64_t epochMilli, epochMicro ;

// Misc

static int wiringPiMode = WPI_MODE_UNINITIALISED ;
static volatile int    pinPass = -1 ;
static pthread_mutex_t pinMutex ;

static int RaspberryPiModel  = -1;
static int RaspberryPiLayout = -1;

// Debugging & Return codes

int wiringPiDebug       = FALSE ;
int wiringPiReturnCodes = FALSE ;

// Use /dev/gpiomem ?

int wiringPiTryGpioMem  = FALSE ;

enum WPIFlag {
  WPI_FLAG_INPUT    = 0x04,
  WPI_FLAG_OUTPUT   = 0x08,
  WPI_FLAG_BIAS_UP  = 0x100,
  WPI_FLAG_BIAS_DOWN= 0x200,
  WPI_FLAG_BIAS_OFF = 0x400,
};


static unsigned int lineFlags [64] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
} ;

static int lineFds [64] =
{
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
} ;

static int isrFds [64] =
{
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
} ;



// ISR Data
static int chipFd = -1;
static void* isrUserdata[64];
static void (*isrFunctionsV2[64])(struct WPIWfiStatus, void* userdata) ;
static void (*isrFunctions [64])(void) ;
static pthread_t isrThreads[64];
static int isrEdgeMode[64];             // irq on rising/falling edge
static unsigned long isrDebouncePeriodUs[64];      // 0: debounce is off

// Doing it the Arduino way with lookup tables...
//	Yes, it's probably more innefficient than all the bit-twidling, but it
//	does tend to make it all a bit clearer. At least to me!

// pinToGpio:
//	Take a Wiring pin (0 through X) and re-map it to the BCM_GPIO pin
//	Cope for 3 different board revisions here.

static int *pinToGpio ;

// Revision 1, 1.1:

static int pinToGpioR1 [64] =
{
  17, 18, 21, 22, 23, 24, 25, 4,	// From the Original Wiki - GPIO 0 through 7:	wpi  0 -  7
   0,  1,				// I2C  - SDA1, SCL1				wpi  8 -  9
   8,  7,				// SPI  - CE1, CE0				wpi 10 - 11
  10,  9, 11, 				// SPI  - MOSI, MISO, SCLK			wpi 12 - 14
  14, 15,				// UART - Tx, Rx				wpi 15 - 16

// Padding:

      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// ... 31
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// ... 47
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// ... 63
} ;

// Revision 2:

static int pinToGpioR2 [64] =
{
  17, 18, 27, 22, 23, 24, 25, 4,	// From the Original Wiki - GPIO 0 through 7:	wpi  0 -  7
   2,  3,				// I2C  - SDA0, SCL0				wpi  8 -  9
   8,  7,				// SPI  - CE1, CE0				wpi 10 - 11
  10,  9, 11, 				// SPI  - MOSI, MISO, SCLK			wpi 12 - 14
  14, 15,				// UART - Tx, Rx				wpi 15 - 16
  28, 29, 30, 31,			// Rev 2: New GPIOs 8 though 11			wpi 17 - 20
   5,  6, 13, 19, 26,			// B+						wpi 21, 22, 23, 24, 25
  12, 16, 20, 21,			// B+						wpi 26, 27, 28, 29
   0,  1,				// B+						wpi 30, 31

// Padding:

  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// ... 47
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// ... 63
} ;


// physToGpio:
//	Take a physical pin (1 through 26) and re-map it to the BCM_GPIO pin
//	Cope for 2 different board revisions here.
//	Also add in the P5 connector, so the P5 pins are 3,4,5,6, so 53,54,55,56

static int *physToGpio ;

static int physToGpioR1 [64] =
{
  -1,		// 0
  -1, -1,	// 1, 2
   0, -1,
   1, -1,
   4, 14,
  -1, 15,
  17, 18,
  21, -1,
  22, 23,
  -1, 24,
  10, -1,
   9, 25,
  11,  8,
  -1,  7,	// 25, 26

                                              -1, -1, -1, -1, -1,	// ... 31
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// ... 47
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// ... 63
} ;

static int physToGpioR2 [64] =
{
  -1,		// 0
  -1, -1,	// 1, 2
   2, -1,
   3, -1,
   4, 14,
  -1, 15,
  17, 18,
  27, -1,
  22, 23,
  -1, 24,
  10, -1,
   9, 25,
  11,  8,
  -1,  7,	// 25, 26

// B+

   0,  1,
   5, -1,
   6, 12,
  13, -1,
  19, 16,
  26, 20,
  -1, 21,

// the P5 connector on the Rev 2 boards:

  -1, -1,
  -1, -1,
  -1, -1,
  -1, -1,
  -1, -1,
  28, 29,
  30, 31,
  -1, -1,
  -1, -1,
  -1, -1,
  -1, -1,
} ;


int piBoard() {
  if (RaspberryPiModel<0) { //need to detect pi model
    int   model, rev, mem, maker, overVolted;
    piBoardId (&model, &rev, &mem, &maker, &overVolted);
  }
  return RaspberryPiModel<0 ? 0 : 1;
}


int piBoard40Pin() {
  if (!piBoard()){
	  // Board not detected
	  return -1;
  }
  switch(RaspberryPiModel){
	case PI_MODEL_A:
	case PI_MODEL_B:
		return 0;
// PI_MODEL_CM
// PI_MODEL_CM3
// PI_MODEL_CM4  
// PI_MODEL_CM4S
//     ? guess yes
	default: 
		return 1;
  }
}


int piRP1Model() {
  switch(RaspberryPiModel){
    case PI_MODEL_5:
    case PI_MODEL_CM5:
    case PI_MODEL_500:
    case PI_MODEL_CM5L:
      return 1;
    default:
      return 0;
   }
}


int GetMaxPin() {
  if (piRP1Model()) {
    switch(wiringPiMode) {
      case WPI_MODE_PHYS:
        return 40;
      case WPI_MODE_PINS:
        return 31;
      default:
        return 27;
    }
  } else {
    return 63;
  }
}


int ToBCMPin(int* pin) {
  if (*pin<0 || *pin>63) {
    return FALSE;
  }
  switch(wiringPiMode) {
    case WPI_MODE_PINS:
      *pin = pinToGpio[*pin];
      break;
    case WPI_MODE_PHYS:
      *pin = physToGpio[*pin];
      break;
    case WPI_MODE_GPIO:
      return TRUE;
    default:
      return FALSE;
  }
  if (piRP1Model() && *pin>27) {
    return FALSE;
  }
  return TRUE;
}


#define RETURN_ON_MODEL5 if (piRP1Model()) { if (wiringPiDebug) printf("Function not supported on Pi5\n");  return; }

int FailOnModel5(const char *function) {
  if (piRP1Model()) {
    return wiringPiFailure (WPI_ALMOST, "Function '%s' not supported on Raspberry Pi 5.\n"
  "  Unable to continue. Keep an eye of new versions at https://github.com/wiringpi/wiringpi\n", function) ;
  }
  return 0;
}

// gpioToGPFSEL:
//	Map a BCM_GPIO pin to it's Function Selection
//	control port. (GPFSEL 0-5)
//	Groups of 10 - 3 bits per Function - 30 bits per port

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
//	(Word) offset to the Rising edge ENable register

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


// GPPUD:
//	GPIO Pin pull up/down register

#define	GPPUD	37

/* 2711 has a different mechanism for pin pull-up/down/enable  */
#define GPPUPPDN0                57        /* Pin pull-up/down for pins 15:0  */
#define GPPUPPDN1                58        /* Pin pull-up/down for pins 31:16 */
#define GPPUPPDN2                59        /* Pin pull-up/down for pins 47:32 */
#define GPPUPPDN3                60        /* Pin pull-up/down for pins 57:48 */

static volatile unsigned int piGpioPupOffset = 0 ;

// gpioToPUDCLK
//	(Word) offset to the Pull Up Down Clock regsiter

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
          0,         0,         0,         0, FSEL_ALT0, FSEL_ALT0,         0,         0, //  8 -> 15
          0,         0, FSEL_ALT5, FSEL_ALT5,         0,         0,         0,         0, // 16 -> 23
          0,         0,         0,         0,         0,         0,         0,         0,	// 24 -> 31
          0,         0,         0,         0,         0,         0,         0,         0,	// 32 -> 39
  FSEL_ALT0, FSEL_ALT0,         0,         0,         0, FSEL_ALT0,         0,         0,	// 40 -> 47
          0,         0,         0,         0,         0,         0,         0,         0,	// 48 -> 55
          0,         0,         0,         0,         0,         0,         0,         0,	// 56 -> 63
} ;


// gpioToPwmPort
//	The port value to put a GPIO pin into PWM mode

static uint8_t gpioToPwmPort [] =
{
          0,         0,         0,         0,         0,         0,         0,         0,	//  0 ->  7
          0,         0,         0,         0, PWM0_DATA, PWM1_DATA,         0,         0, //  8 -> 15
          0,         0, PWM0_DATA, PWM1_DATA,         0,         0,         0,         0, // 16 -> 23
          0,         0,         0,         0,         0,         0,         0,         0,	// 24 -> 31
          0,         0,         0,         0,         0,         0,         0,         0,	// 32 -> 39
  PWM0_DATA, PWM1_DATA,         0,         0,         0, PWM1_DATA,         0,         0,	// 40 -> 47
          0,         0,         0,         0,         0,         0,         0,         0,	// 48 -> 55
          0,         0,         0,         0,         0,         0,         0,         0,	// 56 -> 63

} ;

// gpioToGpClkALT:
//	ALT value to put a GPIO pin into GP Clock mode.
//	On the Pi we can really only use BCM_GPIO_4 and BCM_GPIO_21
//	for clocks 0 and 1 respectively, however I'll include the full
//	list for completeness - maybe one day...

#define	GPIO_CLOCK_SOURCE	1

// gpioToGpClkALT0:

static uint8_t gpioToGpClkALT0 [] =
{
          0,         0,         0,         0, FSEL_ALT0, FSEL_ALT0, FSEL_ALT0,         0,	//  0 ->  7
          0,         0,         0,         0,         0,         0,         0,         0, 	//  8 -> 15
          0,         0,         0,         0, FSEL_ALT5, FSEL_ALT5,         0,         0, 	// 16 -> 23
          0,         0,         0,         0,         0,         0,         0,         0,	// 24 -> 31
  FSEL_ALT0,         0, FSEL_ALT0,         0,         0,         0,         0,         0,	// 32 -> 39
          0,         0, FSEL_ALT0, FSEL_ALT0, FSEL_ALT0,         0,         0,         0,	// 40 -> 47
          0,         0,         0,         0,         0,         0,         0,         0,	// 48 -> 55
          0,         0,         0,         0,         0,         0,         0,         0,	// 56 -> 63
} ;

// gpioToClk:
//	(word) Offsets to the clock Control and Divisor register

static uint8_t gpioToClkCon [] =
{
         -1,        -1,        -1,        -1,        28,        30,        32,        -1,	//  0 ->  7
         -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1, 	//  8 -> 15
         -1,        -1,        -1,        -1,        28,        30,        -1,        -1, 	// 16 -> 23
         -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,	// 24 -> 31
         28,        -1,        28,        -1,        -1,        -1,        -1,        -1,	// 32 -> 39
         -1,        -1,        28,        30,        28,        -1,        -1,        -1,	// 40 -> 47
         -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,	// 48 -> 55
         -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,	// 56 -> 63
} ;

static uint8_t gpioToClkDiv [] =
{
         -1,        -1,        -1,        -1,        29,        31,        33,        -1,	//  0 ->  7
         -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1, 	//  8 -> 15
         -1,        -1,        -1,        -1,        29,        31,        -1,        -1, 	// 16 -> 23
         -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,	// 24 -> 31
         29,        -1,        29,        -1,        -1,        -1,        -1,        -1,	// 32 -> 39
         -1,        -1,        29,        31,        29,        -1,        -1,        -1,	// 40 -> 47
         -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,	// 48 -> 55
         -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,	// 56 -> 63
} ;


/*
 * Functions
 *********************************************************************************
 */


/*
 * wiringPiFailure:
 *	Fail. Or not.
 *********************************************************************************
 */

int wiringPiFailure (int fatal, const char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  if (!fatal && wiringPiReturnCodes)
    return -1 ;

  va_start (argp, message) ;
    vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  fprintf (stderr, "%s", buffer) ;
  exit (EXIT_FAILURE) ;

  return 0 ;
}


/*
 * setupCheck
 *	Another sanity check because some users forget to call the setup
 *	function. Mosty because they need feeding C drip by drip )-:
 *********************************************************************************
 */

static void setupCheck (const char *fName)
{
  if (!wiringPiSetuped)
  {
    fprintf (stderr, "%s: You have not called one of the wiringPiSetup\n"
	"  functions, so I'm aborting your program before it crashes anyway.\n", fName) ;
    exit (EXIT_FAILURE) ;
  }
}

/*
 * gpioMemCheck:
 *	See if we're using the /dev/gpiomem interface, if-so then some operations
 *	can't be done and will crash the Pi.
 *********************************************************************************
 */

static void usingGpioMemCheck (const char *what)
{
  if (usingGpioMem)
  {
    fprintf (stderr, "%s: Unable to do this when using /dev/gpiomem. Try sudo?\n", what) ;
    exit (EXIT_FAILURE) ;
  }

}


void PrintSystemStdErr () {
  struct utsname sys_info;
  if (uname(&sys_info) == 0) {
    fprintf (stderr, "      WiringPi    : %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
    fprintf (stderr, "      system name : %s\n", sys_info.sysname);
    //fprintf (stderr, "  node name   : %s\n", sys_info.nodename);
    fprintf (stderr, "      release     : %s\n", sys_info.release);
    fprintf (stderr, "      version     : %s\n", sys_info.version);
    fprintf (stderr, "      machine     : %s\n", sys_info.machine);
    if (strstr(sys_info.machine, "arm") == NULL && strstr(sys_info.machine, "aarch")==NULL) {
      fprintf (stderr, " -> This is not an ARM architecture; it cannot be a Raspberry Pi.\n") ;
    }
  }
}


void piFunctionOops (const char *function, const char* suggestion, const char* url)
{
  fprintf (stderr, "Oops: Function %s is not supported\n", function) ;
  PrintSystemStdErr();
  if (suggestion) {
    fprintf (stderr, " -> Please %s\n", suggestion) ;
  }
  if (url) {
    fprintf (stderr, " -> See info at %s\n", url) ;
  }
  fprintf (stderr, " -> Check at https://github.com/wiringpi/wiringpi/issues.\n\n") ;
  exit (EXIT_FAILURE) ;
}

void ReportDeviceError(const char *function, int pin, const char *mode, int ret) {
  fprintf(stderr, "wiringPi: ERROR: ioctl %s of %d (%s) returned error '%s' (%d)\n", function, pin, mode, strerror(errno), ret);
}


/*
 * piGpioLayout:
 *	Return a number representing the hardware revision of the board.
 *	This is not strictly the board revision but is used to check the
 *	layout of the GPIO connector - and there are 2 types that we are
 *	really interested in here. The very earliest Pi's and the
 *	ones that came after that which switched some pins ....
 *
 *	Revision 1 really means the early Model A and B's.
 *	Revision 2 is everything else - it covers the B, B+ and CM.
 *		... and the Pi 2 - which is a B+ ++  ...
 *		... and the Pi 0 - which is an A+ ...
 *
 *	The main difference between the revision 1 and 2 system that I use here
 *	is the mapping of the GPIO pins. From revision 2, the Pi Foundation changed
 *	3 GPIO pins on the (original) 26-way header - BCM_GPIO 22 was dropped and
 *	replaced with 27, and 0 + 1 - I2C bus 0 was changed to 2 + 3; I2C bus 1.
 *
 *	Additionally, here we set the piModel2 flag too. This is again, nothing to
 *	do with the actual model, but the major version numbers - the GPIO base
 *	hardware address changed at model 2 and above (not the Zero though)
 *
 *********************************************************************************
 */
 const char* revfile = "/proc/device-tree/system/linux,revision";

void piGpioLayoutOops (const char *why)
{
  fprintf (stderr, "Oops: Unable to determine Raspberry Pi board revision from %s and from /proc/cpuinfo\n", revfile) ;
  PrintSystemStdErr();
  fprintf (stderr, " -> %s\n", why) ;
  fprintf (stderr, " -> WiringPi is designed for Raspberry Pi and can only be used with a Raspberry Pi.\n\n") ;
  fprintf (stderr, " -> Check at https://github.com/wiringpi/wiringpi/issues.\n\n") ;
  exit (EXIT_FAILURE) ;
}

int piGpioLayout (void)
{
  piBoard();
  return RaspberryPiLayout;
}

/*
 * piBoardRev:
 *	Deprecated, but does the same as piGpioLayout
 *********************************************************************************
 */

int piBoardRev (void)
{
  return piGpioLayout () ;
}

const char* GetPiRevision(char* line, int linelength, unsigned int* revision) {

  const char* c = NULL;
  uint32_t Revision = 0;
  _Static_assert(sizeof(Revision)==4, "should be unsigend integer with 4 byte size");

	FILE* fp = fopen(revfile,"rb");
	if (!fp) {
    if (wiringPiDebug)
		  perror(revfile);
		return NULL; // revision file not found or no access
	}
	int result = fread(&Revision, sizeof(Revision), 1, fp);
	fclose(fp);
	if (result<1) {
    if (wiringPiDebug)
		  perror(revfile);
		return NULL; // read error
	}
	Revision = bswap_32(Revision);
	snprintf(line, linelength, "Revision\t: %04x", Revision);
  c =  &line[11];
  *revision = Revision;
  if (wiringPiDebug)
	  printf("GetPiRevision: Revision string: \"%s\" (%s) - 0x%x\n", line, c, *revision);
	return c;
}

/*
 * piBoardId:
 *	Return the real details of the board we have.
 *
 *	This is undocumented and really only intended for the GPIO command.
 *	Use at your own risk!
 *
 *	Seems there are some boards with 0000 in them (mistake in manufacture)
 *	So the distinction between boards that I can see is:
 *
 *		0000 - Error
 *		0001 - Not used
 *
 *	Original Pi boards:
 *		0002 - Model B,  Rev 1,   256MB, Egoman
 *		0003 - Model B,  Rev 1.1, 256MB, Egoman, Fuses/D14 removed.
 *
 *	Newer Pi's with remapped GPIO:
 *		0004 - Model B,  Rev 1.2, 256MB, Sony
 *		0005 - Model B,  Rev 1.2, 256MB, Egoman
 *		0006 - Model B,  Rev 1.2, 256MB, Egoman
 *
 *		0007 - Model A,  Rev 1.2, 256MB, Egoman
 *		0008 - Model A,  Rev 1.2, 256MB, Sony
 *		0009 - Model A,  Rev 1.2, 256MB, Egoman
 *
 *		000d - Model B,  Rev 1.2, 512MB, Egoman	(Red Pi, Blue Pi?)
 *		000e - Model B,  Rev 1.2, 512MB, Sony
 *		000f - Model B,  Rev 1.2, 512MB, Egoman
 *
 *		0010 - Model B+, Rev 1.2, 512MB, Sony
 *		0013 - Model B+  Rev 1.2, 512MB, Embest
 *		0016 - Model B+  Rev 1.2, 512MB, Sony
 *		0019 - Model B+  Rev 1.2, 512MB, Egoman
 *
 *		0011 - Pi CM,    Rev 1.1, 512MB, Sony
 *		0014 - Pi CM,    Rev 1.1, 512MB, Embest
 *		0017 - Pi CM,    Rev 1.1, 512MB, Sony
 *		001a - Pi CM,    Rev 1.1, 512MB, Egoman
 *
 *		0012 - Model A+  Rev 1.1, 256MB, Sony
 *		0015 - Model A+  Rev 1.1, 512MB, Embest
 *		0018 - Model A+  Rev 1.1, 256MB, Sony
 *		001b - Model A+  Rev 1.1, 256MB, Egoman
 *
 *	A small thorn is the olde style overvolting - that will add in
 *		1000000
 *
 *	The Pi compute module has an revision of 0011 or 0014 - since we only
 *	check the last digit, then it's 1, therefore it'll default to not 2 or
 *	3 for a	Rev 1, so will appear as a Rev 2. This is fine for the most part, but
 *	we'll properly detect the Compute Module later and adjust accordingly.
 *
 * And then things changed with the introduction of the v2...
 *
 * For Pi v2 and subsequent models - e.g. the Zero:
 *
 *   [USER:8] [NEW:1] [MEMSIZE:3] [MANUFACTURER:4] [PROCESSOR:4] [TYPE:8] [REV:4]
 *   NEW          23: will be 1 for the new scheme, 0 for the old scheme
 *   MEMSIZE      20: 0=256M 1=512M 2=1G
 *   MANUFACTURER 16: 0=SONY 1=EGOMAN 2=EMBEST
 *   PROCESSOR    12: 0=2835 1=2836
 *   TYPE         04: 0=MODELA 1=MODELB 2=MODELA+ 3=MODELB+ 4=Pi2 MODEL B 5=ALPHA 6=CM
 *   REV          00: 0=REV0 1=REV1 2=REV2
 *********************************************************************************
 */

void piBoardId (int *model, int *rev, int *mem, int *maker, int *warranty)
{
  const int maxlength = 120;
  char line [maxlength+1] ;
  const char *c ;
  unsigned int revision = 0x00 ;
  int bRev, bType, bProc, bMfg, bMem, bWarranty ;

  //piGpioLayoutOops ("this is only a test case");

  c = GetPiRevision(line, maxlength,  &revision); // device tree
  if (NULL==c) {
    c = GetPiRevisionLegacy(line, maxlength, &revision); // proc/cpuinfo
  }
  if (NULL==c) {
    piGpioLayoutOops ("GetPiRevision failed!") ;
  }

  if ((revision &  (1 << 23)) != 0)	// New style, not available for Raspberry Pi 1B/A, CM
  {
    if (wiringPiDebug)
      printf ("piBoardId: New Way: revision is: %08X\n", revision) ;

    bRev      = (revision & (0x0F <<  0)) >>  0 ;
    bType     = (revision & (0xFF <<  4)) >>  4 ;
    bProc     = (revision & (0x0F << 12)) >> 12 ;	// Not used for now.
    bMfg      = (revision & (0x0F << 16)) >> 16 ;
    bMem      = (revision & (0x07 << 20)) >> 20 ;
    bWarranty = (revision & (0x03 << 24)) != 0 ;

    // Ref: https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#raspberry-pi-revision-codes
    *model    = bType ;
    *rev      = bRev ;
    *mem      = bMem ;
    *maker    = bMfg  ;
    *warranty = bWarranty ;
    RaspberryPiLayout = GPIO_LAYOUT_DEFAULT ; //default

    if (wiringPiDebug)
      printf ("piBoardId: rev: %d, type: %d, proc: %d, mfg: %d, mem: %d, warranty: %d\n",
		bRev, bType, bProc, bMfg, bMem, bWarranty) ;
  }
  else					// Old way
  {
    if (wiringPiDebug)
      printf ("piBoardId: Old Way: revision is: %s\n", c) ;

    if (!isdigit (*c))
      piGpioLayoutOops ("Bogus \"Revision\" line (no digit at start of revision)") ;

// Make sure its long enough

    if (strlen (c) < 4)
      piGpioLayoutOops ("Bogus \"Revision\" line (not long enough)") ;

// If longer than 4, we'll assume it's been overvolted

    *warranty = strlen (c) > 4 ;

// Extract last 4 characters:

    c = c + strlen (c) - 4 ;

// Fill out the replys as appropriate
    RaspberryPiLayout = GPIO_LAYOUT_DEFAULT ; //default
    if      (strcmp (c, "0002") == 0) { *model = PI_MODEL_B  ; *rev = PI_VERSION_1   ; *mem = 0 ; *maker = PI_MAKER_EGOMAN ; RaspberryPiLayout = GPIO_LAYOUT_PI1_REV1; }
    else if (strcmp (c, "0003") == 0) { *model = PI_MODEL_B  ; *rev = PI_VERSION_1_1 ; *mem = 0 ; *maker = PI_MAKER_EGOMAN ; RaspberryPiLayout = GPIO_LAYOUT_PI1_REV1; }

    else if (strcmp (c, "0004") == 0) { *model = PI_MODEL_B  ; *rev = PI_VERSION_1_2 ; *mem = 0 ; *maker = PI_MAKER_SONY   ; }
    else if (strcmp (c, "0005") == 0) { *model = PI_MODEL_B  ; *rev = PI_VERSION_1_2 ; *mem = 0 ; *maker = PI_MAKER_EGOMAN ; }
    else if (strcmp (c, "0006") == 0) { *model = PI_MODEL_B  ; *rev = PI_VERSION_1_2 ; *mem = 0 ; *maker = PI_MAKER_EGOMAN ; }

    else if (strcmp (c, "0007") == 0) { *model = PI_MODEL_A  ; *rev = PI_VERSION_1_2 ; *mem = 0 ; *maker = PI_MAKER_EGOMAN ; }
    else if (strcmp (c, "0008") == 0) { *model = PI_MODEL_A  ; *rev = PI_VERSION_1_2 ; *mem = 0 ; *maker = PI_MAKER_SONY   ; }
    else if (strcmp (c, "0009") == 0) { *model = PI_MODEL_A  ; *rev = PI_VERSION_1_2 ; *mem = 0 ; *maker = PI_MAKER_EGOMAN ; }

    else if (strcmp (c, "000d") == 0) { *model = PI_MODEL_B  ; *rev = PI_VERSION_1_2 ; *mem = 1 ; *maker = PI_MAKER_EGOMAN ; }
    else if (strcmp (c, "000e") == 0) { *model = PI_MODEL_B  ; *rev = PI_VERSION_1_2 ; *mem = 1 ; *maker = PI_MAKER_SONY   ; }
    else if (strcmp (c, "000f") == 0) { *model = PI_MODEL_B  ; *rev = PI_VERSION_1_2 ; *mem = 1 ; *maker = PI_MAKER_EGOMAN ; }

    else if (strcmp (c, "0010") == 0) { *model = PI_MODEL_BP ; *rev = PI_VERSION_1_2 ; *mem = 1 ; *maker = PI_MAKER_SONY   ; }
    else if (strcmp (c, "0013") == 0) { *model = PI_MODEL_BP ; *rev = PI_VERSION_1_2 ; *mem = 1 ; *maker = PI_MAKER_EMBEST ; }
    else if (strcmp (c, "0016") == 0) { *model = PI_MODEL_BP ; *rev = PI_VERSION_1_2 ; *mem = 1 ; *maker = PI_MAKER_SONY   ; }
    else if (strcmp (c, "0019") == 0) { *model = PI_MODEL_BP ; *rev = PI_VERSION_1_2 ; *mem = 1 ; *maker = PI_MAKER_EGOMAN ; }

    else if (strcmp (c, "0011") == 0) { *model = PI_MODEL_CM ; *rev = PI_VERSION_1_1 ; *mem = 1 ; *maker = PI_MAKER_SONY   ; }
    else if (strcmp (c, "0014") == 0) { *model = PI_MODEL_CM ; *rev = PI_VERSION_1_1 ; *mem = 1 ; *maker = PI_MAKER_EMBEST ; }
    else if (strcmp (c, "0017") == 0) { *model = PI_MODEL_CM ; *rev = PI_VERSION_1_1 ; *mem = 1 ; *maker = PI_MAKER_SONY   ; }
    else if (strcmp (c, "001a") == 0) { *model = PI_MODEL_CM ; *rev = PI_VERSION_1_1 ; *mem = 1 ; *maker = PI_MAKER_EGOMAN ; }

    else if (strcmp (c, "0012") == 0) { *model = PI_MODEL_AP ; *rev = PI_VERSION_1_1 ; *mem = 0 ; *maker = PI_MAKER_SONY   ; }
    else if (strcmp (c, "0015") == 0) { *model = PI_MODEL_AP ; *rev = PI_VERSION_1_1 ; *mem = 1 ; *maker = PI_MAKER_EMBEST ; }
    else if (strcmp (c, "0018") == 0) { *model = PI_MODEL_AP ; *rev = PI_VERSION_1_1 ; *mem = 0 ; *maker = PI_MAKER_SONY   ; }
    else if (strcmp (c, "001b") == 0) { *model = PI_MODEL_AP ; *rev = PI_VERSION_1_1 ; *mem = 0 ; *maker = PI_MAKER_EGOMAN ; }

    else                              { *model = 0           ; *rev = 0              ; *mem = 0 ; *maker = 0               ; }
  }

  RaspberryPiModel = *model;

  switch (RaspberryPiModel){
    case PI_MODEL_A:
    case PI_MODEL_B:
    case PI_MODEL_AP:
    case PI_MODEL_BP:
    case PI_ALPHA:
    case PI_MODEL_CM:
    case PI_MODEL_ZERO:
    case PI_MODEL_ZERO_W:
      piGpioBase = GPIO_PERI_BASE_OLD ;
      piGpioPupOffset = GPPUD ;
      break ;

    case PI_MODEL_4B:
    case PI_MODEL_400:
    case PI_MODEL_CM4:
    case PI_MODEL_CM4S: 
      piGpioBase = GPIO_PERI_BASE_2711 ;
      piGpioPupOffset = GPPUPPDN0 ;
      break ;

    case PI_MODEL_5:
    case PI_MODEL_CM5:
    case PI_MODEL_500:
    case PI_MODEL_CM5L:
      piGpioBase = GPIO_PERI_BASE_2712 ;
      piGpioPupOffset = 0 ;
      break ;

    default:
      piGpioBase = GPIO_PERI_BASE_2835 ;
      piGpioPupOffset = GPPUD ;
      break ;
  }
}



/*
 * wpiPinToGpio:
 *	Translate a wiringPi Pin number to native GPIO pin number.
 *	Provided for external support.
 *********************************************************************************
 */

int wpiPinToGpio (int wpiPin)
{
  return pinToGpio [wpiPin & 63] ;
}


/*
 * physPinToGpio:
 *	Translate a physical Pin number to native GPIO pin number.
 *	Provided for external support.
 *********************************************************************************
 */

int physPinToGpio (int physPin)
{
  return physToGpio [physPin & 63] ;
}


/*
 * setPadDrive:
 *	Set the PAD driver value
 *********************************************************************************
 */
void setPadDrivePin (int pin, int value) {
  if (!piRP1Model() || !ToBCMPin(&pin)) {
    return;
  }

  uint32_t wrVal;
  value = value & 3; // 0-3 supported
  wrVal = (value << 4); //Drive strength 0-3
  pads[1+pin] = (pads[1+pin] & RP1_INV_PAD_DRIVE_MASK) | wrVal;
  if (wiringPiDebug) {
    printf ("setPadDrivePin: pin: %d, value: %d (%08X)\n", pin, value, pads[1+pin]) ;
  }
}


void setPadDrive (int group, int value)
{
  uint32_t wrVal, rdVal;

  if ((wiringPiMode == WPI_MODE_PINS) || (wiringPiMode == WPI_MODE_PHYS) || (wiringPiMode == WPI_MODE_GPIO))
  {
    value = value & 7; // 0-7 supported
    if (piRP1Model()) {
      if (-1==group) {
        printf ("Pad register:\n");
        for (int pin=0, maxpin=GetMaxPin(); pin<=maxpin; ++pin) {
          unsigned int drive = (pads[1+pin] & RP1_PAD_DRIVE_MASK)>>4;
          printf ("  Pin %2d: 0x%08X drive: 0x%d = %2dmA\n", pin, pads[1+pin], drive, 0==drive ? 2 : drive*4) ;
        }
      }
      if (group !=0) { // only GPIO range @RP1
        return ;
      }
      switch(value) {
        default:
                /* bcm*/                 // RP1
        case 0: /* 2mA*/ value=0; break; // 2mA
        case 1: /* 4mA*/
        case 2: /* 6mA*/ value=1; break; // 4mA
        case 3: /* 8mA*/
        case 4: /*10mA*/ value=2; break; // 8mA
        case 5: /*12mA*/
        case 6: /*14mA*/
        case 7: /*16mA*/ value=3; break; //12mA
      }
      wrVal = (value << 4); //Drive strength 0-3
      //set for all pins even when it's avaiable for each pin separately
      for (int pin=0, maxpin=GetMaxPin(); pin<=maxpin; ++pin) {
        pads[1+pin] = (pads[1+pin] & RP1_INV_PAD_DRIVE_MASK) | wrVal;
      }
      rdVal = pads[1+17]; // only pin 17 readback, for logging
    } else {
      if (-1==group) {
        printf ("Pad register: Group 0: 0x%08X, Group 1: 0x%08X, Group 2: 0x%08X\n", *(pads + 0 + 11), *(pads + 1 + 11), *(pads + 2 + 11)) ;
      }

      if ((group < 0) || (group > 2))
        return ;

      wrVal = BCM_PASSWORD | 0x18 | value; //Drive strength 0-7
      *(pads + group + 11) = wrVal ;
      rdVal = *(pads + group + 11);
    }

    if (wiringPiDebug)
    {
      printf ("setPadDrive: Group: %d, value: %d (%08X)\n", group, value, wrVal) ;
      printf ("Read : %08X\n", rdVal) ;
    }
  }
}


/*
 * getAlt:
 *	Returns the ALT bits for a given port. Only really of-use
 *	for the gpio readall command (I think)
 *********************************************************************************
 */

int getAlt (int pin)
{
  int alt;

  if (!ToBCMPin(&pin)) {
    return 0;
  }

  if (piRP1Model()) {
    alt = (gpio[2*pin+1] & RP1_FSEL_NONE_HW); //0-4  function

  /*
  BCM:
  000b = GPIO Pin 9 is an input
  001b = GPIO Pin 9 is an output
  100b = GPIO Pin 9 takes alternate function 0
  101b = GPIO Pin 9 takes alternate function 1
  110b = GPIO Pin 9 takes alternate function 2
  111b = GPIO Pin 9 takes alternate function 3
  011b = GPIO Pin 9 takes alternate function 4
  010b = GPIO Pin 9 takes alternate function 5
  RP1:
   8 = alternate function 6
   9 = alternate function 7
  10 = alternate function 8
  11 = alternate function 9
  */
    switch(alt) {
      case 0: return FSEL_ALT0;
      case 1: return FSEL_ALT1;
      case 2: return FSEL_ALT2;
      case 3: return FSEL_ALT3;
      case 4: return FSEL_ALT4;
      case RP1_FSEL_GPIO: {
          unsigned int outputmask = gpio[2*pin] & 0x3000;   //Bit13-OETOPAD + Bit12-OEFROMPERI
          return (outputmask==0x3000) ? FSEL_OUTP : FSEL_INPT;
        }
      case 6: return FSEL_ALT6;
      case 7: return FSEL_ALT7;
      case 8: return FSEL_ALT8;
      case RP1_FSEL_NONE: return FSEL_ALT9;
      default:return alt;
    }
  } else {
    int fSel    = gpioToGPFSEL [pin] ;
    int shift   = gpioToShift  [pin] ;

    alt = (*(gpio + fSel) >> shift) & 7 ;
  }
  return alt;
}


enum WPIPinAlt getPinModeAlt(int pin) {
  return (enum WPIPinAlt) getAlt(pin);
}


/*
 * pwmSetMode:
 *	Select the native "balanced" mode, or standard mark:space mode
 *********************************************************************************
 */

void pwmSetMode (int mode)
{
  if ((wiringPiMode == WPI_MODE_PINS) || (wiringPiMode == WPI_MODE_PHYS) || (wiringPiMode == WPI_MODE_GPIO))
  {
    if (piRP1Model()) {
      if(mode != PWM_MODE_MS) {
        fprintf(stderr, "pwmSetMode: Raspberry Pi 5 missing feature PWM BAL mode\n");
      }
      return;
    }
    if (mode == PWM_MODE_MS) {
      *(pwm + PWM_CONTROL) = PWM0_ENABLE | PWM1_ENABLE | PWM0_MS_MODE | PWM1_MS_MODE ;
    } else {
      *(pwm + PWM_CONTROL) = PWM0_ENABLE | PWM1_ENABLE ;
    }
    if (wiringPiDebug) {
      printf ("Enable PWM mode: %s. Current register: 0x%08X\n", mode == PWM_MODE_MS ? "mark:space (freq. stable)" : "balanced (freq. change)", *(pwm + PWM_CONTROL));
    }
  }
}


/*
 * pwmSetRange:
 *	Set the PWM range register. We set both range registers to the same
 *	value. If you want different in your own code, then write your own.
 *********************************************************************************
 */

void pwmSetRange (unsigned int range)
{
  if ((wiringPiMode == WPI_MODE_PINS) || (wiringPiMode == WPI_MODE_PHYS) || (wiringPiMode == WPI_MODE_GPIO))
  {
    /* would be possible on ms mode but not on bal, deactivated, use pwmc modify instead
    if (piGpioBase == GPIO_PERI_BASE_2711) {
      range = (OSC_FREQ_BCM2711*range)/OSC_FREQ_DEFAULT;
    }
    */
    if (!pwm) {
      fprintf(stderr, "wiringPi: pwmSetRange but no pwm memory available, ignoring\n");
      return;
    }
    int readback = 0x00;
    if (piRP1Model()) {
      pwm[RP1_PWM0_CHAN0_RANGE] = range;
      pwm[RP1_PWM0_CHAN1_RANGE] = range;
      pwm[RP1_PWM0_CHAN2_RANGE] = range;
      pwm[RP1_PWM0_CHAN3_RANGE] = range;
      readback = pwm[RP1_PWM0_CHAN0_RANGE];
     } else {
     *(pwm + PWM0_RANGE) = range ; delayMicroseconds (10) ;
     *(pwm + PWM1_RANGE) = range ; delayMicroseconds (10) ;
     readback = *(pwm + PWM0_RANGE);
    }
    if (wiringPiDebug) {
      printf ("PWM range: %u. Current register: 0x%08X\n", range, readback);
    }
  }
}


/*
 * pwmSetClock:
 *	Set/Change the PWM clock. Originally my code, but changed
 *	(for the better!) by Chris Hall, <chris@kchall.plus.com>
 *	after further study of the manual and testing with a 'scope
 *********************************************************************************
 */

void pwmSetClock (int divisor)
{
  uint32_t pwm_control ;
  if (!clk) {
      fprintf(stderr, "wiringPi: pwmSetClock but no clk memory available, ignoring\n");
      return;
  }

  if (divisor > PWMCLK_DIVI_MAX) {
    divisor = PWMCLK_DIVI_MAX;   // even on Pi5 4095 is OK
  }
  if (piRP1Model()) {
    if (divisor < 1) {
      if (wiringPiDebug) { printf("Disable PWM0 clock"); }
      clk[CLK_PWM0_CTRL] = RP1_CLK_PWM0_CTRL_DISABLE_MAGIC;   // 0 = disable on Pi5
    } else {
      divisor = (OSC_FREQ_BCM2712*divisor)/OSC_FREQ_DEFAULT;
      if (wiringPiDebug) {
         printf ("PWM clock divisor: %d\n", divisor) ;
      }
      //clk[CLK_PWM0_CTRL] = RP1_CLK_PWM0_CTRL_DISABLE_MAGIC;
      //delayMicroseconds(100);
      clk[CLK_PWM0_DIV_INT] = divisor;
      clk[CLK_PWM0_DIV_FRAC] = 0;
      clk[CLK_PWM0_SEL] = 1;
      clk[CLK_PWM0_CTRL] = RP1_CLK_PWM0_CTRL_ENABLE_MAGIC;
      }
    return;
  }
  if (piGpioBase == GPIO_PERI_BASE_2711) {
    //calculate value for OSC 54MHz -> 19.2MHz
    // Pi 4 max divisor is 1456, Pi0-3 is 4095 (0xFFF)
    divisor = (OSC_FREQ_BCM2711*divisor)/OSC_FREQ_DEFAULT;
  }
  if (divisor < 1) {
    divisor = 1;
  }
  if ((wiringPiMode == WPI_MODE_PINS) || (wiringPiMode == WPI_MODE_PHYS) || (wiringPiMode == WPI_MODE_GPIO))
  {
    if (wiringPiDebug) {
      printf ("PWM clock divisor: Old register: 0x%08X\n", *(clk + PWMCLK_DIV)) ;
    }
    pwm_control = *(pwm + PWM_CONTROL) ;		// preserve PWM_CONTROL

// We need to stop PWM prior to stopping PWM clock in MS mode otherwise BUSY
// stays high.

    *(pwm + PWM_CONTROL) = 0 ;				// Stop PWM

// Stop PWM clock before changing divisor. The delay after this does need to
// this big (95uS occasionally fails, 100uS OK), it's almost as though the BUSY
// flag is not working properly in balanced mode. Without the delay when DIV is
// adjusted the clock sometimes switches to very slow, once slow further DIV
// adjustments do nothing and it's difficult to get out of this mode.

    *(clk + PWMCLK_CNTL) = BCM_PASSWORD | 0x01 ;	// Stop PWM Clock
      delayMicroseconds (110) ;			// prevents clock going sloooow

    while ((*(clk + PWMCLK_CNTL) & 0x80) != 0)	// Wait for clock to be !BUSY
      delayMicroseconds (1) ;

    *(clk + PWMCLK_DIV)  = BCM_PASSWORD | (divisor << 12) ;

    *(clk + PWMCLK_CNTL) = BCM_PASSWORD | 0x11 ;	// Start PWM clock
    *(pwm + PWM_CONTROL) = pwm_control ;		// restore PWM_CONTROL

    if (wiringPiDebug) {
      printf ("PWM clock divisor %d. Current register: 0x%08X\n", divisor, *(clk + PWMCLK_DIV));
    }
  }
}


/*
 * gpioClockSet:
 *	Set the frequency on a GPIO clock pin
 *********************************************************************************
 */

void gpioClockSet (int pin, int freq)
{
  int divi, divr, divf ;

  FailOnModel5("gpioClockSet");
  if (!ToBCMPin(&pin)) {
    return;
  }

  divi = 19200000 / freq ;
  divr = 19200000 % freq ;
  divf = (int)((double)divr * 4096.0 / 19200000.0) ;

  if (divi > PWMCLK_DIVI_MAX) {
    divi = PWMCLK_DIVI_MAX;
  }
  *(clk + gpioToClkCon [pin]) = BCM_PASSWORD | GPIO_CLOCK_SOURCE ;		// Stop GPIO Clock
  while ((*(clk + gpioToClkCon [pin]) & 0x80) != 0)				// ... and wait
    ;

  *(clk + gpioToClkDiv [pin]) = BCM_PASSWORD | (divi << 12) | divf ;		// Set dividers
  *(clk + gpioToClkCon [pin]) = BCM_PASSWORD | 0x10 | GPIO_CLOCK_SOURCE ;	// Start Clock
}


/*
 * wiringPiFindNode:
 *      Locate our device node
 *********************************************************************************
 */

struct wiringPiNodeStruct *wiringPiFindNode (int pin)
{
  struct wiringPiNodeStruct *node = wiringPiNodes ;

  while (node != NULL)
    if ((pin >= node->pinBase) && (pin <= node->pinMax))
      return node ;
    else
      node = node->next ;

  return NULL ;
}


/*
 * wiringPiNewNode:
 *	Create a new GPIO node into the wiringPi handling system
 *********************************************************************************
 */

static void pinModeDummy         (UNU struct wiringPiNodeStruct *node, UNU int pin, UNU int mode)  { return ; }
static void pullUpDnControlDummy (UNU struct wiringPiNodeStruct *node, UNU int pin, UNU int pud)   { return ; }
static  int digitalReadDummy     (UNU struct wiringPiNodeStruct *node, UNU int UNU pin)            { return LOW ; }
static void digitalWriteDummy    (UNU struct wiringPiNodeStruct *node, UNU int pin, UNU int value) { return ; }
static void pwmWriteDummy        (UNU struct wiringPiNodeStruct *node, UNU int pin, UNU int value) { return ; }
static  int analogReadDummy      (UNU struct wiringPiNodeStruct *node, UNU int pin)                { return 0 ; }
static void analogWriteDummy     (UNU struct wiringPiNodeStruct *node, UNU int pin, UNU int value) { return ; }

struct wiringPiNodeStruct *wiringPiNewNode (int pinBase, int numPins)
{
  int    pin ;
  struct wiringPiNodeStruct *node ;

// Minimum pin base is 64

  if (pinBase < 64)
    (void)wiringPiFailure (WPI_FATAL, "wiringPiNewNode: pinBase of %d is < 64\n", pinBase) ;

// Check all pins in-case there is overlap:

  for (pin = pinBase ; pin < (pinBase + numPins) ; ++pin)
    if (wiringPiFindNode (pin) != NULL)
      (void)wiringPiFailure (WPI_FATAL, "wiringPiNewNode: Pin %d overlaps with existing definition\n", pin) ;

  node = (struct wiringPiNodeStruct *)calloc (sizeof (struct wiringPiNodeStruct), 1) ;	// calloc zeros
  if (node == NULL)
    (void)wiringPiFailure (WPI_FATAL, "wiringPiNewNode: Unable to allocate memory: %s\n", strerror (errno)) ;

  node->pinBase          = pinBase ;
  node->pinMax           = pinBase + numPins - 1 ;
  node->pinMode          = pinModeDummy ;
  node->pullUpDnControl  = pullUpDnControlDummy ;
  node->digitalRead      = digitalReadDummy ;
  node->digitalWrite     = digitalWriteDummy ;
  node->pwmWrite         = pwmWriteDummy ;
  node->analogRead       = analogReadDummy ;
  node->analogWrite      = analogWriteDummy ;
  node->next             = wiringPiNodes ;
  wiringPiNodes          = node ;

  return node ;
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

#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))


int OpenAndCheckGpioChip(int GPIONo, const char* label, const unsigned int lines) {
  char szGPIOChip[30];

  sprintf(szGPIOChip, "/dev/gpiochip%d", GPIONo);
  int Fd = open(szGPIOChip, O_RDWR);
  if (Fd < 0) {
    fprintf(stderr, "wiringPi: ERROR: %s open ret=%d\n", szGPIOChip, Fd);
    return Fd;
  } else {
    if (wiringPiDebug) {
      printf("wiringPi: Open chip %s succeded, fd=%d\n", szGPIOChip, Fd) ;
    }
    struct gpiochip_info chipinfo;
    ZeroMemory(&chipinfo, sizeof(chipinfo));
    int ret = ioctl(Fd, GPIO_GET_CHIPINFO_IOCTL, &chipinfo);
    if (0==ret) {
      if (wiringPiDebug) {
        printf("%s: name=%s, label=%s, lines=%u\n", szGPIOChip, chipinfo.name, chipinfo.label, chipinfo.lines) ;
      }
      int chipOK = 1;
      if (label[0]!='\0' && NULL==strstr(chipinfo.label, label)) {
        chipOK = 0;
      }
      if (lines>0 && chipinfo.lines!=lines) {
        chipOK = 0;
      }
      if (chipOK) {
        if (wiringPiDebug) {
          printf("%s: valid, fd=%d\n", szGPIOChip, Fd);
        }
      } else {
        if (wiringPiDebug) {
          printf("%s: invalid, search for '%s' with %u lines!\n", szGPIOChip, label, lines) ;
        }
        close(Fd);
        return -1; // invalid chip
      }
    }
  }
  return Fd;
}

int wiringPiGpioDeviceGetFd() {
  if (chipFd<0) {
    piBoard();
    if (piRP1Model()) {
      chipFd = OpenAndCheckGpioChip(0, "rp1", 54);   // /dev/gpiochip0 @ Pi5 since Kernel 6.6.47
      if (chipFd<0) {
        chipFd = OpenAndCheckGpioChip(4, "rp1", 54);  // /dev/gpiochip4 @ Pi5 with older kernel
      }
    } else {
      // not all Pis have same number of lines: Pi0, Pi1, Pi3, 54 lines, Pi4, 58 lines (CM ?), see #280, so this check is disabled
      chipFd = OpenAndCheckGpioChip(0, "bcm", 0);
    }
  }
  return chipFd;
}

void releaseLine(int pin) {

  if (wiringPiDebug)
    printf ("releaseLine: pin:%d\n", pin) ;
  lineFlags[pin] = 0;
  close(lineFds[pin]);
  lineFds[pin] = -1;
  isrDebouncePeriodUs[pin] = 0;
}


int requestLineV2(int pin, const unsigned int lineRequestFlags) {
   struct gpio_v2_line_request req;
   struct gpio_v2_line_config config;
   int ret;
   
   if (lineFds[pin]>=0) {
    if (lineRequestFlags == lineFlags[pin]) {
      //already requested
      return lineFds[pin];
    } else {
      //different request -> rerequest
      releaseLine(pin);
    }
  }

  //requested line
  if (wiringPiGpioDeviceGetFd()<0) {
    return -1;  // error
  }
  
  memset(&req, 0, sizeof(req));
  memset(&config, 0, sizeof(config));
  if (lineRequestFlags & WPI_FLAG_INPUT) {
    config.flags |= GPIO_V2_LINE_FLAG_INPUT;
  }
  if (lineRequestFlags & WPI_FLAG_OUTPUT) {
    config.flags |= GPIO_V2_LINE_FLAG_OUTPUT;
  }
  if (lineRequestFlags & WPI_FLAG_BIAS_OFF) {
    config.flags |= GPIO_V2_LINE_FLAG_BIAS_DISABLED;
  }
  if (lineRequestFlags & WPI_FLAG_BIAS_UP) {
    config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_UP;
  }
  if (lineRequestFlags & WPI_FLAG_BIAS_DOWN) {
    config.flags |= GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN;
  }
  if (wiringPiDebug) {
    printf ("requestLine flags v2: %llu\n", config.flags);
  }
  strcpy(req.consumer, "wiringpi_gpio_req");
  
  req.offsets[0] = pin;
  req.num_lines = 1;
  req.config = config;
  
  ret = ioctl(chipFd, GPIO_V2_GET_LINE_IOCTL, &req);
  
  if (ret || req.fd<0) {
    ReportDeviceError("get line handle v2", pin, "RequestLine", ret);
    return -1;  // error
  }

  lineFlags[pin] = lineRequestFlags;
  lineFds[pin] = req.fd;
  if (wiringPiDebug)
    printf ("requestLine succeeded: pin:%d, flags: 0x%u, fd :%d\n", pin, lineRequestFlags, lineFds[pin]) ;
  return lineFds[pin];
}

/*
 *********************************************************************************
 * Core Functions
 *********************************************************************************
 */

/*
 * pinModeAlt:
 *	This is an un-documented special to let you set any pin to any mode
 *********************************************************************************
 */

void pinModeAlt (int pin, int mode)
{
  setupCheck ("pinModeAlt") ;

  if (!ToBCMPin(&pin)) {
    return;
  }

  if (piRP1Model()) {
    //confusion! diffrent to to BCM!  this is taking directly the value for the register
    int modeRP1;
    switch(mode) {
      case FSEL_ALT0:
        modeRP1 = 0;
        break;
      case FSEL_ALT1:
        modeRP1 = 1;
        break;
      case FSEL_ALT2:
        modeRP1 = 2;
        break;
      case FSEL_ALT3:
        modeRP1 = 3;
        break;
      case FSEL_ALT4:
        modeRP1 = 4;
        break;
      case FSEL_ALT5:
        modeRP1 = 5;
        break;
      case FSEL_ALT6:
        modeRP1 = 6;
        break;
      case FSEL_ALT7:
        modeRP1 = 7;
        break;
      case FSEL_ALT8:
        modeRP1 = 8;
        break;
      case FSEL_OUTP:
      case FSEL_INPT:
        modeRP1 = RP1_FSEL_GPIO;
        break;
      default:
        fprintf(stderr, "pinModeAlt: invalid mode %d\n", mode);
        return;
    }
    //printf("pinModeAlt: Pi5 alt pin %d to %d\n", pin, modeRP1);
    gpio[2*pin+1] = (modeRP1 & RP1_FSEL_NONE_HW) | RP1_DEBOUNCE_DEFAULT; //0-4  function, 5-11 debounce time
  } else {
    int fSel  = gpioToGPFSEL [pin] ;
    int shift = gpioToShift  [pin] ;

    *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) | ((mode & 0x7) << shift) ;
  }
 }


/*
 * pinMode:
 *	Sets the mode of a pin to be input, output or PWM output
 *********************************************************************************
 */

//Default: rp1_set_pad(pin, 0, 1, 0, 1, 1, 1, 0);
void rp1_set_pad(int pin, int slewfast, int schmitt, int pulldown, int pullup, int drive, int inputenable, int outputdisable) {

  pads[1+pin] = (slewfast != 0) | ((schmitt != 0) << 1) | ((pulldown != 0) << 2) | ((pullup != 0) << 3) | ((drive & 0x3) << 4) | ((inputenable != 0) << 6) | ((outputdisable != 0) << 7);
}

void pinModeFlagsDevice (int pin, int mode, const unsigned int flags) {
  unsigned int lflag = flags;
  if (wiringPiDebug) {
      printf ("pinModeFlagsDevice: pin:%d mode:%d, flags: %u\n", pin, mode, flags) ;
  }
  lflag &= ~(WPI_FLAG_INPUT | WPI_FLAG_OUTPUT);
  switch(mode) {
    default:
      fprintf(stderr, "pinMode: invalid mode request (only input und output supported)\n");
      return;
    case INPUT:
      lflag |= WPI_FLAG_INPUT;
      break;
    case OUTPUT:
      lflag |= WPI_FLAG_OUTPUT;
      break;
    case PM_OFF:
      pinModeFlagsDevice(pin, INPUT, 0);
      releaseLine(pin);
      return;
  }

  requestLineV2(pin, lflag);
}

void pinModeDevice (int pin, int mode) {
  pinModeFlagsDevice(pin, mode, lineFlags[pin]);
}

void pinMode (int pin, int mode)
{
  int    fSel, shift, alt ;
  struct wiringPiNodeStruct *node = wiringPiNodes ;
  int origPin = pin ;

  if (wiringPiDebug)
    printf ("pinMode: pin:%d mode:%d\n", pin, mode) ;

  setupCheck ("pinMode") ;

  if ((pin & PI_GPIO_MASK) == 0)		// On-board pin
  {
    switch(wiringPiMode) {
      default: //WPI_MODE_GPIO_SYS
        fprintf(stderr, "pinMode: invalid mode\n");
        return;
      case WPI_MODE_PINS:
        pin = pinToGpio [pin];
        break;
      case WPI_MODE_PHYS:
        pin = physToGpio [pin];
        break;
      case WPI_MODE_GPIO_DEVICE_BCM:
        pinModeDevice(pin, mode);
        return;
      case WPI_MODE_GPIO_DEVICE_WPI:
        pinModeDevice(pinToGpio[pin], mode);
        return;
      case WPI_MODE_GPIO_DEVICE_PHYS:
        pinModeDevice(physToGpio[pin], mode);
        return;
      case WPI_MODE_GPIO:
        break;
    }

    if (wiringPiDebug)
      printf ("pinMode: bcm pin:%d mode:%d\n", pin, mode) ;

    softPwmStop  (origPin) ;
    softToneStop (origPin) ;

    fSel    = gpioToGPFSEL [pin] ;
    shift   = gpioToShift  [pin] ;

    if (INPUT==mode  || PM_OFF==mode) {
      if (piRP1Model()) {
        if (INPUT==mode) {
          pads[1+pin] = (pin<=8) ? RP1_PAD_DEFAULT_0TO8 : RP1_PAD_DEFAULT_FROM9;
          gpio[2*pin+1] = RP1_FSEL_GPIO | RP1_DEBOUNCE_DEFAULT; // GPIO
          rio[RP1_RIO_OE + RP1_CLR_OFFSET] = 1<<pin;            // Input
        } else  { //PM_OFF
          pads[1+pin] = (pin<=8) ? RP1_PAD_IC_DEFAULT_0TO8 : RP1_PAD_IC_DEFAULT_FROM9;
          gpio[2*pin+1] = RP1_IRQRESET | RP1_FSEL_NONE_HW | RP1_DEBOUNCE_DEFAULT; // default but with irq reset
        }
      } else {
        *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) ; // Sets bits to zero = input
      }
      if (PM_OFF==mode && !usingGpioMem && pwm && gpioToPwmALT[pin]>0) { //PWM pin -> reset
        pwmWrite(origPin, 0);
        int channel = gpioToPwmPort[pin];
        if (channel>=0 && channel<=3 && piRP1Model()) {
          unsigned int ctrl = pwm[RP1_PWM0_GLOBAL_CTRL];
          pwm[RP1_PWM0_GLOBAL_CTRL] = (ctrl & ~(1<<channel)) | RP1_PWM_CTRL_SETUPDATE;
          //printf("Disable PWM0[%d] (0x%08X->0x%08X)\n", channel, ctrl, pwm[RP1_PWM0_GLOBAL_CTRL]);
        }
      }
    } else if (mode == OUTPUT) {
      if (piRP1Model()) {
        pads[1+pin] = (pin<=8) ? RP1_PAD_DEFAULT_0TO8 : RP1_PAD_DEFAULT_FROM9;
        gpio[2*pin+1] = RP1_FSEL_GPIO | RP1_DEBOUNCE_DEFAULT; // GPIO
        rio[RP1_RIO_OE + RP1_SET_OFFSET] = 1<<pin;            // Output
      } else {
        *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) | (1 << shift) ;
      }
    } else if (mode == SOFT_PWM_OUTPUT) {
      softPwmCreate (origPin, 0, 100) ;
    } else if (mode == SOFT_TONE_OUTPUT) {
      softToneCreate (origPin) ;
    } else if (mode == PWM_TONE_OUTPUT)
    {
      pinMode (origPin, PWM_OUTPUT) ;	// Call myself to enable PWM mode
      pwmSetMode (PWM_MODE_MS) ;
    }
    else if (PWM_OUTPUT==mode || PWM_MS_OUTPUT==mode || PWM_BAL_OUTPUT==mode) {

      usingGpioMemCheck("pinMode PWM") ;  // exit on error!
      alt = gpioToPwmALT[pin];
      if (0==alt) {	// Not a hardware capable PWM pin
	      return;
      }
      int channel = gpioToPwmPort[pin];
      if (piRP1Model()) {
        if (channel>=0 && channel<=3) {
          // enable channel pwm m:s mode
          pwm[RP1_PWM0_CHAN_START+RP1_PWM0_CHAN_OFFSET*channel+RP1_PWM0_CHAN_CTRL]  = (RP1_PWM_TRAIL_EDGE_MS | RP1_PWM_FIFO_POP_MASK);
          // enable pwm global
          unsigned int ctrl = pwm[RP1_PWM0_GLOBAL_CTRL];
          pwm[RP1_PWM0_GLOBAL_CTRL] = ctrl | (1<<channel) | RP1_PWM_CTRL_SETUPDATE;
          //printf("Enable PWM0[%d] (0x%08X->0x%08X)\n", channel, ctrl, pwm[RP1_PWM0_GLOBAL_CTRL]);
          //change GPIO mode
          pads[1+pin] = RP1_PAD_DEFAULT_FROM9;  // enable output
          pinModeAlt(origPin, alt); //switch to PWM mode
        }
      } else {
        // Set pin to PWM mode
        *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) | (alt << shift) ;
        delayMicroseconds (110) ;		// See comments in pwmSetClockWPi

        if (PWM_OUTPUT==mode || PWM_BAL_OUTPUT==mode) {
          pwmSetMode(PWM_MODE_BAL);	// Pi default mode
        } else {
          pwmSetMode(PWM_MODE_MS);
        }
      }
      if (PWM_OUTPUT==mode) {  // predefine
        pwmSetRange (1024) ;		// Default range of 1024
        pwmSetClock (32) ;		// 19.2 / 32 = 600KHz - Also starts the PWM
      }
    }
    else if (mode == GPIO_CLOCK)
    {
      RETURN_ON_MODEL5
      if ((alt = gpioToGpClkALT0 [pin]) == 0)	// Not a GPIO_CLOCK pin
	      return ;

      usingGpioMemCheck ("pinMode CLOCK") ;

// Set pin to GPIO_CLOCK mode and set the clock frequency to 100KHz

      *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) | (alt << shift) ;
      delayMicroseconds (110) ;
      gpioClockSet      (pin, 100000) ;
    }
  }
  else
  {
    if ((node = wiringPiFindNode (pin)) != NULL)
      node->pinMode (node, pin, mode) ;
    return ;
  }
}


/*
 * pullUpDownCtrl:
 *	Control the internal pull-up/down resistors on a GPIO pin.
 *********************************************************************************
 */
void pullUpDnControlDevice (int pin, int pud) {
  unsigned int flag = lineFlags[pin];
  unsigned int biasflags = WPI_FLAG_BIAS_OFF | WPI_FLAG_BIAS_UP | WPI_FLAG_BIAS_DOWN;

  flag &= ~biasflags;
  switch (pud){
    case PUD_OFF:  flag |= WPI_FLAG_BIAS_OFF;   break;
    case PUD_UP:   flag |= WPI_FLAG_BIAS_UP;   break;
    case PUD_DOWN: flag |= WPI_FLAG_BIAS_DOWN; break;
    default: return ; /* An illegal value */
  }

  // reset input/output
  if (lineFlags[pin] & WPI_FLAG_OUTPUT) {
    pinModeFlagsDevice (pin, OUTPUT, flag);
  } else if(lineFlags[pin] & WPI_FLAG_INPUT) {
    pinModeFlagsDevice (pin, INPUT, flag);
  } else {
    lineFlags[pin] = flag; // only store for later
  }
}


void pullUpDnControl (int pin, int pud)
{
  struct wiringPiNodeStruct *node = wiringPiNodes ;

  setupCheck ("pullUpDnControl") ;

  if ((pin & PI_GPIO_MASK) == 0)		// On-Board Pin
  {
    switch(wiringPiMode) {
      default: //WPI_MODE_GPIO_SYS
        fprintf(stderr, "pinMode: invalid mode\n");
        return;
      case WPI_MODE_PINS:
        pin = pinToGpio [pin];
        break;
      case WPI_MODE_PHYS:
        pin = physToGpio [pin];
        break;
      case WPI_MODE_GPIO_DEVICE_BCM:
        return pullUpDnControlDevice(pin, pud);
      case WPI_MODE_GPIO_DEVICE_WPI:
        return pullUpDnControlDevice(pinToGpio[pin], pud);
      case WPI_MODE_GPIO_DEVICE_PHYS:
        return pullUpDnControlDevice(physToGpio[pin], pud);
      case WPI_MODE_GPIO:
        break;
    }

    if (piRP1Model()) {
      unsigned int pullbits = pads[1+pin] & RP1_INV_PUD_MASK; // remove bits
      switch (pud){
        case PUD_OFF:  pads[1+pin] = pullbits;                break;
        case PUD_UP:   pads[1+pin] = pullbits | RP1_PUD_UP;   break;
        case PUD_DOWN: pads[1+pin] = pullbits | RP1_PUD_DOWN; break;
        default: return ; /* An illegal value */
      }
    } else {
      if (piGpioPupOffset == GPPUPPDN0)
      {
        // Pi 4B pull up/down method
        int pullreg = GPPUPPDN0 + (pin>>4);
        int pullshift = (pin & 0xf) << 1;
        unsigned int pullbits;
        unsigned int pull;

        switch (pud) {
         case PUD_OFF:  pull = 0; break;
         case PUD_UP:   pull = 1; break;
         case PUD_DOWN: pull = 2; break;
         default: return ; /* An illegal value */
        }

        pullbits = *(gpio + pullreg);
        pullbits &= ~(3 << pullshift);
        pullbits |= (pull << pullshift);
        *(gpio + pullreg) = pullbits;
      }
      else
      {
        // legacy pull up/down method
        *(gpio + GPPUD)              = pud & 3 ;		delayMicroseconds (5) ;
        *(gpio + gpioToPUDCLK [pin]) = 1 << (pin & 31) ;	delayMicroseconds (5) ;

        *(gpio + GPPUD)              = 0 ;			delayMicroseconds (5) ;
        *(gpio + gpioToPUDCLK [pin]) = 0 ;			delayMicroseconds (5) ;
      }
    }
  }
  else						// Extension module
  {
    if ((node = wiringPiFindNode (pin)) != NULL)
      node->pullUpDnControl (node, pin, pud) ;
    return ;
  }
}

/*
 helper functions for gpio_v2_line_values bits 
*/
static inline void gpiotools_set_bit(__u64 *b, int n)
{
	*b |= _BITULL(n);
}

static inline void gpiotools_clear_bit(__u64 *b, int n)
{
	*b &= ~_BITULL(n);
}

static inline void gpiotools_assign_bit(__u64 *b, int n, bool value)
{
	if (value)
		gpiotools_set_bit(b, n);
	else
		gpiotools_clear_bit(b, n);
}

static inline int gpiotools_test_bit(__u64 b, int n)
{
	return !!(b & _BITULL(n));
}

//*********************************************


/*
 * digitalRead:
 *	Read the value of a given Pin, returning HIGH or LOW
 *********************************************************************************
 */

int digitalReadDeviceV2(int pin) {   // INPUT and OUTPUT should work
  struct gpio_v2_line_values lv;
  int ret;
  
  if (lineFds[pin]<0) {
    // line not requested - auto request on first read as input
     pinModeDevice(pin, INPUT);
  }
  lv.mask = 0;
  lv.bits = 0;
  if (lineFds[pin]>=0) {
    gpiotools_set_bit(&lv.mask, 0); 
    ret = ioctl(lineFds[pin], GPIO_V2_LINE_GET_VALUES_IOCTL, &lv);
    if (ret) {
      ReportDeviceError("get line values", pin, "digitalRead", ret);
      return LOW;  // error
    }
    return gpiotools_test_bit(lv.bits, 0);
  }
  return LOW;  // error , need to request line before
}


int digitalRead (int pin)
{
  struct wiringPiNodeStruct *node = wiringPiNodes ;

  if ((pin & PI_GPIO_MASK) == 0)		// On-Board Pin
  {
    switch(wiringPiMode) {
      default: //WPI_MODE_GPIO_SYS
        fprintf(stderr, "digitalRead: invalid mode\n");
        return LOW;
      case WPI_MODE_PINS:
        pin = pinToGpio [pin];
        break;
      case WPI_MODE_PHYS:
        pin = physToGpio [pin];
        break;
      case WPI_MODE_GPIO_DEVICE_BCM:
        return digitalReadDeviceV2(pin);
      case WPI_MODE_GPIO_DEVICE_WPI:
        return digitalReadDeviceV2(pinToGpio[pin]);
      case WPI_MODE_GPIO_DEVICE_PHYS:
        return digitalReadDeviceV2(physToGpio[pin]);
      case WPI_MODE_GPIO:
        break;
    }

    if (ISRP1MODEL) {
      switch(gpio[2*pin] & RP1_STATUS_LEVEL_MASK) {
        default: // 11 or 00 not allowed, give LOW!
        case RP1_STATUS_LEVEL_LOW:  return LOW ;
        case RP1_STATUS_LEVEL_HIGH: return HIGH ;
      }
    } else {
      if ((*(gpio + gpioToGPLEV [pin]) & (1 << (pin & 31))) != 0)
        return HIGH ;
      else
        return LOW ;
    }
  }
  else
  {
    if ((node = wiringPiFindNode (pin)) == NULL)
      return LOW ;
    return node->digitalRead (node, pin) ;
  }
}


/*
 * digitalWrite:
 *	Set an output bit
 *********************************************************************************
 */

void digitalWriteDeviceV2(int pin, int value) {
  int ret;
  struct gpio_v2_line_values values;
  
  if (wiringPiDebug)
    printf ("digitalWriteDeviceV2: ioctl pin:%d value: %d\n", pin, value) ;

  if (lineFds[pin]<0) {
    // line not requested - auto request on first write as output
    pinModeDevice(pin, OUTPUT);
  }
  
  if (lineFds[pin]>=0 && (lineFlags[pin] & GPIO_V2_LINE_FLAG_OUTPUT)>0) {
    values.mask = 0;
    values.bits = 0;    
    gpiotools_set_bit(&values.mask, 0);
    gpiotools_assign_bit(&values.bits, 0, !!value);

    ret = ioctl(lineFds[pin], GPIO_V2_LINE_SET_VALUES_IOCTL, &values);
    if (ret == -1) {
        ReportDeviceError("digitalWriteDeviceV2", pin, "GPIO_V2_LINE_SET_VALUES_IOCTL", ret);
        return; // error
    }
  } else {
    fprintf(stderr, "digitalWriteDeviceV2: no output (%d)\n", lineFlags[pin]);
  }
  return; // error
}


void digitalWrite (int pin, int value)
{
  struct wiringPiNodeStruct *node = wiringPiNodes ;

  if ((pin & PI_GPIO_MASK) == 0)		// On-Board Pin
  {
    switch(wiringPiMode) {
      default: //WPI_MODE_GPIO_SYS
        fprintf(stderr, "digitalWrite: invalid mode\n");
        return;
      case WPI_MODE_PINS:
        pin = pinToGpio [pin];
        break;
      case WPI_MODE_PHYS:
        pin = physToGpio [pin];
        break;
      case WPI_MODE_GPIO_DEVICE_BCM:
        digitalWriteDeviceV2(pin, value);
        return;
      case WPI_MODE_GPIO_DEVICE_WPI:
        digitalWriteDeviceV2(pinToGpio[pin], value);
        return;
      case WPI_MODE_GPIO_DEVICE_PHYS:
        digitalWriteDeviceV2(physToGpio[pin], value);
        return;
      case WPI_MODE_GPIO:
        break;
    }

    if (ISRP1MODEL) {
      if (value == LOW) {
        //printf("Set pin %d >>0x%08x<< to low\n", pin, 1<<pin);
        rio[RP1_RIO_OUT + RP1_CLR_OFFSET] = 1<<pin;
      } else {
        //printf("Set pin %d >>0x%08x<< to high\n", pin, 1<<pin);
        rio[RP1_RIO_OUT + RP1_SET_OFFSET] = 1<<pin;
      }
    } else {
      if (value == LOW)
        *(gpio + gpioToGPCLR [pin]) = 1 << (pin & 31) ;
      else
        *(gpio + gpioToGPSET [pin]) = 1 << (pin & 31) ;
    }
  }
  else
  {
    if ((node = wiringPiFindNode (pin)) != NULL)
      node->digitalWrite (node, pin, value) ;
  }
}


void pwmWrite (int pin, int value)
{
  struct wiringPiNodeStruct *node = wiringPiNodes ;

  setupCheck ("pwmWrite") ;

  if ((pin & PI_GPIO_MASK) == 0)		// On-Board Pin
  {
    if (!ToBCMPin(&pin)) {
      return;
    }

    /* would be possible on ms mode but not on bal, deactivated, use pwmc modify instead
    if (piGpioBase == GPIO_PERI_BASE_2711) {
      value = (OSC_FREQ_BCM2711*value)/OSC_FREQ_DEFAULT;
    }
    */
    usingGpioMemCheck ("pwmWrite") ;
    int channel = gpioToPwmPort[pin];
    int readback = 0x00;
    if (piRP1Model()) {
      if (channel>=0 && channel<=3) {
        unsigned int addr = RP1_PWM0_CHAN_START+RP1_PWM0_CHAN_OFFSET*channel+RP1_PWM0_CHAN_DUTY;
        pwm[addr] = value;
        readback = pwm[addr];
      } else {
        fprintf(stderr, "pwmWrite: invalid channel at GPIO pin %d \n", pin);
      }
    } else {
      *(pwm + channel) = value ;
      readback = *(pwm + channel);
    }
    if (wiringPiDebug) {
      printf ("PWM value(duty): %u. Current register: 0x%08X\n", value, readback);
    }
  }
  else
  {
    if ((node = wiringPiFindNode (pin)) != NULL)
      node->pwmWrite (node, pin, value) ;
  }
}


/*
 * analogRead:
 *	Read the analog value of a given Pin.
 *	There is no on-board Pi analog hardware,
 *	so this needs to go to a new node.
 *********************************************************************************
 */

int analogRead (int pin)
{
  struct wiringPiNodeStruct *node = wiringPiNodes ;

  if ((node = wiringPiFindNode (pin)) == NULL)
    return 0 ;
  else
    return node->analogRead (node, pin) ;
}


/*
 * analogWrite:
 *	Write the analog value to the given Pin.
 *	There is no on-board Pi analog hardware,
 *	so this needs to go to a new node.
 *********************************************************************************
 */

void analogWrite (int pin, int value)
{
  struct wiringPiNodeStruct *node = wiringPiNodes ;

  if ((node = wiringPiFindNode (pin)) == NULL)
    return ;

  node->analogWrite (node, pin, value) ;
}


/*
 * pwmToneWrite:
 *	Pi Specific.
 *      Output the given frequency on the Pi's PWM pin
 *********************************************************************************
 */

void pwmToneWrite (int pin, int freq)
{
  setupCheck ("pwmToneWrite") ;

  if (freq == 0)
    pwmWrite (pin, 0) ;             // Off
  else
  {
    int range = 600000 / freq ;
    pwmSetRange (range) ;
    pwmWrite    (pin, freq / 2) ;
  }
}



/*
 * digitalWriteByte:
 * digitalReadByte:
 *	Pi Specific
 *	Write an 8-bit byte to the first 8 GPIO pins - try to do it as
 *	fast as possible.
 *	However it still needs 2 operations to set the bits, so any external
 *	hardware must not rely on seeing a change as there will be a change
 *	to set the outputs bits to zero, then another change to set the 1's
 *	Reading is just bit fiddling.
 *	These are wiringPi pin numbers 0..7, or BCM_GPIO pin numbers
 *	17, 18, 22, 23, 24, 24, 4 on a Pi v1 rev 0-3
 *	17, 18, 27, 23, 24, 24, 4 on a Pi v1 rev 3 onwards or B+, 2, 3, zero
 *********************************************************************************
 */

void digitalWriteByte (const int value)
{
  uint32_t pinSet = 0 ;
  uint32_t pinClr = 0 ;
  int mask = 1 ;
  int pin ;

  FailOnModel5("digitalWriteByte");

  if (wiringPiMode == WPI_MODE_GPIO_SYS)
  {
    return ;
  }
  else
  {
    for (pin = 0 ; pin < 8 ; ++pin)
    {
      if ((value & mask) == 0)
	pinClr |= (1 << pinToGpio [pin]) ;
      else
	pinSet |= (1 << pinToGpio [pin]) ;

      mask <<= 1 ;
    }

    *(gpio + gpioToGPCLR [0]) = pinClr ;
    *(gpio + gpioToGPSET [0]) = pinSet ;
  }
}

unsigned int digitalReadByte (void)
{
  int pin, x ;
  uint32_t raw ;
  uint32_t data = 0 ;

  FailOnModel5("digitalReadByte");

  if (wiringPiMode == WPI_MODE_GPIO_SYS)
  {
    return 0;
  }
  else
  {
    raw = *(gpio + gpioToGPLEV [0]) ; // First bank for these pins
    for (pin = 0 ; pin < 8 ; ++pin)
    {
      x = pinToGpio [pin] ;
      data = (data << 1) | (((raw & (1 << x)) == 0) ? 0 : 1) ;
    }
  }
  return data ;
}


/*
 * digitalWriteByte2:
 * digitalReadByte2:
 *	Pi Specific
 *	Write an 8-bit byte to the second set of 8 GPIO pins. This is marginally
 *	faster than the first lot as these are consecutive BCM_GPIO pin numbers.
 *	However they overlap with the original read/write bytes.
 *********************************************************************************
 */

void digitalWriteByte2 (const int value)
{
  FailOnModel5("digitalWriteByte2");

  if (wiringPiMode == WPI_MODE_GPIO_SYS)
  {
  }
  else
  {
    *(gpio + gpioToGPCLR [0]) = (~value & 0xFF) << 20 ; // 0x0FF00000; ILJ > CHANGE: Old causes glitch
    *(gpio + gpioToGPSET [0]) = ( value & 0xFF) << 20 ;
  }
}

unsigned int digitalReadByte2 (void)
{
  uint32_t data = 0 ;

  FailOnModel5("digitalReadByte2");

  if (wiringPiMode == WPI_MODE_GPIO_SYS)
  {
  }
  else
    data = ((*(gpio + gpioToGPLEV [0])) >> 20) & 0xFF ; // First bank for these pins

  return data ;
}



/*
 * waitForInterrupt2:
 *	Wait for Interrupt on a GPIO pin and use v2 of the character device API, need Kernel 5.1
 *  Returns struct WPIWfiStatus
 *********************************************************************************
 */

struct WPIWfiStatus waitForInterrupt2(int pin, int edgeMode, int ms, unsigned long debounce_period_us)    // ms < 0 wait infinite, = 0 return immediately, > 0 wait timeout
{
  int ret;
  int fd, attr, status, readret;
  struct pollfd polls ;
  struct gpio_v2_line_event evdata;
  struct gpio_v2_line_config config;
  struct gpio_v2_line_request req;
  const char* strmode = "";
  struct WPIWfiStatus wfiStatus;
  
  memset(&wfiStatus, 0, sizeof(wfiStatus));
  /* open gpio */
  if (wiringPiGpioDeviceGetFd()<0 || !ToBCMPin(&pin)) {
    wfiStatus.statusOK = -1;
    return wfiStatus;
  }
  
  memset(&req, 0, sizeof(req));
  memset(&config, 0, sizeof(config));
  
  /* setup config */
  config.flags = GPIO_V2_LINE_FLAG_INPUT;
  
  switch(edgeMode) {
    default:
    case INT_EDGE_SETUP:
      if (wiringPiDebug) {
        printf ("waitForInterrupt2: edgeMode INT_EDGE_SETUP - exiting\n") ;
      }
      wfiStatus.statusOK = -1;
      return wfiStatus;
    case INT_EDGE_FALLING:
      config.flags |= GPIO_V2_LINE_FLAG_EDGE_FALLING;
      strmode = "falling";
      break;
    case INT_EDGE_RISING:
      config.flags |= GPIO_V2_LINE_FLAG_EDGE_RISING;
      strmode = "rising";
      break;
    case INT_EDGE_BOTH:
      config.flags |= (GPIO_V2_LINE_FLAG_EDGE_FALLING | GPIO_V2_LINE_FLAG_EDGE_RISING);
      strmode = "both";
      break;
  }
  strcpy(req.consumer, "wiringpi_gpio_irq");
  
  if (debounce_period_us) {
	  attr = config.num_attrs;
	  config.num_attrs++;
    gpiotools_set_bit(&config.attrs[attr].mask, 0);
	  config.attrs[attr].attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
	  config.attrs[attr].attr.debounce_period_us = debounce_period_us;
  }
  
  req.num_lines = 1;
  req.offsets[0] = pin;
  req.event_buffer_size = 32;
  req.config = config;

  status = ioctl(chipFd, GPIO_V2_GET_LINE_IOCTL, &req);
  if (status == -1) {
    ReportDeviceError("GPIO_V2_GET_LINE_IOCTL", pin , strmode, status);
    wfiStatus.statusOK = -1;
    return wfiStatus;
  }

  if (wiringPiDebug) {
    printf ("waitForInterrupt2: GPIO get line %d , mode %s succeded, fd=%d\n", pin, strmode, req.fd) ;
  }
 
  fd = req.fd;
  isrFds [pin] = fd;
  isrDebouncePeriodUs[pin] = debounce_period_us; 
  
/* set event fd nonbloack read */ 
  /*
  int flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  status = fcntl(fd, F_SETFL, flags);
  if (status) {
    fprintf(stderr, "wiringPi: ERROR: fcntl set nonblock return=%d\n", status);
    return -1;
  }
*/

  // Setup poll structure
  polls.fd      = fd;
  polls.events  = POLLIN | POLLPRI;
  polls.revents = 0;

  ret = poll(&polls, 1, ms);
  if (ret < 0) {
    if (wiringPiDebug) { 
      fprintf(stderr, "waitForInterrupt2: ERROR: poll returned=%d\n", ret);
    }
    wfiStatus.statusOK = -1;
  } else if (ret == 0) { 
    if (wiringPiDebug) {
      fprintf(stderr, "waitForInterrupt2: timeout: poll returned zero\n");
    }
    wfiStatus.statusOK = 0; // timeout
  }
  else {
    if (wiringPiDebug) {
      printf ("waitForInterrupt2: IRQ line %d received %d, fd=%d\n", pin, ret, isrFds[pin]);
    }
    if (polls.revents & POLLIN) {  
      /* read event data */
      readret = read(isrFds [pin], &evdata, sizeof(evdata));
      if (readret == sizeof(evdata)) {
        if (wiringPiDebug) {
          printf ("waitForInterrupt2: IRQ at PIN: %d, timestamp: %lld\n", evdata.offset, evdata.timestamp_ns) ;
        }
        switch (evdata.id) {
          case GPIO_V2_LINE_EVENT_RISING_EDGE:
            wfiStatus.edge = INT_EDGE_RISING;
            if (wiringPiDebug) printf("waitForInterrupV2: rising edge\n");
          break;
          case GPIO_V2_LINE_EVENT_FALLING_EDGE:
            wfiStatus.edge = INT_EDGE_FALLING;
            if (wiringPiDebug) printf("waitForInterrupt2: falling edge\n");
			    break;
		      default:
            wfiStatus.edge = INT_EDGE_SETUP;        // edge = 0
            if (wiringPiDebug) printf("waitForInterrupt2: unknown event\n");
            break;
		    }
        wfiStatus.timeStamp_us = evdata.timestamp_ns / 1000LL;    // nanoseconds u64 to microseconds
        wfiStatus.pinBCM = evdata.offset;
        wfiStatus.statusOK = 1;
      }
      else {
        wfiStatus.statusOK = -1;
      }
    }
    else {
        wfiStatus.statusOK = -1;
    }
  }

  if (isrFds[pin] > 0) {
    close(isrFds [pin]);        // release line
    isrFds [pin] = -1;
    isrDebouncePeriodUs[pin] = 0;
  }

  return wfiStatus;
}

int waitForInterrupt (int pin, int ms) {
  struct WPIWfiStatus status;

  int edgeMode = isrEdgeMode[pin];
  if (edgeMode==0) {
    fprintf(stderr, "waitForInterrupt: ERROR: edge mode missing, legacy function, please use waitForInterrupt2!\n");
    return -1;
  }
  status = waitForInterrupt2(pin, edgeMode, ms, 0);

  return  status.statusOK;
}

/*
 * wiringPiISRStop:
 * stop interruptHandler thread and
 * wait untill stopped.
 * close isrFds[pin], reset isrFds[pin], isrFunction[pin] and isrDebouncePeriodUs[pin]
 *
 *********************************************************************************
 */

int wiringPiISRStop(int pin) {

  if (wiringPiMode == WPI_MODE_UNINITIALISED) {
    return wiringPiFailure(WPI_FATAL, "wiringPiISRStop: wiringPi has not been initialised. Unable to continue.\n");
  }
  if (!ToBCMPin(&pin)) {
    fprintf(stderr, "wiringPiISRStop: wrong pin %d (mode: %d) number!\n", pin, wiringPiMode);
    return EINVAL;
  }
  if (wiringPiDebug) {
    printf("wiringPiISRStop: pin %d\n", pin) ;
  }

  if (isrFds[pin] > 0) {
    void *res;

    if (wiringPiDebug)
      printf("wiringPiISRStop: close thread 0x%lX\n", (unsigned long)isrThreads[pin]);
    
    if (isrThreads[pin] != 0) {
      if (pthread_cancel(isrThreads[pin]) == 0) {
        pthread_join(isrThreads[pin], &res); 
        if (res == PTHREAD_CANCELED) {
            if (wiringPiDebug)
               printf("wiringPiISRStop: thread was canceled\n");
        }
        else {
            if (wiringPiDebug)
               printf("wiringPiISRStop: thread was not canceled\n");
        }
      } else {
        if (wiringPiDebug)
          printf("wiringPiISRStop: could not cancel thread\n");
      }
    }
    close(isrFds [pin]);
  } else {
      if (wiringPiDebug)
        printf("wiringPiISRStop: Warning stop isr, but its not active\n");
  }
  isrFds [pin] = -1;
  isrFunctions[pin] = NULL;
  isrFunctionsV2[pin] = NULL;
  isrUserdata[pin] = NULL;;
  isrDebouncePeriodUs[pin] = 0;
  
  /* -not closing so far - other isr may be using it - only close if no other is using - will code later
  if (chipFd>0) {
    close(chipFd);
  }
  chipFd = -1;
  */
  if (wiringPiDebug) {
    printf("wiringPiISRStop: wiringPiISRStop finished\n");
  }
  return 0;
}

int waitForInterruptClose(int pin) {
  return wiringPiISRStop(pin);
}

/*
 * interruptHandlerV2:
 *	This is a thread and gets started to wait for the interrupt we're
 *	hoping to catch. It will call the user-function when the interrupt
 *	fires.
 *********************************************************************************
 */

void *interruptHandlerV2(void *arg)
{
  const char* strmode = ""; 
  int pin, EdgeMode, ret, fd, attr, i;
  unsigned int readret;
  unsigned long debounce_period_us;
  struct pollfd polls ;  
  struct gpio_v2_line_config config;
  struct gpio_v2_line_request req;
  struct gpio_v2_line_event evdat[64];  
  struct WPIWfiStatus wfiStatus;
  struct timespec tspec = {0, 5e5};  /* 0.5 ms timeout {0, 1e6} */
  
  pin = *(int *)arg;

  if (wiringPiGpioDeviceGetFd()<0) {
    return NULL;
  }
    
  EdgeMode = isrEdgeMode[pin];
  debounce_period_us = isrDebouncePeriodUs[pin];
 
  if (wiringPiDebug) {
    printf ("interruptHandlerV2: GPIO line %d, edge mode %d, debounce_period_us %lu \n", pin, EdgeMode, debounce_period_us) ;
  } 
  
  memset(&req, 0, sizeof(req));
  memset(&config, 0, sizeof(config));
  
  /* setup config */
  config.flags = GPIO_V2_LINE_FLAG_INPUT;
  switch(EdgeMode) {
    default:
    case INT_EDGE_SETUP:
      if (wiringPiDebug) {
        printf ("interruptHandlerV2: waitForInterruptMode edge mode INT_EDGE_SETUP - exiting\n") ;
      }
      return NULL;
    case INT_EDGE_FALLING:
      config.flags |= GPIO_V2_LINE_FLAG_EDGE_FALLING;
      strmode = "falling";
      break;
    case INT_EDGE_RISING:
      config.flags |= GPIO_V2_LINE_FLAG_EDGE_RISING;
      strmode = "rising";
      break;
    case INT_EDGE_BOTH:
      config.flags |= (GPIO_V2_LINE_FLAG_EDGE_FALLING | GPIO_V2_LINE_FLAG_EDGE_RISING);
      strmode = "both";
      break;
  }
  strcpy(req.consumer, "wiringpi_gpio_irq");
  
  if (debounce_period_us) {
		attr = config.num_attrs;
		config.num_attrs++;
        gpiotools_set_bit(&config.attrs[attr].mask, 0);
		config.attrs[attr].attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
		config.attrs[attr].attr.debounce_period_us = debounce_period_us;
  }
  
  req.num_lines = 1;
  req.event_buffer_size = 45;
  req.offsets[0] = pin;
  req.config = config;

  ret = ioctl(chipFd, GPIO_V2_GET_LINE_IOCTL, &req);
  if (ret == -1) {
    ReportDeviceError("interruptHandlerV2: get line event", pin , strmode, ret);
    return NULL;
  }

  if (wiringPiDebug) 
    printf ("interruptHandlerV2: GPIO get line %d , mode %s succeded, fd=%d\n", pin, strmode, req.fd) ;

  /* set event fd  */
  fd = req.fd;
  isrFds [pin] = fd;
  
  (void)piHiPri (55) ;	// Only effective if we run as root

  for (;;) {    // check if event data is available, check if interruptHandlerV2 thread must be canceled

  // Setup poll structure
    polls.fd      = fd;
    polls.events  = POLLIN | POLLPRI;
    polls.revents = 0;
    
    // get event data, this is also a cancelation point, when pthread_cancel is called
    ret = ppoll(&polls, 1, &tspec, NULL);     // returns -1 on error, 0 on timeout, >0 number of elements
  
    if (ret < 0) {      // we do not reach this point if canceled, ppoll does not return, is Cancellation Point
        if (wiringPiDebug)  
            printf("interruptHandlerV2: ERROR: poll returned=%d\n", ret);
        pthread_exit(NULL); 
        return NULL;        // never landing here
    } else if (ret == 0) { 
//        if (wiringPiDebug)  
//            printf("interruptHandlerV2: timeout: poll returned=%d\n", ret);
        continue;
    }
    else {
        if (wiringPiDebug)
            printf ("interruptHandlerV2: IRQ line %d received %d events, fd=%d\n", pin, ret, isrFds[pin]) ;
        if (polls.revents & POLLIN) {  
            /* read event data */
            readret = read(fd, &evdat, sizeof(evdat));
            if (readret >= sizeof(evdat[0])) {
                if (wiringPiDebug)
                    printf ("interruptHandlerV2: IRQ at PIN: %d, events: %u\n", evdat[0].offset, readret/(unsigned int)sizeof(evdat[0])) ;

                ret = readret/sizeof(evdat[0]);     // number of events read from fd
                for (i = 0; i < ret; ++i) {
                    if (isrFunctionsV2[pin]) {
                        if (wiringPiDebug) 
                            printf( "interruptHandlerV2: GPIO EVENT at %llu on line %u (%u|%u) \n", evdat[i].timestamp_ns, evdat[i].offset, evdat[i].line_seqno, evdat[i].seqno);
                        wfiStatus.statusOK = 1;
                        wfiStatus.pinBCM = pin;
                        switch (evdat[i].id) {
                            case GPIO_V2_LINE_EVENT_RISING_EDGE:
                                wfiStatus.edge = INT_EDGE_RISING;
                                if (wiringPiDebug)
                                    printf("waitForInterrupt2: rising edge\n");
                                break;
                            case GPIO_V2_LINE_EVENT_FALLING_EDGE:
                                wfiStatus.edge = INT_EDGE_FALLING;
                                if (wiringPiDebug)
                                    printf("waitForInterrupt2: falling edge\n");
                                break;
                            default:
                                wfiStatus.edge = INT_EDGE_SETUP;        // edge = 0
                                if (wiringPiDebug) 
                                    printf("waitForInterrupt2: unknown event\n");
                                break;
                        }        
                        wfiStatus.timeStamp_us = evdat[i].timestamp_ns/1000LL;
                        if (wiringPiDebug) {
                          printf( "interruptHandlerV2: call isr function\n");
                        }
                        isrFunctionsV2[pin](wfiStatus, isrUserdata[pin]);
                        if (wiringPiDebug) {
                          printf( "interruptHandlerV2: return from isr function\n");
                        }
                    }
                    if (isrFunctions[pin]) {
                      if (wiringPiDebug) {
                        printf( "interruptHandlerV2: call isr function classic\n");
                      }
                      isrFunctions[pin]();
                      if (wiringPiDebug) {
                        printf( "interruptHandlerV2: return from isr function classic\n");
                      }
                    }
                }
            }
            else {  // if thread canceled we do not reach this point, read(...) does not return, is Cancellation Point
                if (wiringPiDebug)
                    printf ("interruptHandlerV2: reading events from fd received signal, exit thread\n");
                pthread_exit(NULL);  
                return NULL; // never landing here
            }
        }
    }
  }
}


/*
 * wiringPiISR:
 *	Pi Specific.
 *	Take the details and create an interrupt handler that will do a call-
 *	back to the user supplied function.
 *  debounce_period_us in microseconds
 *********************************************************************************
 */

int wiringPiISRInternal(int pin, int edgeMode, void (*function)(struct WPIWfiStatus wfiStatus, void* userdata), void (*functionClassic)(void), unsigned long debounce_period_us, void* userdata)
{
  if (wiringPiMode == WPI_MODE_UNINITIALISED) {
    return wiringPiFailure(WPI_FATAL, "wiringPiISR: wiringPi has not been initialised. Unable to continue.\n");
  }
  if (!ToBCMPin(&pin)) {
    fprintf(stderr, "wiringPiISRStop: wrong pin %d (mode: %d) number!\n", pin, wiringPiMode);
    return EINVAL;
  }
  if (wiringPiDebug) {
    printf("wiringPi: wiringPiISR pin %d, edgeMode %d\n", pin, edgeMode);
  }
  if (isrFunctions[pin] || isrFunctionsV2[pin]) {
    fprintf(stderr, "wiringPi: ISR function already active, ignoring \n");
  }

  isrFunctionsV2[pin] = function;
  isrUserdata[pin] = userdata;
  isrFunctions[pin] = functionClassic;
  isrEdgeMode[pin] = edgeMode;
  isrDebouncePeriodUs[pin] = debounce_period_us;
  
  if (wiringPiDebug) {
    printf("wiringPi: mutex in\n");
  }
  pthread_mutex_lock (&pinMutex) ;
    pinPass = pin ;
    if (wiringPiDebug) {
      printf("wiringPi: pthread_create before 0x%lX\n", (unsigned long)isrThreads[pin]);
    }
    if (pthread_create (&isrThreads[pin], NULL, interruptHandlerV2, &pin)==0) {
      if (wiringPiDebug) {
        printf("wiringPi: pthread_create successed, 0x%lX\n", (unsigned long)isrThreads[pin]);
      }
/*      while (pinPass != -1)
        delay (1) ; */
    // wait so that interruptHandler is up und running. 
    // when interruptHandler is running, the calling function wiringPiISR
    // must be still alive, otherwise the thread argument &pin points into nirwana,
    // when it is picked up from interruptHandler.
      delay (10);
    } else {
      if (wiringPiDebug) {
        printf("wiringPi: pthread_create failed\n");
      }
    }

    if (wiringPiDebug) {
      printf("wiringPi: mutex out\n");
    }
  pthread_mutex_unlock (&pinMutex) ;

  if (wiringPiDebug) {
    printf("wiringPi: wiringPiISR finished\n");
  }
  return 0 ;
}

int wiringPiISR (int pin, int mode, void (*function)(void))
{
  return wiringPiISRInternal(pin, mode, NULL, function, 0, NULL);
}

int wiringPiISR2(int pin, int edgeMode, void (*function)(struct WPIWfiStatus wfiStatus, void* userdata), unsigned long debounce_period_us, void* userdata)
{
  return wiringPiISRInternal(pin, edgeMode, function, NULL, debounce_period_us, userdata);
}

/*
 * initialiseEpoch:
 *	Initialise our start-of-time variable to be the current unix
 *	time in milliseconds and microseconds.
 *********************************************************************************
 */

static void initialiseEpoch (void)
{
#ifdef	OLD_WAY
  struct timeval tv ;

  gettimeofday (&tv, NULL) ;
  epochMilli = (uint64_t)tv.tv_sec * (uint64_t)1000    + (uint64_t)(tv.tv_usec / 1000) ;
  epochMicro = (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)(tv.tv_usec) ;
#else
  struct timespec ts ;

  clock_gettime (CLOCK_MONOTONIC_RAW, &ts) ;
  epochMilli = (uint64_t)ts.tv_sec * (uint64_t)1000    + (uint64_t)(ts.tv_nsec / 1000000L) ;
  epochMicro = (uint64_t)ts.tv_sec * (uint64_t)1000000 + (uint64_t)(ts.tv_nsec /    1000L) ;
#endif
}


/*
 * delay:
 *	Wait for some number of milliseconds
 *********************************************************************************
 */

void delay (unsigned int ms)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(ms / 1000) ;
  sleeper.tv_nsec = (long)(ms % 1000) * 1000000 ;

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
 *
 *      Plan B: It seems all might not be well with that plan, so changing it
 *      to use gettimeofday () and poll on that instead...
 *********************************************************************************
 */

void delayMicrosecondsHard (unsigned int us)
{
  struct timeval tNow, tLong, tEnd ;

  gettimeofday (&tNow, NULL) ;
  tLong.tv_sec  = us / 1000000 ;
  tLong.tv_usec = us % 1000000 ;
  timeradd (&tNow, &tLong, &tEnd) ;

  while (timercmp (&tNow, &tEnd, <))
    gettimeofday (&tNow, NULL) ;
}

void delayMicroseconds (unsigned int us)
{
  struct timespec sleeper ;
  unsigned int uSecs = us % 1000000 ;
  unsigned int wSecs = us / 1000000 ;

  if      (us ==   0)
    return ;
  else if (us  < 100)
    delayMicrosecondsHard (us) ;
  else
  {
    sleeper.tv_sec  = wSecs ;
    sleeper.tv_nsec = (long)(uSecs * 1000L) ;
    nanosleep (&sleeper, NULL) ;
  }
}


/*
 * millis:
 *	Return a number of milliseconds as an unsigned int.
 *	Wraps at 49 days.
 *********************************************************************************
 */

unsigned int millis (void)
{
  uint64_t now ;

#ifdef	OLD_WAY
  struct timeval tv ;

  gettimeofday (&tv, NULL) ;
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000 + (uint64_t)(tv.tv_usec / 1000) ;

#else
  struct  timespec ts ;

  clock_gettime (CLOCK_MONOTONIC_RAW, &ts) ;
  now  = (uint64_t)ts.tv_sec * (uint64_t)1000 + (uint64_t)(ts.tv_nsec / 1000000L) ;
#endif

  return (uint32_t)(now - epochMilli) ;
}


/*
 * micros:
 *	Return a number of microseconds as an unsigned int.
 *	Wraps after 71 minutes.
 *********************************************************************************
 */

unsigned int micros (void)
{
  uint64_t now ;
#ifdef	OLD_WAY
  struct timeval tv ;

  gettimeofday (&tv, NULL) ;
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)tv.tv_usec ;
#else
  struct  timespec ts ;

  clock_gettime (CLOCK_MONOTONIC_RAW, &ts) ;
  now  = (uint64_t)ts.tv_sec * (uint64_t)1000000 + (uint64_t)(ts.tv_nsec / 1000) ;
#endif


  return (uint32_t)(now - epochMicro) ;
}


unsigned long long piMicros64(void) {
  struct  timespec ts;

  clock_gettime (CLOCK_MONOTONIC_RAW, &ts) ;
  uint64_t now  = (uint64_t)ts.tv_sec * (uint64_t)1000000 + (uint64_t)(ts.tv_nsec / 1000) ;
  return (now - epochMicro) ;
}

/*
 * wiringPiVersion:
 *	Return our current version number
 *********************************************************************************
 */

void wiringPiVersion (int *major, int *minor)
{
  *major = VERSION_MAJOR ;
  *minor = VERSION_MINOR ;
}


int wiringPiUserLevelAccess(void)
{
  struct stat statBuf ;
  const char* gpiomemModule = gpiomem_BCM;

  piBoard();
  if (piRP1Model()) {
    gpiomemModule = gpiomem_RP1;
  }

  return stat(gpiomemModule, &statBuf) == 0 ? 1 : 0;
}


int CheckPCIeFileContent(const char* pcieaddress, const char* filename, const char* content) {
  char file_path[512];
  int Found = 0;

  snprintf(file_path, sizeof(file_path), "%s/%s/%s", pcie_path, pcieaddress, filename);
  if (wiringPiDebug) { printf("Open: %s  ->", file_path); }
  FILE *device_file = fopen(file_path, "r");
  if (device_file != NULL) {
    char buffer[64];
    if (fgets(buffer, sizeof(buffer), device_file) != NULL) {
      if (wiringPiDebug) { printf("  %s", buffer); }
      if (strstr(buffer, content) != NULL) {
        Found = 1;
        if (wiringPiDebug) { printf("    >> correct\n"); }
      } else {
        if (wiringPiDebug) { printf("    >> wrong\n"); }
      }
    }
    fclose(device_file);
  } else {
    if (wiringPiDebug) { perror("fopen"); };
  }
  return Found;
}


void GetRP1Memory() {

    pciemem_RP1[0] = '\0';
    DIR *dir = opendir(pcie_path);
    struct dirent *entry;

    if (dir == NULL) {
        if (wiringPiDebug) { perror("opendir"); };
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_LNK) {
            if (CheckPCIeFileContent(entry->d_name, "device", pciemem_RP1_Device) &&
                CheckPCIeFileContent(entry->d_name, "vendor", pciemem_RP1_Ventor)) {
              snprintf(pciemem_RP1, sizeof(pciemem_RP1), "%s/%s/%s", pcie_path, entry->d_name, pciemem_RP1_bar);
              if (wiringPiDebug) { printf("RP1 device memory found at '%s'\n", pciemem_RP1); }
              break;
            }
        }
    }

    closedir(dir);
}


int wiringPiGlobalMemoryAccess(void)
{
  const char* gpiomemGlobal;
  int fd=-1;
  unsigned int MMAP_size;
  unsigned int BaseAddr, PWMAddr;

  piBoard();
  if (piRP1Model()) {
    GetRP1Memory(pciemem_RP1, sizeof(pciemem_RP1));
    gpiomemGlobal = pciemem_RP1;
    MMAP_size = pciemem_RP1_Size;
    BaseAddr  = 0x00000000;
    PWMAddr	  = 0x00000000;  //not supported so far
  } else {
    gpiomemGlobal = gpiomem_global;
    MMAP_size = BLOCK_SIZE;
    BaseAddr	= piGpioBase + 0x00200000 ;
    PWMAddr	  = piGpioBase + 0x0020C000 ;
  }

  if ((fd = open (gpiomemGlobal, O_RDWR | O_SYNC | O_CLOEXEC)) >0) {
    int returnvalue = 1; // OK

    uint32_t * lgpio = (uint32_t *)mmap(0, MMAP_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, BaseAddr) ;
    if (lgpio == MAP_FAILED) {
      returnvalue = 0;
      if (wiringPiDebug)
        fprintf(stderr,"wiringPiGlobalMemoryAccess: mmap (GPIO 0x%X,0x%X) failed: %s\n", BaseAddr, MMAP_size, strerror (errno)) ;
    } else {
      munmap(lgpio, MMAP_size);
      if (piRP1Model()) {
        returnvalue = 2;    // GPIO & PWM accessible (same area, nothing to mmap)
      } else {
        //check PWM area
        uint32_t* lpwm = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PWMAddr) ;
        if (lpwm == MAP_FAILED) {
          returnvalue = 1;    // only GPIO accessible
          if (wiringPiDebug)
            fprintf(stderr,"wiringPiGlobalMemoryAccess: mmap (PWM 0x%X,0x%X) failed: %s\n", PWMAddr, MMAP_size, strerror (errno)) ;
        } else {
          returnvalue = 2;  // GPIO & PWM accessible
          munmap(lpwm, BLOCK_SIZE);
        }
      }
    }

    close(fd);
    return returnvalue;
  }
  return 0;  // Failed!
}

/*
 * wiringPiSetup:
 *	Must be called once at the start of your program execution.
 *
 * Default setup: Initialises the system into wiringPi Pin mode and uses the
 *	memory mapped hardware directly.
 *
 * Changed now to revert to "gpio" mode if we're running on a Compute Module.
 *********************************************************************************
 */

int wiringPiSetup (void)
{
  int   fd ;
  int   model, rev, mem, maker, overVolted ;

  if (wiringPiSetuped)
    return 0 ;

  wiringPiSetuped = TRUE ;

  if (getenv (ENV_DEBUG) != NULL)
    wiringPiDebug = TRUE ;

  if (getenv (ENV_CODES) != NULL)
    wiringPiReturnCodes = TRUE ;

  if (wiringPiDebug)
    printf ("wiringPi: wiringPiSetup called\n") ;

// Get the board ID information. We're not really using the information here,
//	but it will give us information like the GPIO layout scheme (2 variants
//	on the older 26-pin Pi's) and the GPIO peripheral base address.
//	and if we're running on a compute module, then wiringPi pin numbers
//	don't really mean anything, so force native BCM mode anyway.

  piBoardId (&model, &rev, &mem, &maker, &overVolted) ;

  if ((model == PI_MODEL_CM) ||
      (model == PI_MODEL_CM3) ||
      (model == PI_MODEL_CM3P))
    wiringPiMode = WPI_MODE_GPIO ;
  else
    wiringPiMode = WPI_MODE_PINS ;

  if (piGpioLayout () == GPIO_LAYOUT_PI1_REV1)	// A, B, Rev 1, 1.1
  {
    pinToGpio =  pinToGpioR1 ;
    physToGpio = physToGpioR1 ;
  }
  else 					// A2, B2, A+, B+, CM, Pi2, Pi3, Zero, Zero W, Zero 2 W
  {
    pinToGpio =  pinToGpioR2 ;
    physToGpio = physToGpioR2 ;
  }

// Open the master /dev/ memory control device
// Device strategy: December 2016:
//	Try /dev/mem. If that fails, then
//	try /dev/gpiomem. If that fails then game over.

  const char* gpiomemGlobal = gpiomem_global;
  const char* gpiomemModule = gpiomem_BCM;

  if (piRP1Model()) {
    GetRP1Memory();
    gpiomemGlobal = pciemem_RP1;
    gpiomemModule = gpiomem_RP1;

    // PWM alt pins @RP1 - need to be translated to RP1_FSEL with pinModeAlt
    gpioToPwmALT[12] = FSEL_ALT0;
    gpioToPwmALT[13] = FSEL_ALT0;
    gpioToPwmALT[18] = FSEL_ALT3;
    gpioToPwmALT[19] = FSEL_ALT3;
    //PWM0 channel @RP1
    gpioToPwmPort[12] = 0;
    gpioToPwmPort[13] = 1;
    gpioToPwmPort[18] = 2;
    gpioToPwmPort[19] = 3;
  }

  usingGpioMem = FALSE;
  if (gpiomemGlobal==NULL || (fd = open (gpiomemGlobal, O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
  {
    if (wiringPiDebug) {
      printf ("wiringPi: no access to %s try %s\n", gpiomemGlobal, gpiomemModule) ;
    }
    if (gpiomemModule && (fd = open (gpiomemModule, O_RDWR | O_SYNC | O_CLOEXEC) ) >= 0)	// We're using gpiomem
    {
      piGpioBase   = 0 ;
      usingGpioMem = TRUE ;
    }
    else
      return wiringPiFailure (WPI_ALMOST, "wiringPiSetup: Unable to open %s or %s: %s.\n"
	"  Aborting your program because if it can not access the GPIO\n"
	"  hardware then it most certianly won't work\n"
	"  Try running with sudo?\n", gpiomemGlobal, gpiomemModule, strerror (errno)) ;
  }
  if (wiringPiDebug) {
    printf ("wiringPi: access to %s succeded %d\n", usingGpioMem ? gpiomemModule : gpiomemGlobal, fd) ;
  }
//	GPIO:
  if (!piRP1Model()) {
   //Set the offsets into the memory interface.

    GPIO_PADS 	= piGpioBase + 0x00100000 ;
    GPIO_CLOCK_ADR = piGpioBase + 0x00101000 ;
    GPIO_BASE	  = piGpioBase + 0x00200000 ;
    GPIO_TIMER	= piGpioBase + 0x0000B000 ;
    GPIO_PWM	  = piGpioBase + 0x0020C000 ;
    GPIO_RIO    = 0x00 ;

// Map the individual hardware components

  //	GPIO:
    base = NULL;
    gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_BASE) ;
    if (gpio == MAP_FAILED)
      return wiringPiFailure (WPI_ALMOST, "wiringPiSetup: mmap (GPIO) failed: %s\n", strerror (errno)) ;

  //	PWM

    pwm = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_PWM) ;
    if (pwm == MAP_FAILED)
      return wiringPiFailure (WPI_ALMOST, "wiringPiSetup: mmap (PWM) failed: %s\n", strerror (errno)) ;

  //	Clock control (needed for PWM)

    clk = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_CLOCK_ADR) ;
    if (clk == MAP_FAILED)
      return wiringPiFailure (WPI_ALMOST, "wiringPiSetup: mmap (CLOCK) failed: %s\n", strerror (errno)) ;

  //	The drive pads

    pads = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_PADS) ;
    if (pads == MAP_FAILED)
      return wiringPiFailure (WPI_ALMOST, "wiringPiSetup: mmap (PADS) failed: %s\n", strerror (errno)) ;

  //	The system timer

    timer = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_TIMER) ;
    if (timer == MAP_FAILED)
      return wiringPiFailure (WPI_ALMOST, "wiringPiSetup: mmap (TIMER) failed: %s\n", strerror (errno)) ;

  // Set the timer to free-running, 1MHz.
  //	0xF9 is 249, the timer divide is base clock / (divide+1)
  //	so base clock is 250MHz / 250 = 1MHz.

    *(timer + TIMER_CONTROL) = 0x0000280 ;
    *(timer + TIMER_PRE_DIV) = 0x00000F9 ;
    timerIrqRaw = timer + TIMER_IRQ_RAW ;

    // Export the base addresses for any external software that might need them
    _wiringPiBase  = base ;
    _wiringPiGpio  = gpio ;
    _wiringPiPwm   = pwm ;
    _wiringPiClk   = clk ;
    _wiringPiPads  = pads ;
    _wiringPiTimer = timer ;
    _wiringPiRio   = NULL ;
  } else {
    unsigned int MMAP_size = (usingGpioMem) ? gpiomem_RP1_Size : pciemem_RP1_Size;

    GPIO_PADS 	= (RP1_PADS0_Addr-RP1_IO0_Addr) ;
    GPIO_CLOCK_ADR = (RP1_CLOCK_Addr-RP1_BASE_Addr);
    GPIO_BASE	  = (RP1_IO0_Addr-RP1_BASE_Addr) ;
    GPIO_TIMER	=  0x00;
    GPIO_PWM	  = RP1_PWM0_Addr-RP1_BASE_Addr;
    GPIO_RIO    = (RP1_SYS_RIO0_Addr-RP1_IO0_Addr) ;

    //map hole RP1 memory block from beginning,
    base = (unsigned int *)mmap(0, MMAP_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x00000000) ;
    if (base == MAP_FAILED)
      return wiringPiFailure (WPI_ALMOST, "wiringPiSetup: mmap failed: %s\n", strerror (errno)) ;
    if (usingGpioMem) {
      gpio = base;              // RP1 start adress of map memory for gpio (same as module memory)
      pwm = NULL;               // outside of mapped memory, PWM not available from none root
      clk = NULL;               // outside of mapped memory, CLK main not available from none root
    } else {
      gpio = &base[GPIO_BASE/4];      // RP1 start adress of map memory for gpio
      pwm  = &base[GPIO_PWM/4];       // RP1 start adress of map memory for pwm0
      clk  = &base[GPIO_CLOCK_ADR/4]; // RP1 start adress of map memory for clocks_main
    }
    pads = &gpio[GPIO_PADS/4];  // RP1 start adress of map memory for pads
    rio  = &gpio[GPIO_RIO/4];   // RP1 start adress of map memory for rio
    GPIO_PADS += GPIO_BASE;
    GPIO_RIO += GPIO_BASE;

    // Export the base addresses for any external software that might need them
    _wiringPiBase  = base ;
    _wiringPiGpio  = gpio ;
    _wiringPiPwm   = pwm  ;
    _wiringPiClk   = clk  ;
    _wiringPiPads  = pads ;
    _wiringPiTimer = NULL ;
    _wiringPiRio   = rio ;
  }
  if (wiringPiDebug) {
    printf ("wiringPi: memory map gpio   0x%x %s\n", GPIO_BASE     , _wiringPiGpio ? "valid" : "invalid");
    printf ("wiringPi: memory map pads   0x%x %s\n", GPIO_PADS     , _wiringPiPads ? "valid" : "invalid");
    printf ("wiringPi: memory map rio    0x%x %s\n", GPIO_RIO      , _wiringPiRio  ? "valid" : "invalid");
    printf ("wiringPi: memory map pwm0   0x%x %s\n", GPIO_PWM      , _wiringPiPwm  ? "valid" : "invalid");
    printf ("wiringPi: memory map clocks 0x%x %s\n", GPIO_CLOCK_ADR, _wiringPiClk  ? "valid" : "invalid");
    printf ("wiringPi: memory map timer  0x%x %s\n", GPIO_TIMER    ,_wiringPiTimer ? "valid" : "invalid");
  }

  initialiseEpoch () ;

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
  (void)wiringPiSetup () ;

  if (wiringPiDebug)
    printf ("wiringPi: wiringPiSetupGpio called\n") ;

  wiringPiMode = WPI_MODE_GPIO ;

  return 0 ;
}


/*
 * wiringPiSetupPhys:
 *	Must be called once at the start of your program execution.
 *
 * Phys setup: Initialises the system into Physical Pin mode and uses the
 *	memory mapped hardware directly.
 *********************************************************************************
 */

int wiringPiSetupPhys (void)
{
  (void)wiringPiSetup () ;

  if (wiringPiDebug)
    printf ("wiringPi: wiringPiSetupPhys called\n") ;

  wiringPiMode = WPI_MODE_PHYS ;

  return 0 ;
}

int wiringPiSetupPinType (enum WPIPinType pinType) {
  if (wiringPiDebug)
    printf ("wiringPi: wiringPiSetupPinType(%d) called\n", (int) pinType) ;
  switch (pinType) {
    case WPI_PIN_BCM:  return wiringPiSetupGpio();
    case WPI_PIN_WPI:  return wiringPiSetup();
    case WPI_PIN_PHYS: return wiringPiSetupPhys();
    default:           return -1;
  }
}


int wiringPiSetupGpioDevice (enum WPIPinType pinType) {
 if (wiringPiSetuped)
    return 0 ;
  if (wiringPiDebug) {
    printf ("wiringPi: wiringPiSetupGpioDevice(%d) called\n", (int)pinType) ;
  }
  if (getenv (ENV_DEBUG) != NULL)
    wiringPiDebug = TRUE ;

  if (getenv (ENV_CODES) != NULL)
    wiringPiReturnCodes = TRUE ;

  if (wiringPiGpioDeviceGetFd()<0) {
    return -1;
  }
  wiringPiSetuped = TRUE ;

  if (piGpioLayout () == GPIO_LAYOUT_PI1_REV1){
    pinToGpio  = pinToGpioR1 ;
    physToGpio = physToGpioR1 ;
  } else {
    pinToGpio  = pinToGpioR2 ;
    physToGpio = physToGpioR2 ;
  }

  initialiseEpoch () ;

  switch (pinType) {
    case WPI_PIN_BCM:
      wiringPiMode = WPI_MODE_GPIO_DEVICE_BCM;
      break;
    case WPI_PIN_WPI:
      wiringPiMode = WPI_MODE_GPIO_DEVICE_WPI;
      break;
    case WPI_PIN_PHYS:
      wiringPiMode = WPI_MODE_GPIO_DEVICE_PHYS;
      break;
    default:
      wiringPiSetuped = FALSE;
      return -1;
  }

  return 0 ;
}

/*
 * wiringPiSetupSys:
 * GPIO Sysfs Interface for Userspace is deprecated
 *   https://www.kernel.org/doc/html/v5.5/admin-guide/gpio/sysfs.html
 *
 * Switched to new GPIO driver Interface in version 3.3
 */

int wiringPiSetupSys (void)
{
  if (wiringPiSetuped)
    return 0 ;
  if (wiringPiDebug)
    printf ("wiringPi: wiringPiSetupSys called\n") ;
  return wiringPiSetupGpioDevice(WPI_PIN_BCM);
}
