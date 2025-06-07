/*
 * gpio.c:
 *	Swiss-Army-Knife, Set-UID command-line interface to the Raspberry
 *	Pi's GPIO.
 *	Copyright (c) 2012-2025 Gordon Henderson and contributors
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
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
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <wiringPi.h>
#include <wpiExtensions.h>

#include <gertboard.h>
#include <piFace.h>

#include "../version.h"

extern int wiringPiDebug ;
int gpioDebug ;

// External functions I can't be bothered creating a separate .h file for:

extern void doReadall    (void) ;
extern void doAllReadall (void) ;
extern void doQmode      (int argc, char *argv []) ;

#ifndef TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

#define	PI_USB_POWER_CONTROL	38
#define	I2CDETECT		"i2cdetect"
#define	MODPROBE		"modprobe"
#define	RMMOD			"rmmod"

int wpMode ;

char *usage = "Usage: gpio -v\n"
              "       gpio -h\n"
              "       gpio [-g|-1] ...\n"
              "         where -g: pin numbering according to readall BCM column\n"
              "         where -1: pin numbering according to readall Physical column\n"
              "         when omitted: pin numbering according to readall wPi column\n"
              "       gpio [-d] ...\n"
              "       [-x extension:params] [[ -x ...]] ...\n"
              "       gpio [-p] <read/write/wb> ...\n"
              "       gpio <mode/read/write/aread/awritewb/pwm/pwmTone/clock> ...\n"
              "       gpio <toggle/blink> <pin>\n"
              "       gpio readall\n"
              "       gpio wfi <pin> <mode>\n"
              "         where <mode> can be: rising, falling, both\n"
              "       gpio drive <group> <value>\n"
              "       gpio pwm-bal/pwm-ms \n"
              "       gpio pwmr <range> \n"
              "       gpio pwmc <divider> \n"
              "       gpio i2cd/i2cdetect\n"
              "       gpio rbx/rbd\n"
              "       gpio wb <value>\n"
              "       gpio usbp high/low\n"
              "       gpio gbr <channel>\n"
              "       gpio gbw <channel> <value>" ;	// No trailing newline needed here.


#ifdef	NOT_FOR_NOW
/*
 * decodePin:
 *	Decode a pin "number" which can actually be a pin name to represent
 *	one of the Pi's on-board pins.
 *********************************************************************************
 */

static int decodePin (const char *str)
{

// The first case - see if it's a number:

  if (isdigit (str [0]))
    return atoi (str) ;

  return 0 ;
}
#endif



/*
 * doI2Cdetect:
 *	Run the i2cdetect command with the right runes for this Pi revision
 *********************************************************************************
 */

static void doI2Cdetect (const char *progName)
{
  int port = piGpioLayout () == GPIO_LAYOUT_PI1_REV1 ? 0 : 1 ;
  char command[64];

  snprintf(command, 64, "i2cdetect -y %d", port);
  int ret = system(command);
  if (ret < 0) {
    fprintf (stderr, "%s: Unable to run i2cdetect: %s\n", progName, strerror(errno));
  }
  if (0x7F00 == (ret & 0xFF00)) {
    fprintf (stderr, "%s: i2cdetect not found, please install i2c-tools\n", progName);
  }
}


void SYSFS_DEPRECATED(const char *progName) {
  fprintf(stderr, "%s: GPIO Sysfs Interface for Userspace is deprecated (https://www.kernel.org/doc/Documentation/gpio/sysfs.txt).\n Function is now useless and empty.\n\n", progName);
}

void LOAD_DEPRECATED(const char *progName) {
  fprintf(stderr, "%s: load/unload modules is deprecated. You need to run the raspi-config program (as root) and select the interface option (SPI or I2C) that you wish to de-/activate.\n\n", progName);
}

/*
 * doExports:  -> deprecated, removed
 *	List all GPIO exports
 *********************************************************************************
 */

/*
 * doExport:  -> deprecated, removed
 *	gpio export pin mode
 *	This uses the /sys/class/gpio device interface.
 *********************************************************************************
 */


/*
 * doWfi:
 *	gpio wfi pin mode
 *	Wait for Interrupt on a given pin.
 *	Slight cheat here - it's easier to actually use ISR now (which calls
 *	gpio to set the pin modes!) then we simply sleep, and expect the thread
 *	to exit the program. Crude but effective.
 *********************************************************************************
 */

