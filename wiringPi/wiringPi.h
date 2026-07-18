/*
 * wiringPi.h:
 *	Arduino like Wiring library for the Raspberry Pi.
 *	Copyright (c) 2012–2019 Gordon Henderson; 2019–2026 Contributors
 ***********************************************************************
 * This file is part of wiringPi:
 *  https://github.com/WiringPi/WiringPi/
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

#ifndef __WIRING_PI_H__
#define __WIRING_PI_H__

#include <stdbool.h>

// macros retained for old code compatibility, we now use stdbool.h
#ifndef TRUE
  #define TRUE true
#endif
#ifndef FALSE
  #define FALSE false
#endif

// GCC warning suppressor

#define UNU __attribute__((unused))

// Mask for the bottom 64 pins which belong to the Raspberry Pi
//  The others are available for the other devices

#define PI_GPIO_MASK (0xFFFFFFC0)

// Handy defines

// wiringPi modes
enum WPI_MODE {
  WPI_MODE_PINS             = 0,
  WPI_MODE_GPIO             = 1,
  WPI_MODE_GPIO_SYS         = 2,  // deprecated since 3.2
  WPI_MODE_PHYS             = 3,
  WPI_MODE_PIFACE           = 4,
  WPI_MODE_GPIO_DEVICE_BCM  = 5,  // BCM pin numbers like WPI_MODE_GPIO
  WPI_MODE_GPIO_DEVICE_WPI  = 6,  // WiringPi pin numbers like WPI_MODE_PINS
  WPI_MODE_GPIO_DEVICE_PHYS = 7,  // Physic pin numbers like WPI_MODE_PHYS

  WPI_MODE_UNINITIALISED    = -1,
};

// Pin modes
enum WPI_PIN_MODE {
  INPUT            = 0,
  OUTPUT           = 1,
  PWM_OUTPUT       = 2,
  PWM_MS_OUTPUT    = 8,
  PWM_BAL_OUTPUT   = 9,
  GPIO_CLOCK       = 3,
  SOFT_PWM_OUTPUT  = 4,
  SOFT_TONE_OUTPUT = 5,
  PWM_TONE_OUTPUT  = 6,
  PM_OFF           = 7  // to input / release line
};

enum WPI_PIN_LEVEL {
  LOW  = 0,
  HIGH = 1
};

// Pull up/down/none
enum WPI_PUD_MODE {
  PUD_OFF  = 0,
  PUD_DOWN = 1,
  PUD_UP   = 2
};

// PWM
enum WPI_PWM_MODE {
  PWM_MODE_MS  = 0,
  PWM_MODE_BAL = 1
};

// Interrupt levels
enum WPI_INTERRUPT_LEVEL {
  INT_EDGE_SETUP   = 0,
  INT_EDGE_FALLING = 1,
  INT_EDGE_RISING  = 2,
  INT_EDGE_BOTH    = 3
};

// Pi model types and version numbers
//  Intended for the GPIO program Use at your own risk.
// https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#new-style-revision-codes
// https://github.com/raspberrypi/documentation/blob/develop/documentation/asciidoc/computers/raspberry-pi/revision-codes.adoc

enum PI_MODEL {
  PI_MODEL_A       = 0,
  PI_MODEL_B       = 1,
  PI_MODEL_AP      = 2,
  PI_MODEL_BP      = 3,
  PI_MODEL_2       = 4,
  PI_ALPHA         = 5,
  PI_MODEL_CM      = 6,

  PI_MODEL_3B      = 8,
  PI_MODEL_ZERO    = 9,
  PI_MODEL_CM3     = 10,

  PI_MODEL_ZERO_W  = 12,
  PI_MODEL_3BP     = 13,
  PI_MODEL_3AP     = 14,

  PI_MODEL_CM3P    = 16,
  PI_MODEL_4B      = 17,
  PI_MODEL_ZERO_2W = 18,
  PI_MODEL_400     = 19,
  PI_MODEL_CM4     = 20,
  PI_MODEL_CM4S    = 21,

  PI_MODEL_5       = 23,
  PI_MODEL_CM5     = 24,
  PI_MODEL_500     = 25,
  PI_MODEL_CM5L    = 26
};

#define PI_MODELS_MAX 27

enum PI_VERSION {
  PI_VERSION_1   = 0,
  PI_VERSION_1_1 = 1,
  PI_VERSION_1_2 = 2,
  PI_VERSION_2   = 3
};

enum PI_MAKER {
  PI_MAKER_SONY    = 0,
  PI_MAKER_EGOMAN  = 1,
  PI_MAKER_EMBEST  = 2,
  PI_MAKER_UNKNOWN = 3
};

enum GPIO_LAYOUT {
  GPIO_LAYOUT_PI1_REV1 = 1,  // Pi 1 A/B Revision 1, 1.1, CM
  GPIO_LAYOUT_DEFAULT  = 2
};

extern const char *piModelNames    [PI_MODELS_MAX];
extern const char *piProcessor     [ 5] ;
extern const char *piRevisionNames [16] ;
extern const char *piMakerNames    [16] ;
extern const int   piMemorySize    [ 8] ;


//  Intended for the GPIO program Use at your own risk.

// Threads

#define PI_THREAD(X) void *X(UNU void *dummy)

// Failure modes

enum WPI_FAILURE_MODE {
  WPI_FATAL  = true,
  WPI_ALMOST = false
};

// wiringPiNodeStruct:
//  This describes additional device nodes in the extended wiringPi
//  2.0 scheme of things.
//  It's a simple linked list for now, but will hopefully migrate to
//  a binary tree for efficiency reasons - but then again, the chances
//  of more than 1 or 2 devices being added are fairly slim, so who
//  knows....

struct wiringPiNodeStruct {
  int pinBase;
  int pinMax;

  int          fd;     // Node specific
  unsigned int data0;  //  ditto
  unsigned int data1;  //  ditto
  unsigned int data2;  //  ditto
  unsigned int data3;  //  ditto

  void (*pinMode)         (struct wiringPiNodeStruct *node, int pin, int mode) ;
  void (*pullUpDnControl) (struct wiringPiNodeStruct *node, int pin, int mode) ;
  int  (*digitalRead)     (struct wiringPiNodeStruct *node, int pin) ;
  void (*digitalWrite)    (struct wiringPiNodeStruct *node, int pin, int value) ;
  void (*pwmWrite)        (struct wiringPiNodeStruct *node, int pin, int value) ;
  int  (*analogRead)      (struct wiringPiNodeStruct *node, int pin) ;
  void (*analogWrite)     (struct wiringPiNodeStruct *node, int pin, int value) ;

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
//  c++ wrappers thanks to a comment by Nick Lott
//  (and others on the Raspberry Pi forums)

#ifdef __cplusplus
extern "C" {
#endif

// Data

// Internal
extern void piGpioLayoutOops (const char *why);
extern int wiringPiFailure (int fatal, const char *message, ...) ;

// Core wiringPi functions

extern struct wiringPiNodeStruct *wiringPiFindNode (int pin) ;
extern struct wiringPiNodeStruct *wiringPiNewNode  (int pinBase, int numPins) ;

enum WPIPinType {
  WPI_PIN_BCM = 1,
  WPI_PIN_WPI,
  WPI_PIN_PHYS,
};

extern void wiringPiVersion (int *major, int *minor) ;
extern int  wiringPiGlobalMemoryAccess(void);                 //Interface V3.3
extern int  wiringPiUserLevelAccess (void) ;
extern int  wiringPiSetup       (void) ;
extern int  wiringPiSetupSys    (void) ;
extern int  wiringPiSetupGpio   (void) ;
extern int  wiringPiSetupPhys   (void) ;
extern int  wiringPiSetupPinType (enum WPIPinType pinType);   //Interface V3.3
extern int  wiringPiSetupGpioDevice(enum WPIPinType pinType); //Interface V3.3


enum WPIPinAlt {
  WPI_ALT_UNKNOWN = -1,
  WPI_ALT_INPUT   = 0,
  WPI_ALT_OUTPUT,
  WPI_ALT5,
  WPI_ALT4,
  WPI_ALT0,
  WPI_ALT1,
  WPI_ALT2,
  WPI_ALT3,
  WPI_ALT6,
  WPI_ALT7,
  WPI_ALT8,
  WPI_ALT9,
  WPI_NONE = 0x1F,  // Pi5 default
};


extern int  wiringPiGpioDeviceGetFd();               //Interface V3.3
extern void pinModeAlt          (int pin, int mode) ;
extern enum WPIPinAlt getPinModeAlt       (int pin) ;  // Interface V3.5, same as getAlt but wie enum
extern void pinMode             (int pin, int mode) ;
extern void pullUpDnControl     (int pin, int pud) ;
extern int  digitalRead         (int pin) ;
extern void digitalWrite        (int pin, int value) ;
extern void pwmWrite            (int pin, int value) ;
extern int  analogRead          (int pin) ;
extern void analogWrite         (int pin, int value) ;

// PiFace specifics
//  (Deprecated)

extern int wiringPiSetupPiFace(void);
extern int wiringPiSetupPiFaceForGpioProg(void);  // Don't use this - for gpio program only

// On-Board Raspberry Pi hardware specific stuff

extern int          piGpioLayout(void);
extern int          piBoardRev(void);  // Deprecated, but does the same as piGpioLayout
extern void         piBoardId(int *model, int *rev, int *mem, int *maker, int *overVolted);
extern int          piBoard40Pin(void);          // V3.8
extern int          piRP1Model(void);            // V3.14
extern int          wpiPinToGpio(int wpiPin);    // please don't use outside 0-63 and on RP1
extern int          physPinToGpio(int physPin);  // please don't use outside 0-63 and on RP1
extern void         setPadDrive(int group, int value);
extern void         setPadDrivePin(int pin, int value);  // V3.0
extern int          getAlt(int pin);
extern void         pwmToneWrite(int pin, int freq);
extern void         pwmSetMode(int mode);
extern void         pwmSetRange(unsigned int range);
extern void         pwmSetChannelRange(unsigned int channel, unsigned int range);
extern void         pwmSetPinRange(int pin, unsigned int range);
extern void         pwmSetClock(int divisor);
extern void         gpioClockSet(int pin, int freq);
extern unsigned int digitalReadByte(void);
extern unsigned int digitalReadByte2(void);
extern void         digitalWriteByte(int value);
extern void         digitalWriteByte2(int value);

// Interrupts
// status returned from waitForInterruptV2    V3.16
struct WPIWfiStatus {
    int statusOK;               // -1: error (return of 'poll' command), 0: timeout, 1: irq processed, next data values are valid if needed
    unsigned int pinBCM;        // gpio as BCM pin
    int edge;                   // INT_EDGE_FALLING or INT_EDGE_RISING
    long long int timeStamp_us; // time stamp in microseconds
};

//extern int  waitForInterrupt    (int pin, int ms);  disabled since V3.16
// Legacy call 'waitForInterrupt' is no longer supported.
// It depended on sysfs edge configuration "gpio edge 17 falling",
// which has been removed in modern kernels.
// Use 'waitForInterrupt2' instead and define the desired edge explicitly.
// Note: 'waitForInterrupt2' also provides built-in debounce support,
extern int  wiringPiISR         (int pin, int mode, void (*function)(void)) ;
extern struct WPIWfiStatus  waitForInterrupt2(int pin, int edgeMode, int ms, unsigned long debounce_period_us) ;   // V3.16
extern int  wiringPiISR2       (int pin, int edgeMode, void (*function)(struct WPIWfiStatus wfiStatus, void* userdata), unsigned long debounce_period_us, void* userdata) ;  // V3.16
extern int  wiringPiISRStop     (int pin) ;  //V3.2
extern int  waitForInterruptClose(int pin) ; //V3.2 legacy use wiringPiISRStop

// Threads

extern int  piThreadCreate      (void *(*fn)(void *)) ;
extern void piLock              (int key) ;
extern void piUnlock            (int key) ;

// Schedulling priority

extern int piHiPri (const int pri) ;

// Extras from arduino land

extern void         delay             (unsigned int ms) ;
extern void         delayMicroseconds (unsigned int us) ;
extern unsigned int millis            (void) ;
extern unsigned int micros            (void) ;
extern unsigned long long piMicros64(void);   // V3.8

extern unsigned long long pulseIn64  (int pin, int level, unsigned long long timeout_us); // V3.20
extern unsigned int       pulseIn    (int pin, int level, unsigned int       timeout);    // V3.20
extern unsigned long long frequencyIn(int pin, unsigned long window_ms);                  // V3.20

#ifdef __cplusplus
}
#endif

#endif
