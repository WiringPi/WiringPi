# Documentation WiringPi-library

The WiringPi-library enables access to the GPIO pins of the Raspberry Pi. In this documentation you will find information about the available functions and their use.
Since Version 3, extensions to the interface have been made again. In the case of new implementations, you should rely on the current / new functions.  
The old [GPIO Sysfs Interface for Userspace](https://www.kernel.org/doc/Documentation/gpio/sysfs.txt) is no longer supported.

**Attention:**  

This documentation is still in progress and therefore incomplete.
The content of this documentation was created with care and to the best of our knowledge. However, the authors do not guarantee the correctness, completeness and topicality of the information provided.The content of the documentation is used at your own risk.

No liability is assumed for material or intangible damage caused by the use or non-use of the information provided or by the use of incorrect or incomplete information.

## Installation

The WiringPi Library is not directly available in Raspberry Pi OS, so it must be installed manually. Either download a Debian package or create it manually.

**Create Debian package:**  

```bash
sudo apt install git
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build debian
mv debian-template/wiringpi_3.16_arm64.deb .
```

**Install Debian package:**  

```bash
sudo apt install ./wiringpi_3.16_arm64.deb
```

**Uninstall Debian package:**  

```bash
sudo apt purge wiringpi
```

## PIN Numbering and Raspberry Pi Models

GPIOs: [https://pinout.xyz/pinout/wiringpi](https://pinout.xyz/pinout/wiringpi)

**Raspberry Pi Models with 40-Pin GPIO J8 Header:**  

 | BCM | WPI |   Name  | Physical  | Name    | WPI | BCM |
 |-----|-----|---------|:---------:|---------|-----|-----|
 |     |     |     3V3 |  1  I  2  | 5V      |     |     |
 |   2 |   8 |   SDA.1 |  3  I  4  | 5V      |     |     |
 |   3 |   9 |   SCL.1 |  5  I  6  | GND     |     |     |
 |   4 |   7 | GPIO. 7 |  7  I  8  | TxD     | 15  | 14  |
 |     |     |     GND |  9  I  10 | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 | 11  I  12 | GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 | 13  I  14 | GND     |     |     |
 |  22 |   3 | GPIO. 3 | 15  I  16 | GPIO. 4 | 4   | 23  |
 |     |     |     3V3 | 17  I  18 | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI | 19  I  20 | GND     |     |     |
 |   9 |  13 |    MISO | 21  I  22 | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK | 23  I  24 | CE0     | 10  | 8   |
 |     |     |     GND | 25  I  26 | CE1     | 11  | 7   |
 |   0 |  30 |   SDA.0 | 27  I  28 | SCL.0   | 31  | 1   |
 |   5 |  21 | GPIO.21 | 29  I  30 | GND     |     |     |
 |   6 |  22 | GPIO.22 | 31  I  32 | GPIO.26 | 26  | 12  |
 |  13 |  23 | GPIO.23 | 33  I  34 | GND     |     |     |
 |  19 |  24 | GPIO.24 | 35  I  36 | GPIO.27 | 27  | 16  |
 |  26 |  25 | GPIO.25 | 37  I  38 | GPIO.28 | 28  | 20  |
 |     |     |     GND | 39  I  40 | GPIO.29 | 29  | 21  |

**Raspberry Pi 1B Rev. 2 with 26-Pin GPIO P1 Header:**  

 | BCM | WPI |   Name  | Physical |  Name    | WPI | BCM |
 |-----|-----|---------|:--------:|----------|-----|-----|
 |     |     |     3V3 |  1 I 2   |  5V      |     |     |
 |   2 |   8 |   SDA.1 |  3 I 4   |  5V      |     |     |
 |   3 |   9 |   SCL.1 |  5 I 6   |  GND     |     |     |
 |   4 |   7 | GPIO. 7 |  7 I 8   |  TxD     | 15  | 14  |
 |     |     |     GND |  9 I 10  |  RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 | 11 I 12  |  GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 | 13 I 14  |  GND     |     |     |
 |  22 |   3 | GPIO. 3 | 15 I 16  |  GPIO. 4 | 4   | 23  |
 |     |     |     3V3 | 17 I 18  |  GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI | 19 I 20  |  GND     |     |     |
 |   9 |  13 |    MISO | 21 I 22  |  GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK | 23 I 24  |  CE0     | 10  | 8   |
 |     |     |     GND | 25 I 26  |  CE1     | 11  | 7   |

**Raspberry Pi 1B Rev. 1 with 26-Pin GPIO P1 Header:**  

 | BCM | WPI |   Name  | Physical |  Name   | WPI | BCM |
 |-----|-----|---------|:--------:|---------|-----|-----|
 |     |     |     3V3 |  1 I 2   | 5V      |     |     |
 |   0 |   8 |   SDA.0 |  3 I 4   | 5V      |     |     |
 |   1 |   9 |   SCL.0 |  5 I 6   | GND     |     |     |
 |   4 |   7 | GPIO. 7 |  7 I 8   | TxD     | 15  | 14  |
 |     |     |     GND |  9 I 10  | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 | 11 I 12  | GPIO. 1 | 1   | 18  |
 |  21 |   2 | GPIO. 2 | 13 I 14  | GND     |     |     |
 |  22 |   3 | GPIO. 3 | 15 I 16  | GPIO. 4 | 4   | 23  |
 |     |     |     3V3 | 17 I 18  | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI | 19 I 20  | GND     |     |     |
 |   9 |  13 |    MISO | 21 I 22  | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK | 23 I 24  | CE0     | 10  | 8   |
 |     |     |     GND | 25 I 26  | CE1     | 11  | 7   |

**References**  

Note the different pin numbers and the I2C0 at Raspberry Pi 1B Rev. 1!

## Initialization

At the beginning, the WiringPi Library must be initialized.
To do this, one of the following functions must be called up:

Outdated functions (no longer use):

``wiringPiSetup`` uses WiringPi numbering (WPI) of the GPIOs and accesses the GPIO register directly.  
``wiringPiSetupGpio`` uses BCM numbering of the GPIOs and accesses the GPIO registers directly.  
``wiringPiSetupPhys`` uses physical PIN numbering of the GPIOs and accesses the GPIO register directly.  
``wiringPiSetupSys`` uses BCM numbering and calls the new function ``wiringPiSetupGpioDevice`` from version 3.4 to ensure compatibility with new core. In version 2, the virtual file system/SYS/Class/GPIO was used. However, the GPIOs exports had to take place externally before the initialization! The function is outdated and should not be used!

**Since Version 3.4:**  

``wiringPiSetupPinType`` decides whether WiringPi, BCM or physical pin numbering is used, based on the parameter pinType. So it combines the first 3 setup functions together.  
``wiringPiSetupGpioDevice`` is the successor to the ``wiringPiSetupSys`` function and now uses "GPIO Character Device Userspace API" in version 2 (WiringPi version 3.16 or higher). More information can be found at [docs.kernel.org/driver-api/gpio/driver.html](https://docs.kernel.org/driver-api/gpio/driver.html) on the parameter pintype, it is again decided which pin numbering is used.  
In this variant, there is no direct access to the GPIO memory (DMA) but rather through a kernel interface that is available with user permissions. The disadvantage is the limited functionality and low performance.


### wiringPiSetup V2 (outdated)

Inializating WiringPi in a classic way.

**Notice:** This function is deprecated and should not be used in modern implementations.

```C
int wiringPiSetupGpio(void);
```

``Return Value``:  Error status

> 0 ... No Error  

**Example:**  

```C
wiringPiSetupGpio();
```

### wiringPiSetup V3

Initializing WiringPi.  

```C
int wiringPiSetupPinType(enum WPIPinType pinType);
```  

``pinType``: Type of PIN numbering...

- `WPI_PIN_BCM` ... BCM-Numbering  
- `WPI_PIN_WPI` ... WiringPi-Numbering  
- `WPI_PIN_PHYS` ... Physical Numbering  

``Return Value``:  Error status  

> 0 ... No Error  
> -1 ... Invalid Parameter Error

**Example:**  

```C
wiringPiSetupPinType(WPI_PIN_BCM);
```

## Basic Functions

### pinMode

Changes the mode of a GPIO pins.

```C
void pinMode(int pin, int mode);
```  

``Pin``: The desired PIN (BCM, Wiringpi or PIN number).  
``Mode``: The desired pin mode...

- `INPUT` ... Input
- `OUTPUT` ... Output
- `PWM_OUTPUT` ... PWM output (frequency and pulse break ratio can be configured)
- `PWM_MS_OUTPUT` ... PWM output with MS (Mark/Space) (since version 3)
- `PWM_BAL_OUTPUT` ... PWM output with mode balanced) (since version 3)
- `GPIO_CLOCK` ... Frequency output
- `PM_OFF` ... Release

**Example:**  

```C
pinMode(17, OUTPUT);
```

**Notice:**  

- `PM_OFF` resets the GPIO (Input) and releases it. PWM is stopped.
- Raspberry Pi 5 does not support the PWM Bal (Balanced) mode. The MS mode is activated at `PWM_OUTPUT`.
- `GPIO_CLOCK` is currently not yet supported in Raspberry Pi 5 (RP1).

**PWM Exit**  

`PWM_OUTPUT` Activates the specified PWM output with the settings:

- Mode: BAL-Balanced (Pi0-4), MS-Mark/Space (Pi 5)
- Range: 1024  
- Divider: 32  

In order to make sure that the output starts without an active frequency, you should execute ``pwmWrite(PWM_GPIO, 0);`` before activating.
After that, the corresponding clock and range values ​​can be adapted, without a frequency already being output unintentionally.

### digitalWrite

Writes the value `HIGH` or `LOW` (1 or 0) to the given pin which must have been previously set as an output.

```C
void digitalWrite(int pin, int value);
```

``pin``: The desired Pin (BCM-, Wiringpi- or PIN number).  
``value``: The logical value...

- `HIGH` ... Value 1 (electrical ~3.3 V)
- `LOW` ... Value 0 (electrical ~0 V / GND)

**Example:**  

```C
pinMode(17, OUTPUT);

digitalWrite(17, HIGH);
```

**Note**  

WiringPi treats any non-zero number as `HIGH`, however 0 is the only representation of `LOW`.

### pullUpDnControl

Changes the internal pull-up / pull-down resistance.

```C
void pullUpDnControl (int pin, int pud);
```  

``pin``: The desired Pin (BCM-, WiringPi-, or Pin-number).  
``pud``: The resistance type...

- `PUD_OFF` ... No resistance
- `PUD_UP` ... Pull-Up to pull to 3.3v (~50 kOhm resistance)
- `PUD_DOWN` ... Pull-Down to pull to ground (~50 kOhm resistance)

**Example:**  

```C
pullUpDnControl(17, PUD_DOWN);
```

### digitalRead

Reads the value of the given GPIO-Pin. It will be `HIGH` or `LOW` (1 or 0) depending on the logic level at the pin.

```C
int digitalRead(int pin);
```

``pin``: The desired Pin (BCM-, WiringPi-, or Pin-number).  
``Return Value``: The logical value.  

> `HIGH` ... Value 1  
> `LOW` ... Value 0

**Example:**  

```C
pinMode(17, INPUT);
pullUpDnControl(17, PUD_DOWN);

int value = digitalRead(17);

if (value == HIGH) 
{
    // Your Code
}
```

## Interrupts

### wiringPiISR

Registers an Interrupt Service Routine (ISR) / function that is executed on edge detection.  
In this classic version, no parameters are passed to the ISR.

```C
int wiringPiISR(int pin, int mode, void (*function)(void));
```

``pin``: The desired pin (BCM-, WiringPi-, or Pin-number).  
``mode``: Triggering edge mode

- `INT_EDGE_RISING` ... Rising edge
- `INT_EDGE_FALLING` ... Falling edge
- `INT_EDGE_BOTH` ... Rising and falling edge

``*function``: Function pointer for ISR  
``Return Value``:

> 0 ... Successful
<!-- > <>0 ... Error not implemented at the moment. -->

For example see [wiringPiISRStop](#wiringPiISRStop).


### wiringPiISR2

Registers an Interrupt Service Routine (ISR) / function that is executed on edge detection.  
Extended parameters are passed to the ISR.

>>>
```C
int wiringPiISR2(int pin, int edgeMode, void (*function)(struct WPIWfiStatus wfiStatus, void* userdata), unsigned long debounce_period_us, void* userdata);
```

``pin``: The desired pin (BCM-, WiringPi-, or Pin-number).  
``edgeMode``: Triggering edge mode

- `INT_EDGE_RISING` ... Rising edge
- `INT_EDGE_FALLING` ... Falling edge
- `INT_EDGE_BOTH` ... Rising and falling edge

``*function``: Function pointer for ISR with the parameter struct WPIWfiStatus and a pointer.   
```C
struct WPIWfiStatus {
    int statusOK;               // -1: error (return of 'poll' command), 0: timeout, 1: irq processed, next data values are valid if needed
    unsigned int pinBCM;        // gpio as BCM pin
    int edge;                   // INT_EDGE_FALLING or INT_EDGE_RISING
    long long int timeStamp_us; // time stamp in microseconds
};
```

``debounce_period_us``: Debounce time in microseconds, 0 disables debouncing.  
``userdata``: Pointer that is passed when calling the ISR.

``Return Value``:
 > 0 ... Successful

 For example see [waitForInterrupt2](#waitForInterrupt2).


### wiringPiISRStop

Deregisters the Interrupt Service Routine (ISR) on a pin.

```C
int wiringPiISRStop (int pin);
```

``pin``: The desired Pin (BCM-, WiringPi-, or Pin-number).  
``Return Value``:

> 0 ... Successful

<!-- > <>0 ... Error not implemented at the moment. -->

**Example:**  

```C
static volatile int edgeCounter;

static void isr(void) { 
  edgeCounter++;
}

int main (void) {
    wiringPiSetupPinType(WPI_PIN_BCM);
    edgeCounter = 0;

    wiringPiISR(17, INT_EDGE_RISING, &isr);

    Sleep(1000);
    printf("%d rising edges\n", edgeCounter);

    wiringPiISRStop(17);
}
```

### waitForInterrupt

The function is no longer available, only ``waitForInterrupt2``!


### waitForInterrupt2

Waits for a call to the Interrupt Service Routine (ISR) with a timeout and debounce time in microseconds. Blocks the program until the triggering edge occurs or the timeout expires.

```C
struct WPIWfiStatus wfiStatus waitForInterrupt2(int pin, int edgeMode, int ms, unsigned long debounce_period_us)
```

``pin``: The desired Pin (BCM-, WiringPi-, or Pin-number).  
``ms``: Timeout in milliseconds.
 - \-1 ... Wait without timeout
 - 0 ... No wait
 - 1-n ... Waits for a maximum of n milliseconds
 
``debounce_period_us``: Debounce time in microseconds, 0 disables debouncing.  

``Return Value``:
```C
struct WPIWfiStatus {
    int statusOK;               // -1: error (return of 'poll' command), 0: timeout, 1: irq processed, next data values are valid if needed
    unsigned int pinBCM;        // gpio as BCM pin
    int edge;                   // INT_EDGE_FALLING or INT_EDGE_RISING
    long long int timeStamp_us; // time stamp in microseconds
};
```

**Example:**

```C
/*
 * isr_debounce.c:
 *	Wait for Interrupt test program  WiringPi >=3.16 - ISR2 method
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <time.h>

#define BOUNCETIME 3000 // microseconds
#define BOUNCETIME_WFI  300
#define TIMEOUT    10000
//*************************************
// BCM pins
// IRQpin : setup as input with internal pullup. Connected with push button to GND with 1K resistor in series.
// OUTpin : connected to a LED with 470 Ohm resistor in series to GND. Toggles LED with every push button pressed.
//*************************************
#define IRQpin     16
#define OUTpin     12

int toggle = 0;

static void wfi(struct WPIWfiStatus wfiStatus, void* userdata) {
//  struct timeval now;
  long long int timenow, diff;
  struct timespec curr;
  char *edgeType;

  if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1) {
        printf("clock_gettime error");
        return;
  }

  timenow = curr.tv_sec * 1000000LL + curr.tv_nsec/1000L; // convert to microseconds
  diff = timenow - wfiStatus.timeStamp_us;
  if (wfiStatus.edge == INT_EDGE_RISING)
      edgeType = "rising";
  else if (wfiStatus.edge == INT_EDGE_FALLING)
      edgeType = "falling";
  else
      edgeType = "none";
  printf("gpio BCM = %d, IRQ edge = %s, timestamp = %lld microseconds, timenow = %lld, diff = %lld\n", wfiStatus.gpioPin, edgeType, wfiStatus.timeStamp_us, timenow, diff);
  if (toggle == 0) {
    digitalWrite (OUTpin, HIGH);
    toggle = 1;
  }
  else {
    digitalWrite (OUTpin, LOW);
    toggle = 0;
  }
}


int main (void)
{
  int major, minor;
  wiringPiVersion(&major, &minor);
  printf("\nISR debounce test (WiringPi %d.%d)\n\n", major, minor);

  wiringPiSetupGpio();
  pinMode(IRQpin, INPUT);
  // pull up/down mode (PUD_OFF, PUD_UP, PUD_DOWN) => down
  pullUpDnControl(IRQpin, PUD_UP);
  pinMode(OUTpin, OUTPUT);
  digitalWrite (OUTpin, LOW) ;

  printf("Testing waitForInterrupt on both edges IRQ @ GPIO%d, timeout is %d\n", IRQpin, TIMEOUT);
  struct WPIWfiStatus wfiStatus = waitForInterrupt2(IRQpin, INT_EDGE_BOTH, TIMEOUT, BOUNCETIME_WFI);
  if (wfiStatus.status < 0) {
    printf("waitForInterrupt returned error\n");
    pinMode(OUTpin, INPUT);
    return 0;
  }
  else if (wfiStatus.status == 0) {
    printf("waitForInterrupt timed out\n\n");
  }
  else {
    if (wfiStatus.edge == INT_EDGE_FALLING)
        printf("waitForInterrupt: GPIO pin %d falling edge fired at %lld microseconds\n\n", wfiStatus.gpioPin, wfiStatus.timeStamp_us);
    else
        printf("waitForInterrupt: GPIO pin %d rising edge fired at %lld microseconds\n\n", wfiStatus.gpioPin, wfiStatus.timeStamp_us);
  }

  printf("Testing IRQ @ GPIO%d on both edges and bouncetime %d microseconds. Toggle LED @ GPIO%d on IRQ.\n\n", IRQpin, BOUNCETIME, OUTpin);
  printf("To stop program hit return key\n\n");

  wiringPiISR2(IRQpin, INT_EDGE_BOTH, &wfi, BOUNCETIME, NULL); 

  getc(stdin);

  wiringPiISRStop (IRQpin);
  pinMode(OUTpin, INPUT);

  return 0;
}
```

Output on terminal:

```
pi@RaspberryPi:~/wiringpi-test-v3.16 $ gcc -o isr_debounce isr_debounce.c -l wiringPi
pi@RaspberryPi:~/wiringpi-test-v3.16 $ ./isr_debounce

ISR debounce test (WiringPi 3.16)

Testing waitForInterrupt on both edges IRQ @ GPIO16, timeout is 10000
waitForInterrupt: GPIO pin 16 falling edge fired at 256522528012 microseconds

Testing IRQ @ GPIO16 on both edges and bouncetime 3000 microseconds. Toggle LED @ GPIO12 on IRQ.

To stop program hit return key

gpio BCM = 16, IRQ edge = rising, timestamp = 256522668010 microseconds, timenow = 256522668017, diff = 7
gpio BCM = 16, IRQ edge = falling, timestamp = 256536364014 microseconds, timenow = 256536364021, diff = 7
gpio BCM = 16, IRQ edge = rising, timestamp = 256536952011 microseconds, timenow = 256536952018, diff = 7
gpio BCM = 16, IRQ edge = falling, timestamp = 256537856010 microseconds, timenow = 256537856016, diff = 6
gpio BCM = 16, IRQ edge = rising, timestamp = 256538744011 microseconds, timenow = 256538744018, diff = 7
gpio BCM = 16, IRQ edge = falling, timestamp = 256539664010 microseconds, timenow = 256539664017, diff = 7
gpio BCM = 16, IRQ edge = rising, timestamp = 256540516010 microseconds, timenow = 256540516016, diff = 6
gpio BCM = 16, IRQ edge = falling, timestamp = 256541560013 microseconds, timenow = 256541560022, diff = 9
gpio BCM = 16, IRQ edge = rising, timestamp = 256542360010 microseconds, timenow = 256542360016, diff = 6
gpio BCM = 16, IRQ edge = falling, timestamp = 256543320012 microseconds, timenow = 256543320020, diff = 8
gpio BCM = 16, IRQ edge = rising, timestamp = 256544092021 microseconds, timenow = 256544092029, diff = 8
^C
pi@RaspberryPi:~/wiringpi-test-v3.16 $
```


## Hardware Pulse Width Modulation (PWM)

Available GPIOs:  [https://pinout.xyz/pinout/pwm](https://pinout.xyz/pinout/pwm)

### pwmWrite

Changes the PWM value of the pin. Possible values ​​are 0 -> { PWM Range }.

```C
pwmWrite(int pin, int value);
```

``pin``: The desired Pin (BCM-, WiringPi-, or Pin-number).  
``value``: PWM value

### pwmSetRange

Set the range for the PWM value of all PWM pins or PWM channels.
19200 / divisor / range applies to the calculation of the PWM frequency (m/s mode).
If ``pinMode(pin, PWM_OUTPUT)`` The value 1024 is automatically set for the divider.

```C
pwmSetRange (unsigned int range);
```

``range``: PWM Range

### pwmSetMode

Set the PWM mode for all PWM pins or PWM channels.

```C
pwmSetMode(int mode);
```

``mode``: PWM Mode

- `PWM_MODE_MS` ... Mark / Space Mode (PWM Fixed Frequency)  
- `PWM_MODE_BAL` ... Balanced Mode (PWM Variable Frequency)

**Notice:**  

Raspberry Pi 5 does not support the balanced mode!

### pwmSetClock

Set the divider for the basic PWM. The base clock is standardized for all Raspberry Pi's to 1900 kHz.
19200 / divisor / range applies to the calculation of the PWM frequency (m/s mode).
If ``pinMode(pin, PWM_OUTPUT)`` The value 32 is automatically set for the divider.

```C
pwmSetClock(int divisor);
```

``divisor``: Divider (Raspberry Pi 4: 1-1456, all other 1-4095)

- `0` ... Deactivates the PWM clock on Raspberry Pi 5, with other Pi's divisor `1` is used

**Notice:**  

Due to its higher internal basic clock, the Raspberry Pi 4 only has a setting range of `1 - 1456`.
Otherwise, `0 - 4095` applies to a valid divider.

**Example:**  

```C
int main (void) {
    wiringPiSetupGpio();

    pwmSetRange(1024);
    pwmSetClock(35);

    pwmWrite(18, 512);

     // Notice: PWM_BAL_OUTPUT NOT supported on Pi 5
    pinMode(18, PWM_MS_OUTPUT);

    double freq = 19200.0 / (double)pwmc / (double)pwmr;

    printf("PWM 50%% @ %g kHz", freq);
    delay(250);

    pinMode(18, PM_OFF);
}
```

## I2C - Bus

``wiringPiI2CRawWrite`` and ``wiringPiI2CRawRead`` are the new functions in version 3 that now allow direct sending and reading of I2C data. The other write and read functions use the SMBus protocol, which is commonly used with I2C chips.

### wiringPiI2CSetup

Open the default I2C bus on the Raspberry Pi and addresses the specified device / slave.

```C
wiringPiI2CSetup(const int devId);
```

``devId``: I2C device / slave address.  
``Return Value``: File handle to I2C bus, or -1 on error.

**Example**  

```C
int fd = wiringPiI2CSetup(0x20);
```

### wiringPiI2CSetupInterface

Opens the specified I2C bus and addresses the specified I2C device / slave.

```C
wiringPiI2CSetupInterface(const char *device, int devId);
```

``devId``: I2C device / slave address.  
``Return Value``: File handle to I2C bus, or -1 on error.

**Example**  

```C
int fd = wiringPiI2CSetupInterface("/dev/i2c-1", 0x20);
```

### wiringPiI2CWrite

Simple device write. Some devices accept data this way without needing to access any internal registers.

### wiringPiI2CWriteReg8

Writes 8-bit data value to the device register.

### wiringPiI2CWriteReg16

Writes 16-bit data value to the device register.

### wiringPiI2CWriteBlockData

Writes specified byte data values to the device register.

### wiringPiI2CRawWrite

Writing data about an I2C slave.

```C
int wiringPiI2CRawWrite(int fd, const uint8_t *values, uint8_t size);
```

``fd``: File Handle.  
``values``: Source buffer.  
``size``: Number of bytes of the source buffer that should be written.  
``Return Value``:  Number of bytes that were written.

**Example**  

```C
int fd = wiringPiI2CSetup(I2C_ADDR);

if (fd > 0) {
    uint8_t i2cvalue = 0x55;
    int result = wiringPiI2CRawWrite(fd, &i2cvalue, 1);

    if (1 == result) {
        // 1 byte from i2cvalue send to I2C_ADDR slave  
    } else {
        // error
    }
}
else {
    // error
}
```


### wiringPiI2CRead


Simple read from I2C slave. Some devices accept data this way without needing to access any internal registers.

### wiringPiI2CReadReg8

Reads 8-bit data value from the device register.

### wiringPiI2CReadReg16

Reads 16-bit data value from the device register.


### wiringPiI2CReadBlockData

Reads specified byte data values from the device register.

### wiringPiI2CRawRead

Reading data from an I2C slave.

```C
int wiringPiI2CRawRead(int fd, uint8_t *values, uint8_t size);
```

``fd``: File Handle.  
``values``: Target buffer.  
``size``: Number of bytes that are to be read into the target buffer.  
``Return Value``:  Number of bytes that were read.

**Example**  

```C
int fd = wiringPiI2CSetup(I2C_ADDR);

if (fd > 0) {
    uint8_t i2cvalue;
    int result = wiringPiI2CRawRead(fd, &i2cvalue, 1);
    
    if (1 == result) {
        // 1 byte received from I2C_ADDR and stored to i2cvalue  
    } else {
        // error
    }
}
else {
    // error
}
```

## SPI - Bus

Functions that start with  ``wiringPiSPIx`` are new since version 3, allowing the SPI bus number to be specified. This is especially useful for the Compute Module, which has multiple SPI buses (0-7). The old functions remain available, but they always refer to SPI bus 0, which is available on the 40 pin GPIO connector.

### wiringPiSPISetup / wiringPiSPISetupMode / wiringPiSPIxSetupMode

Opens the specified SPI bus. The Raspberry Pi has 2 channels, 0 and 1. The speed parameter is an integer in the range of 500,000 through 32,000,000 and represents the SPI clock speed in Hz.

```C
int wiringPiSPISetup(int channel, int speed);

int wiringPiSPISetupMode(int channel, int speed, int mode);

int wiringPiSPIxSetupMode(const int number, const int channel, const int speed, const int mode);
```

``number``: SPI number (typically 0, on Compute Module 0-7).  
``channel``: SPI channel (typically 0 or 1, on Compute Module 0-3).  
``speed``: SPI clock speed in Hz (500,000 to 32,000,000).  
``mode``: SPI mode ([www.kernel.org/doc/Documentation/spi/spidev](https://www.kernel.org/doc/Documentation/spi/spidev)).  
``Return Value``:  File handle to the SPI bus, or -1 on error.

**Example**  

```C
const int spiChannel = 1;
const int spiSpeedInit = 250000; // Hz
int hSPI;

if ((hSPI = wiringPiSPISetup (spiChannel, spiSpeed)) < 0) {
    //error
}

//operate SPI

wiringPiSPIClose(spiChannel);
```

### wiringPiSPIDataRW / wiringPiSPIxDataRW

A synchronous fullduplex write and read operation is performed on the opened SPI bus. In the process, the sent data is overwritten by the received data.

```C
int wiringPiSPIDataRW (int channel, unsigned char *data, int len);

int wiringPiSPIxDataRW (const int number, const int channel, unsigned char *data, const int len);
```

``number``: SPI number (typically 0, on Compute Module 0-7).  
``channel``: SPI channel (typically 0 or 1, on Compute Module 0-3).  
``data``: Buffer  
``len``: Size of ``data`` buffer or data size.  
``Return Value``:  Return Value of ``ioctl`` function ([https://man7.org/linux/man-pages/man2/ioctl.2.html](https://man7.org/linux/man-pages/man2/ioctl.2.html))

> 0 ... Error, see ``errno`` for error number.

**Example**  

```C
const int spiChannel = 1;
const int spiSpeedInit = 250000; // Hz
int hSPI;

if ((hSPI = wiringPiSPIxSetupMode (0, spiChannel, spiSpeed, 0)) < 0) {
    //error
}
unsigned char spiData[3];
int returnvalue;

spiData[0] = 0b11010000;
spiData[1] = 0;
spiData[2] = 0;

returnvalue = wiringPiSPIxDataRW(0, spiChannel, spiData, 3);
if (returnvalue <= 0) {
  printf("SPI transfer error: %d\n", errno);
}

wiringPiSPIxClose(0, spiChannel);
```

### wiringPiSPIGetFd / wiringPiSPIxGetFd

Returns the file handle to the opened SPI bus.

```C
int wiringPiSPIGetFd(int channel);

int wiringPiSPIxGetFd(const int number, int channel);
```

``number``: SPI number (typically 0, on Compute Module 0-7).  
``channel``: SPI channel (typically 0 or 1, on Compute Module 0-3).  
``Return Value``:  File handle to the SPI bus

> -1 ... Invalid or not opened

**Example**  

```C
const int spiChannel = 1;
const int spiSpeedInit = 250000; // Hz

int hSPI;
if ((hSPI = wiringPiSPISetup (spiChannel, spiSpeed)) < 0) {
    //error
}

int fd_spi = wiringPiSPIGetFd(spiChannel);

wiringPiSPIClose(spiChannel);
```