static volatile int globalIterations ;
static volatile int globalCounter ;

void printgpioflush(const char* text) {
  if (gpioDebug) {
    printf("%s", text); 
    fflush(stdout);
  }
}

void printgpio(const char* text) {
  if (gpioDebug) {
    printf("%s", text);
  }
}

static void wfi (void) {
  globalCounter++;
  if(globalCounter>=globalIterations) {
    printgpio("finished\n");
    exit(0);
  } else {
    printgpioflush("I");
  }
}


static void wfi2(struct WPIWfiStatus wfiStatus, void* userdata) {
  (void)wfiStatus;
  (void)userdata;
  globalCounter++;
  if (globalCounter>=globalIterations) {
    switch(wfiStatus.edge) {
      case INT_EDGE_FALLING:
        printgpio("finished falling\n");
        break;
      case INT_EDGE_RISING:
        printgpio("finished rising\n");
        break;
      default:
        printgpio("finished\n");
        break;
    }
    exit(wfiStatus.edge);
  } else {
    printgpioflush("I");
  }
}


int get_wfi_edge(const char* arg_cmd, const char* arg_mode, int exitcode) {
    if (strcasecmp (arg_mode, "rising")  == 0) {
    return INT_EDGE_RISING ;
  } else if (strcasecmp (arg_mode, "falling") == 0) {
    return INT_EDGE_FALLING ;
  } else if (strcasecmp (arg_mode, "both")    == 0) {
    return INT_EDGE_BOTH ;
  } else {
    fprintf (stderr, "%s: wfi: Invalid mode: %s. Should be rising, falling or both\n", arg_cmd, arg_mode) ;
    exit(exitcode);
  }
}


void doWfiInternal(const char* cmd, int pin, int mode, int interations, int timeoutSec, int debounce) {

  globalIterations = interations;
  globalCounter = 0;
  if (debounce>=0) {
    // V2 function
    if (wiringPiISR2(pin, mode, &wfi2, debounce, NULL) < 0) {
      fprintf (stderr, "%s: Unable to setup ISR2: %s\n", cmd, strerror (errno));
      exit(1);
    }
  } else {
    // classic function
    if (wiringPiISR(pin, mode, &wfi) < 0) {
      fprintf (stderr, "%s: Unable to setup ISR: %s\n", cmd, strerror (errno));
      exit(1);
    }
  }

  printgpio("wait for interrupt function call\n");
  for (int Sec=0; Sec<timeoutSec; ++Sec) {
    printgpioflush(".");
    delay(999);
  }
  printgpio("\nstopping wait for interrupt\n");
  wiringPiISRStop(pin);
}


void doWfi(int argc, char *argv [])
{
  int pin, mode, interations=1;
  int timeoutSec = 2147483647;

  if (argc != 4 && argc != 5 && argc != 6) {
    fprintf (stderr, "Usage: %s wfi pin mode [interations] [timeout sec.]\n", argv [0]) ;
    exit(1);
  }

  pin  = atoi (argv[2]) ;
  mode = get_wfi_edge(argv[1], argv[3], 1);
  if (argc>=5) {
    interations = atoi(argv[4]);
  }
  if (argc>=6) {
    timeoutSec = atoi(argv[5]);
  }

  doWfiInternal(argv[1], pin, mode, interations, timeoutSec, -1);
}


void doWfi2(int argc, char *argv [])
{
  int pin, mode, interations=1, debounce=50000;
  int timeoutSec = 2147483647;

  if (argc != 4 && argc != 5 && argc != 6 && argc != 7) {
    fprintf (stderr, "Usage: %s wfis pin mode [debounce period microsec.] [interations] [timeout sec.]\n", argv [0]);
    exit(-2);
  }

  pin  = atoi (argv[2]) ;
  mode = get_wfi_edge(argv[1], argv[3], -1);
  if (argc>=5) {
    debounce = atoi(argv[4]);
  }
  if (argc>=6) {
    interations = atoi(argv[5]);
  }
  if (argc>=7) {
    timeoutSec = atoi(argv[6]);
  }
  if (timeoutSec<0 || interations<0 || debounce<0) {
    fprintf (stderr, " invalid parameter\n");
    exit(-2);
  }

  doWfiInternal(argv[1], pin, mode, interations, timeoutSec, debounce);
  exit(-1); // timeout
}


