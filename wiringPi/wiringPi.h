/*
 * wiringPi.h:
 *  Arduino like Wiring library for the Raspberry Pi.
 *  Copyright (c) 2012-2017 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 * https://github.com/WiringPi/WiringPi/
 *
 * wiringPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * wiringPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#ifndef __WIRING_PI_H__
#define __WIRING_PI_H__

#include <stdbool.h>
#ifndef TRUE
#  define TRUE  true
#  define FALSE false
#endif

// GCC warning suppressor - don't warn about unused parameters (-Wunused-parameter)
#define UNU __attribute__((unused))

// Mask for the bottom 64 pins which belong to the Raspberry Pi
//    The others are available for the other devices
// @TODO Replace these checks with a check for (pin < 64)
#define PI_GPIO_MASK (0xFFFFFFC0)


// wiringPi modes
#define WPI_MODE_PINS     0   // Virtual pin numbers 0 through 16
#define WPI_MODE_GPIO     1   // Broadcom GPIO pin numbers
#define WPI_MODE_GPIO_SYS 2   // Broadcom GPIO pin numbers, but uses /sys/class/gpio (slower)
#define WPI_MODE_PHYS     3   // Raspberry Pi physical pins
#define WPI_MODE_PIFACE   4   // UNUSED
#define WPI_MODE_UNINITIALISED    -1

// Pin modes
#define INPUT             0
#define OUTPUT            1
#define PWM_OUTPUT        2
#define GPIO_CLOCK        3
#define SOFT_PWM_OUTPUT   4
#define SOFT_TONE_OUTPUT  5
#define PWM_TONE_OUTPUT   6

// Pin levels
#define LOW               0
#define HIGH              1

// Pull up/down/none
#define PUD_OFF           0
#define PUD_DOWN          1
#define PUD_UP            2

// PWM
#define PWM_MODE_MS       0
#define PWM_MODE_BAL      1

// Interrupt levels
#define INT_EDGE_SETUP    0
#define INT_EDGE_FALLING  1
#define INT_EDGE_RISING   2
#define INT_EDGE_BOTH     3

// Pi model types and version numbers
// Intended for the GPIO program
// Use at your own risk
// https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#new-style-revision-codes
// Format (bits): FMMM'CCCC'PPPP'TTTTTTTT'RRRR

// F bit (New flag)
#define PI_FLAG_OLD 0 // old-style revision
#define PI_FLAG_NEW 1 // new-style revision

// MMM bits (Memory size)
#define PI_MEM_256M 0
#define PI_MEM_512M 1
#define PI_MEM_1G   2
#define PI_MEM_2G   3
#define PI_MEM_4G   4
#define PI_MEM_8G   5

// CCCC bits (Manufacturer)
#define PI_MAKER_SONY      0  // Sony UK
#define PI_MAKER_EGOMAN    1
#define PI_MAKER_EMBEST    2
#define PI_MAKER_SONYJAPAN 3
#define PI_MAKER_EMBEST4   4
#define PI_MAKER_STADIUM   5

// PPPP bits (Processor)
#define PI_PROC_BCM2835 0
#define PI_PROC_BCM2836 1
#define PI_PROC_BCM2837 2
#define PI_PROC_BCM2711 3

// TTTTTTTT bits (Type)
#define PI_MODEL_A        0x00
#define PI_MODEL_B        0x01
#define PI_MODEL_AP       0x02
#define PI_MODEL_BP       0x03
#define PI_MODEL_2B       0x04
#define PI_ALPHA          0x05
#define PI_MODEL_CM1      0x06
// 0x07
#define PI_MODEL_3B       0x08
#define PI_MODEL_ZERO     0x09
#define PI_MODEL_CM3      0x0A
// 0x0B
#define PI_MODEL_ZERO_W   0x0C
#define PI_MODEL_3BP      0x0D
#define PI_MODEL_3AP      0x0E
// 0x0F
#define PI_MODEL_CM3P     0x10
#define PI_MODEL_4B       0x11
#define PI_MODEL_ZERO_2W  0x12
#define PI_MODEL_400      0x13
#define PI_MODEL_CM4      0x14

// RRRR bits (Revision)
#define PI_VERSION_1      0
#define PI_VERSION_1_1    1
#define PI_VERSION_1_2    2
#define PI_VERSION_2      3
#define PI_VERSION_1_4    4


extern const char *piModelNames    [21];
extern const char *piRevisionNames [ 5];
extern const char *piMakerNames    [16];
extern const char *piProcessorNames[ 5];
extern const int   piMemorySize    [ 8];


// Intended for the GPIO program
// Use at your own risk.

// Threads
#define PI_THREAD(X) void *X (UNU void *dummy)

// Failure modes
#define WPI_FATAL     true
#define WPI_NON_FATAL false


// wiringPiNodeStruct:
// This describes additional device nodes in the extended wiringPi
// 2.0 scheme of things. It's a simple linked list.
struct wiringPiNodeStruct
{
  int     pinBase;
  int     pinMax;

  int          fd;    // Node specific
  unsigned int data0; //  ditto
  unsigned int data1; //  ditto
  unsigned int data2; //  ditto
  unsigned int data3; //  ditto

  void   (*pinMode)          (struct wiringPiNodeStruct *node, int pin, int mode);
  void   (*pullUpDnControl)  (struct wiringPiNodeStruct *node, int pin, int mode);
  int    (*digitalRead)      (struct wiringPiNodeStruct *node, int pin);
  void   (*digitalWrite)     (struct wiringPiNodeStruct *node, int pin, int value);
  void   (*pwmWrite)         (struct wiringPiNodeStruct *node, int pin, int value);
  int    (*analogRead)       (struct wiringPiNodeStruct *node, int pin);
  void   (*analogWrite)      (struct wiringPiNodeStruct *node, int pin, int value);

  struct wiringPiNodeStruct *next;
};

extern struct wiringPiNodeStruct *wiringPiNodes;

// Export variables for the hardware pointers
extern volatile unsigned int *_wiringPiGpio;
extern volatile unsigned int *_wiringPiPwm;
extern volatile unsigned int *_wiringPiClk;
extern volatile unsigned int *_wiringPiPads;
extern volatile unsigned int *_wiringPiTimer;
extern volatile unsigned int *_wiringPiTimerIrqRaw;


// Function prototypes
//    c++ wrappers thanks to a comment by Nick Lott
//    (and others on the Raspberry Pi forums)
#ifdef __cplusplus
extern "C" {
#endif

// Data

// Internal
extern int wiringPiFailure (int fatal, const char *message, ...);

// Core wiringPi functions
extern struct wiringPiNodeStruct *wiringPiFindNode (int pin);
extern struct wiringPiNodeStruct *wiringPiNewNode  (int pinBase, int numPins);

extern void wiringPiVersion     (int *major, int *minor);
extern int  wiringPiSetup       (void);
extern int  wiringPiSetupSys    (void);
extern int  wiringPiSetupGpio   (void);
extern int  wiringPiSetupPhys   (void);

extern void pinModeAlt          (int pin, int mode); // opposite of getAlt
extern void pinMode             (int pin, int mode);
extern void pullUpDnControl     (int pin, int pud);
extern int  digitalRead         (int pin);
extern void digitalWrite        (int pin, int value);
extern void pwmWrite            (int pin, int value);
extern int  analogRead          (int pin);
extern void analogWrite         (int pin, int value);

// PiFace specifics
//    (Deprecated)
extern int  wiringPiSetupPiFace (void);
extern int  wiringPiSetupPiFaceForGpioProg (void);    // Don't use this - for gpio program only

// On-Board Raspberry Pi hardware specific stuff
extern int  piGpioLayout        (void);
extern void piBoardId           (int *model, int *proc, int *rev, int *mem, int *maker, int *overVolted);
extern int  wpiPinToGpio        (int wpiPin);
extern int  physPinToGpio       (int physPin);
extern void setPadDrive         (int group, int value);
extern int  getAlt              (int pin);
#define setAlt(pin, mode) pinModeAlt(pin, mode)  // for symmetry

// PWM funcs
extern void pwmToneWrite        (int pin, int freq);
extern void pwmSetMode          (int mode); // sets PWM mode
extern void pwmSetRange         (unsigned int range);
extern void pwmSetClock         (int divisor);

extern void gpioClockSet        (int pin, int freq);
extern unsigned int digitalReadByte(void);
extern unsigned int digitalReadByte2(void);
extern void digitalWriteByte    (int value);
extern void digitalWriteByte2   (int value);

// Interrupts
//    (Also Pi hardware specific)
extern int  waitForInterrupt    (int pin, int mS); // Deprecated
extern int  wiringPiISR         (int pin, int mode, void (*function)(void));

// Threads
extern int  piThreadCreate      (void *(*fn)(void *));
extern void piLock              (int key);
extern void piUnlock            (int key);

// Schedulling priority
extern int piHiPri (const int pri);

// Extras from arduino land
extern void         delay             (unsigned int howLong); // milliseconds
extern void         delayMicroseconds (unsigned int howLong);
extern unsigned int millis            (void);
extern unsigned int micros            (void);

#ifdef __cplusplus
}
#endif

#endif // __WIRING_PI_H__