/*
 * doEdge:  -> deprecated, removed
 *	gpio edge pin mode
 *	Easy access to changing the edge trigger on a GPIO pin
 *	This uses the /sys/class/gpio device interface.
 *********************************************************************************
 */

/*
 * doUnexport: -> deprecated, removed
 *	gpio unexport pin
 *	This uses the /sys/class/gpio device interface.
 *********************************************************************************
 */

/*
 * doUnexportAll: -> deprecated, removed
 *	gpio unexportall
 *	Un-Export all the GPIO pins.
 *	This uses the /sys/class/gpio device interface.
 *********************************************************************************
 */


/*
 * doReset:
 *	Reset the GPIO pins - as much as we can do
 *********************************************************************************
 */

static void doReset (UNU char *progName)
{
  printf ("GPIO Reset is dangerous and has been removed from the gpio command.\n") ;
  printf (" - Please write a shell-script to reset the GPIO pins into the state\n") ;
  printf ("   that you need them in for your applications.\n") ;
}


/*
 * doMode:
 *	gpio mode pin mode ...
 *********************************************************************************
 */

void doMode (int argc, char *argv [])
{
  int pin ;
  char *mode ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s mode pin mode\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  mode = argv [3] ;

  if      (strcasecmp (mode, "in")      == 0) pinMode         (pin, INPUT) ;
  else if (strcasecmp (mode, "input")   == 0) pinMode         (pin, INPUT) ;
  else if (strcasecmp (mode, "out")     == 0) pinMode         (pin, OUTPUT) ;
  else if (strcasecmp (mode, "output")  == 0) pinMode         (pin, OUTPUT) ;
  else if (strcasecmp (mode, "pwm")     == 0) pinMode         (pin, PWM_OUTPUT) ;
  else if (strcasecmp (mode, "pwmTone") == 0) pinMode         (pin, PWM_TONE_OUTPUT) ;
  else if (strcasecmp (mode, "clock")   == 0) pinMode         (pin, GPIO_CLOCK) ;
  else if (strcasecmp (mode, "up")      == 0) pullUpDnControl (pin, PUD_UP) ;
  else if (strcasecmp (mode, "down")    == 0) pullUpDnControl (pin, PUD_DOWN) ;
  else if (strcasecmp (mode, "tri")     == 0) pullUpDnControl (pin, PUD_OFF) ;
  else if (strcasecmp (mode, "off")     == 0) pullUpDnControl (pin, PUD_OFF) ;
  else if (strcasecmp (mode, "alt0")    == 0) pinModeAlt (pin, 0b100) ;
  else if (strcasecmp (mode, "alt1")    == 0) pinModeAlt (pin, 0b101) ;
  else if (strcasecmp (mode, "alt2")    == 0) pinModeAlt (pin, 0b110) ;
  else if (strcasecmp (mode, "alt3")    == 0) pinModeAlt (pin, 0b111) ;
  else if (strcasecmp (mode, "alt4")    == 0) pinModeAlt (pin, 0b011) ;
  else if (strcasecmp (mode, "alt5")    == 0) pinModeAlt (pin, 0b010) ;
  else
  {
    fprintf (stderr, "%s: Invalid mode: %s. Should be in/out/pwm/clock/up/down/tri\n", argv [1], mode) ;
    exit (1) ;
  }
}


/*
 * doPadDrive:
 *	gpio drive group value
 *********************************************************************************
 */

static void doPadDrivePin (int argc, char *argv [])
{

  if (argc != 4) {
    fprintf (stderr, "Usage: %s drivepin pin value\n", argv [0]) ;
    exit (1) ;
  }

  int pin = atoi (argv [2]) ;
  int val = atoi (argv [3]) ;

  if ((pin < 0) || (pin > 27)) {
    fprintf (stderr, "%s: drive pin not 0-27: %d\n", argv [0], pin) ;
    exit (1) ;
  }

  if ((val < 0) || (val > 3)) {
    fprintf (stderr, "%s: drive value not 0-3: %d\n", argv [0], val) ;
    exit (1) ;
  }

  setPadDrivePin (pin, val) ;
}


static void doPadDrive (int argc, char *argv [])
{
  int group, val ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s drive group value\n", argv [0]) ;
    exit (1) ;
  }

  group = atoi (argv [2]) ;
  val   = atoi (argv [3]) ;

  if ((group < -1) || (group > 2))  //-1 hidden feature for read and print values
  {
    fprintf (stderr, "%s: drive group not 0, 1 or 2: %d\n", argv [0], group) ;
    exit (1) ;
  }

  if ((val < 0) || (val > 7))
  {
    fprintf (stderr, "%s: drive value not 0-7: %d\n", argv [0], val) ;
    exit (1) ;
  }

  setPadDrive (group, val) ;
}


/*
 * doUsbP:
 *	Control USB Power - High (1.2A) or Low (600mA)
 *	gpio usbp high/low
 *********************************************************************************
 */

static void doUsbP (int argc, char *argv [])
{
  int model, rev, mem, maker, overVolted ;

  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s usbp high|low\n", argv [0]) ;
    exit (1) ;
  }

// Make sure we're on a B+

  piBoardId (&model, &rev, &mem, &maker, &overVolted) ;

  if (!((model == PI_MODEL_BP) || (model == PI_MODEL_2)))
  {
    fprintf (stderr, "USB power contol is applicable to B+ and v2 boards only.\n") ;
    exit (1) ;
  }
    
// Make sure we start in BCM_GPIO mode

  wiringPiSetupGpio () ;

  if ((strcasecmp (argv [2], "high") == 0) || (strcasecmp (argv [2], "hi") == 0))
  {
    digitalWrite (PI_USB_POWER_CONTROL, 1) ;
    pinMode (PI_USB_POWER_CONTROL, OUTPUT) ;
    printf ("Switched to HIGH current USB (1.2A)\n") ;
    return ;
  }

  if ((strcasecmp (argv [2], "low") == 0) || (strcasecmp (argv [2], "lo") == 0))
  {
    digitalWrite (PI_USB_POWER_CONTROL, 0) ;
    pinMode (PI_USB_POWER_CONTROL, OUTPUT) ;
    printf ("Switched to LOW current USB (600mA)\n") ;
    return ;
  }

  fprintf (stderr, "Usage: %s usbp high|low\n", argv [0]) ;
  exit (1) ;
}


/*
 * doGbw:
 *	gpio gbw channel value
 *	Gertboard Write - To the Analog output
 *********************************************************************************
 */

static void doGbw (int argc, char *argv [])
{
  int channel, value ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s gbw <channel> <value>\n", argv [0]) ;
    exit (1) ;
  }

  channel = atoi (argv [2]) ;
  value   = atoi (argv [3]) ;

  if ((channel < 0) || (channel > 1))
  {
    fprintf (stderr, "%s: gbw: Channel number must be 0 or 1\n", argv [0]) ;
    exit (1) ;
  }

  if ((value < 0) || (value > 255))
  {
    fprintf (stderr, "%s: gbw: Value must be from 0 to 255\n", argv [0]) ;
    exit (1) ;
  }

  if (gertboardAnalogSetup (64) < 0)
  {
    fprintf (stderr, "Unable to initialise the Gertboard SPI interface: %s\n", strerror (errno)) ;
    exit (1) ;
  }

  analogWrite (64 + channel, value) ;
}


/*
 * doGbr:
 *	gpio gbr channel
 *	From the analog input
 *********************************************************************************
 */

static void doGbr (int argc, char *argv [])
{
  int channel ;

  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s gbr <channel>\n", argv [0]) ;
    exit (1) ;
  }

  channel = atoi (argv [2]) ;

  if ((channel < 0) || (channel > 1))
  {
    fprintf (stderr, "%s: gbr: Channel number must be 0 or 1\n", argv [0]) ;
    exit (1) ;
  }

  if (gertboardAnalogSetup (64) < 0)
  {
    fprintf (stderr, "Unable to initialise the Gertboard SPI interface: %s\n", strerror (errno)) ;
    exit (1) ;
  }

  printf ("%d\n", analogRead (64 + channel)) ;
}


/*
 * doWrite:
 *	gpio write pin value
 *********************************************************************************
 */

static void doWrite (int argc, char *argv [])
{
  int pin, val ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s write pin value\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  if      ((strcasecmp (argv [3], "up") == 0) || (strcasecmp (argv [3], "on") == 0))
    val = 1 ;
  else if ((strcasecmp (argv [3], "down") == 0) || (strcasecmp (argv [3], "off") == 0))
    val = 0 ;
  else
    val = atoi (argv [3]) ;

  if (val == 0)
    digitalWrite (pin, LOW) ;
  else
    digitalWrite (pin, HIGH) ;
}


/*
 * doAwriterite:
 *	gpio awrite pin value
 *********************************************************************************
 */

static void doAwrite (int argc, char *argv [])
{
  int pin, val ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s awrite pin value\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  val = atoi (argv [3]) ;

  analogWrite (pin, val) ;
}


/*
 * doWriteByte:
 *	gpio wb value
 *********************************************************************************
 */

static void doWriteByte (int argc, char *argv [])
{
  int val ;

  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s wb value\n", argv [0]) ;
    exit (1) ;
  }

  val = (int)strtol (argv [2], NULL, 0) ;

  digitalWriteByte (val) ;
}


/*
 * doReadByte:
 *	gpio rbx|rbd value
 *********************************************************************************
 */

static void doReadByte (int argc, char *argv [], int printHex)
{
  int val ;

  if (argc != 2)
  {
    fprintf (stderr, "Usage: %s rbx|rbd\n", argv [0]) ;
    exit (1) ;
  }

  val = digitalReadByte () ;
  if (printHex)
    printf ("%02X\n", val) ;
  else
    printf ("%d\n", val) ;
}


/*
 * doRead:
 *	Read a pin and return the value
 *********************************************************************************
 */

void doRead (int argc, char *argv []) 
{
  int pin, val ;

  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s read pin\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;
  val = digitalRead (pin) ;

  printf ("%s\n", val == 0 ? "0" : "1") ;
}


/*
 * doAread:
 *	Read an analog pin and return the value
 *********************************************************************************
 */

void doAread (int argc, char *argv []) 
{
  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s aread pin\n", argv [0]) ;
    exit (1) ;
  }

  printf ("%d\n", analogRead (atoi (argv [2]))) ;
}


/*
 * doToggle:
 *	Toggle an IO pin
 *********************************************************************************
 */

void doToggle (int argc, char *argv [])
{
  int pin ;

  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s toggle pin\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  digitalWrite (pin, !digitalRead (pin)) ;
}


/*
 * doBlink:
 *	Blink an IO pin
 *********************************************************************************
 */

void doBlink (int argc, char *argv [])
{
  int pin ;

  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s blink pin\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  pinMode (pin, OUTPUT) ;
  for (;;)
  {
    digitalWrite (pin, !digitalRead (pin)) ;
    delay (500) ;
  }

}


/*
 * doPwmTone:
 *	Output a tone in a PWM pin
 *********************************************************************************
 */

void doPwmTone (int argc, char *argv [])
{
  int pin, freq ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s pwmTone <pin> <freq>\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;
  freq = atoi (argv [3]) ;

  pwmToneWrite (pin, freq) ;
}


/*
 * doClock:
 *	Output a clock on a pin
 *********************************************************************************
 */

void doClock (int argc, char *argv [])
{
  int pin, freq ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s clock <pin> <freq>\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  freq = atoi (argv [3]) ;

  gpioClockSet (pin, freq) ;
}


/*
 * doPwm:
 *	Output a PWM value on a pin
 *********************************************************************************
 */

void doPwm (int argc, char *argv [])
{
  int pin, val ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s pwm <pin> <value>\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  val = atoi (argv [3]) ;

  pwmWrite (pin, val) ;
}


/*
 * doPwmMode: doPwmRange: doPwmClock:
 *	Change the PWM mode, range and clock divider values
 *********************************************************************************
 */

static void doPwmMode (int mode)
{
  pwmSetMode (mode) ;
}

static void doPwmRange (int argc, char *argv [])
{
  unsigned int range ;

  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s pwmr <range>\n", argv [0]) ;
    exit (1) ;
  }

  range = (unsigned int)strtoul (argv [2], NULL, 10) ;

  if (range == 0)
  {
    fprintf (stderr, "%s: range must be > 0\n", argv [0]) ;
    exit (1) ;
  }

  pwmSetRange (range) ;
}

static void doPwmClock (int argc, char *argv [])
{
  unsigned int clock ;

  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s pwmc <clock>\n", argv [0]) ;
    exit (1) ;
  }

  clock = (unsigned int)strtoul (argv [2], NULL, 10) ;

  if ((clock < 1) || (clock > 4095))
  {
    fprintf (stderr, "%s: pwm clock must be between 1 and 4095\n", argv [0]) ;
    exit (1) ;
  }

  pwmSetClock (clock) ;
}


/*
 * doVersion:
 *	Handle the ever more complicated version command and print out
 *	some usefull information.
 *********************************************************************************
 */

static void doVersion (char *argv [])
{
  int model, rev, mem, maker, warranty ;
  struct stat statBuf ;
  char name [80] ;
  FILE *fd ;

  int vMaj, vMin ;

  wiringPiVersion (&vMaj, &vMin) ;
  printf ("gpio version: %d.%d\n", vMaj, vMin) ;
  printf ("Copyright (c) 2012-2025 Gordon Henderson and contributors\n") ;
  printf ("This is free software with ABSOLUTELY NO WARRANTY.\n") ;
  printf ("For details type: %s -warranty\n", argv [0]) ;
  printf ("\n") ;
  piBoardId (&model, &rev, &mem, &maker, &warranty) ;

  printf ("Hardware details:\n") ;
  printf ("  Type: %s, Revision: %s, Memory: %dMB, Maker: %s %s\n", 
      piModelNames [model], piRevisionNames [rev], piMemorySize [mem], piMakerNames [maker], warranty ? "[Out of Warranty]" : "") ;

// Check for device tree
  printf ("\nSystem details:\n") ;
  if (stat ("/proc/device-tree", &statBuf) == 0) {	// We're on a devtree system ...
    printf ("  * Device tree present.\n") ;
  }
  if (stat ("/proc/device-tree/model", &statBuf) == 0)	// Output Kernel idea of board type
  {
    if ((fd = fopen ("/proc/device-tree/model", "r")) != NULL)
    {
      if (fgets(name, sizeof(name), fd) == NULL) {
        // Handle error or end of file condition
        perror("Error reading /proc/device-tree/model");
      }
      fclose (fd) ;
      printf ("      Model: %s\n", name) ;
    }
  }

  int bGlobalAccess = wiringPiGlobalMemoryAccess();		// User level GPIO is GO
  switch(bGlobalAccess) {
    case 0:
        printf ("  * Does not support basic user-level GPIO access via memory.\n") ;
        break;
    case 1:
        printf ("  * Supports basic user-level GPIO access via /dev/mem.\n") ;
        break;
    case 2:
        printf ("  * Supports full  user-level GPIO access via memory.\n") ;
        break;
  }
  if (wiringPiUserLevelAccess()) {
        printf ("  * Supports basic user-level GPIO access via /dev/gpiomem.\n") ;
  } else  {
        printf ("  * Does not support basic user-level GPIO access via /dev/gpiomem.\n") ;
    if(0==bGlobalAccess) {
        printf ("  * root or sudo may be required for direct GPIO access.\n") ;
    }
  }
  if (wiringPiGpioDeviceGetFd()>0) {
    printf ("  * Supports basic user-level GPIO access via /dev/gpiochip (slow).\n") ;
  }

}

static void doIs40Pin ()
{
  exit(piBoard40Pin() ? EXIT_SUCCESS : EXIT_FAILURE);
}

/*
 * main:
 *	Start here
 *********************************************************************************
 */

int main (int argc, char *argv [])
{
  int i ;

  if (getenv ("WIRINGPI_DEBUG") != NULL)
  {
    printf ("gpio: wiringPi debug mode enabled\n") ;
    wiringPiDebug = TRUE ;
  }
  if (getenv ("GPIO_DEBUG") != NULL)
  {
    printf ("gpio: gpio debug mode enabled\n") ;
    gpioDebug = TRUE ;
  }

  if (argc == 1)
  {
    fprintf (stderr,
"%s: At your service!\n"
"  Type: gpio -h for full details and\n"
"        gpio readall for a quick printout of your connector details\n", argv [0]) ;
    exit (EXIT_FAILURE) ;
  }

// Help

  if (strcasecmp (argv [1], "-h") == 0)
  {
    printf ("%s: %s\n", argv [0], usage) ;
    exit (EXIT_SUCCESS) ;
  }

// Version & Warranty
//	Wish I could remember why I have both -R and -V ...

  if ((strcmp (argv [1], "-R") == 0) || (strcmp (argv [1], "-V") == 0))
  {
    printf ("%d\n", piGpioLayout ()) ;
    exit (EXIT_SUCCESS) ;
  }

// Version and information

  if (strcmp (argv [1], "-v") == 0)
  {
    doVersion (argv) ;
    exit (EXIT_SUCCESS) ;
  }

  if (strcasecmp (argv [1], "-warranty") == 0)
  {
    printf ("gpio version: %s\n", VERSION) ;
    printf ("Copyright (c) 2012-2025 Gordon Henderson and contributors\n") ;
    printf ("\n") ;
    printf ("    This program is free software; you can redistribute it and/or modify\n") ;
    printf ("    it under the terms of the GNU Leser General Public License as published\n") ;
    printf ("    by the Free Software Foundation, either version 3 of the License, or\n") ;
    printf ("    (at your option) any later version.\n") ;
    printf ("\n") ;
    printf ("    This program is distributed in the hope that it will be useful,\n") ;
    printf ("    but WITHOUT ANY WARRANTY; without even the implied warranty of\n") ;
    printf ("    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n") ;
    printf ("    GNU Lesser General Public License for more details.\n") ;
    printf ("\n") ;
    printf ("    You should have received a copy of the GNU Lesser General Public License\n") ;
    printf ("    along with this program. If not, see <http://www.gnu.org/licenses/>.\n") ;
    printf ("\n") ;
    exit (EXIT_SUCCESS) ;
  }

  if (geteuid () != 0)
  {
    fprintf (stderr, "%s: Must be root to run. Program should be suid root. This is an error.\n", argv [0]) ;
    exit (EXIT_FAILURE) ;
  }

// Initial test for /sys/class/gpio operations:  --> deprecated, empty but still there

  if      (strcasecmp (argv [1], "exports"    ) == 0)	{ SYSFS_DEPRECATED(argv[0]);	return 0 ; }
  else if (strcasecmp (argv [1], "export"     ) == 0)	{ SYSFS_DEPRECATED(argv[0]);	return 0 ; }
  else if (strcasecmp (argv [1], "edge"       ) == 0)	{ SYSFS_DEPRECATED(argv[0]);	return 0 ; }
  else if (strcasecmp (argv [1], "unexport"   ) == 0)	{ SYSFS_DEPRECATED(argv[0]);	return 0 ; }
  else if (strcasecmp (argv [1], "unexportall") == 0)	{ SYSFS_DEPRECATED(argv[0]);	return 0 ; }

// Check for un-/load command:  --> deprecated, empty but still there

  if (strcasecmp (argv [1], "load"   ) == 0)	{ LOAD_DEPRECATED(argv[0]) ; return 0 ; }
  if (strcasecmp (argv [1], "unload" ) == 0)	{ LOAD_DEPRECATED(argv[0]) ; return 0 ; }

// Check for usb power command

  if (strcasecmp (argv [1], "usbp"   ) == 0)	{ doUsbP   (argc, argv) ; return 0 ; }

// Gertboard commands

  if (strcasecmp (argv [1], "gbr" ) == 0)	{ doGbr (argc, argv) ; return 0 ; }
  if (strcasecmp (argv [1], "gbw" ) == 0)	{ doGbw (argc, argv) ; return 0 ; }

// Check for allreadall command, force Gpio mode

  if (strcasecmp (argv [1], "allreadall") == 0)
  {
    wiringPiSetupGpio () ;
    doAllReadall      () ;
    return 0 ;
  }

// Check for -g argument

  if (strcasecmp (argv [1], "-g") == 0)
  {
    wiringPiSetupGpio () ;

    for (i = 2 ; i < argc ; ++i)
      argv [i - 1] = argv [i] ;
    --argc ;
    wpMode = WPI_MODE_GPIO ;
  }

// Check for -1 argument

  else if (strcasecmp (argv [1], "-1") == 0)
  {
    wiringPiSetupPhys () ;

    for (i = 2 ; i < argc ; ++i)
      argv [i - 1] = argv [i] ;
    --argc ;
    wpMode = WPI_MODE_PHYS ;
  }

// Check for -p argument for PiFace

  else if (strcasecmp (argv [1], "-p") == 0)
  {
    piFaceSetup (200) ;

    for (i = 2 ; i < argc ; ++i)
      argv [i - 1] = argv [i] ;
    --argc ;
    wpMode = WPI_MODE_PIFACE ;
  }

// Check for -z argument so we don't actually initialise wiringPi

  else if (strcasecmp (argv [1], "-z") == 0)
  {
    for (i = 2 ; i < argc ; ++i)
      argv [i - 1] = argv [i] ;
    --argc ;
    wpMode = WPI_MODE_UNINITIALISED ;
  }

// Default to wiringPi mode

  else
  {
    wiringPiSetup () ;
    wpMode = WPI_MODE_PINS ;
  }

// Check for -x argument to load in a new extension
//	-x extension:base:args
//	Can load many modules, but unless daemon mode we can only send one
//	command at a time.

  while (strcasecmp (argv [1], "-x") == 0)
  {
    if (argc < 3)
    {
      fprintf (stderr, "%s: -x missing extension command.\n", argv [0]) ;
      exit (EXIT_FAILURE) ;
    }

    if (!loadWPiExtension (argv [0], argv [2], TRUE))
    {
      fprintf (stderr, "%s: Extension load failed: %s\n", argv [0], strerror (errno)) ;
      exit (EXIT_FAILURE) ;
    }

// Shift args down by 2

    for (i = 3 ; i < argc ; ++i)
      argv [i - 2] = argv [i] ;
    argc -= 2 ;
  }

  if (argc <= 1)
  {
    fprintf (stderr, "%s: no command given\n", argv [0]) ;
    exit (EXIT_FAILURE) ;
  }

// Core wiringPi functions

  if      (strcasecmp (argv [1], "mode"   ) == 0) doMode      (argc, argv) ;
  else if (strcasecmp (argv [1], "read"   ) == 0) doRead      (argc, argv) ;
  else if (strcasecmp (argv [1], "write"  ) == 0) doWrite     (argc, argv) ;
  else if (strcasecmp (argv [1], "pwm"    ) == 0) doPwm       (argc, argv) ;
  else if (strcasecmp (argv [1], "awrite" ) == 0) doAwrite    (argc, argv) ;
  else if (strcasecmp (argv [1], "aread"  ) == 0) doAread     (argc, argv) ;

// GPIO Nicies

  else if (strcasecmp (argv [1], "toggle" ) == 0) doToggle    (argc, argv) ;
  else if (strcasecmp (argv [1], "blink"  ) == 0) doBlink     (argc, argv) ;

// Pi Specifics

  else if (strcasecmp (argv [1], "pwm-bal"  ) == 0) doPwmMode    (PWM_MODE_BAL) ;
  else if (strcasecmp (argv [1], "pwm-ms"   ) == 0) doPwmMode    (PWM_MODE_MS) ;
  else if (strcasecmp (argv [1], "pwmr"     ) == 0) doPwmRange   (argc, argv) ;
  else if (strcasecmp (argv [1], "pwmc"     ) == 0) doPwmClock   (argc, argv) ;
  else if (strcasecmp (argv [1], "pwmTone"  ) == 0) doPwmTone    (argc, argv) ;
  else if (strcasecmp (argv [1], "drive"    ) == 0) doPadDrive   (argc, argv) ;
  else if (strcasecmp (argv [1], "drivepin" ) == 0) doPadDrivePin(argc, argv) ;
  else if (strcasecmp (argv [1], "readall"  ) == 0) doReadall    () ;
  else if (strcasecmp (argv [1], "nreadall" ) == 0) doReadall    () ;
  else if (strcasecmp (argv [1], "pins"     ) == 0) doReadall    () ;
  else if (strcasecmp (argv [1], "qmode"    ) == 0) doQmode      (argc, argv) ;
  else if (strcasecmp (argv [1], "i2cdetect") == 0) doI2Cdetect  (argv [0]) ;
  else if (strcasecmp (argv [1], "i2cd"     ) == 0) doI2Cdetect  (argv [0]) ;
  else if (strcasecmp (argv [1], "reset"    ) == 0) doReset      (argv [0]) ;
  else if (strcasecmp (argv [1], "wb"       ) == 0) doWriteByte  (argc, argv) ;
  else if (strcasecmp (argv [1], "rbx"      ) == 0) doReadByte   (argc, argv, TRUE) ;
  else if (strcasecmp (argv [1], "rbd"      ) == 0) doReadByte   (argc, argv, FALSE) ;
  else if (strcasecmp (argv [1], "clock"    ) == 0) doClock      (argc, argv) ;
  else if (strcasecmp (argv [1], "wfis"     ) == 0) doWfi2       (argc, argv) ;
  else if (strcasecmp (argv [1], "wfi"      ) == 0) doWfi        (argc, argv) ;
  else if (strcasecmp (argv [1], "is40pin"  ) == 0) doIs40Pin    () ;
  else
  {
    fprintf (stderr, "%s: Unknown command: %s.\n", argv [0], argv [1]) ;
    exit (EXIT_FAILURE) ;
  }

  return 0 ;
}
