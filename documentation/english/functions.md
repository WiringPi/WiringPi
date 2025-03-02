# Documentation WiringPi-library

The WiringPi-library enables access to the GPIO pins of the Raspberry Pi. In this documentation you will find information about the available functions and their use.
Since Version 3, extensions to the interface have been made again. In the case of new implementations, you should rely on the current / new functions.  
The old [GPIO Sysfs Interface for Userspace](https://www.kernel.org/doc/Documentation/gpio/sysfs.txt) is no longer supported.

**Attention:** This documentation is still in progress and therefore incomplete.
The content of this documentation was created with care and to the best of our knowledge and belief. However, the authors do not guarantee the correctness, completeness and topicality of the information provided.The content of the documentation is used at your own risk.
No liability is generally assumed for damage caused by material or intangible nature caused by the use or non-use of the information provided or by the use of incorrect and incomplete information.

## Installation

Unfortunately, the WiringPi Library is not directly available in Raspberry Pi OS, so it must be installed manually. Either download a Debian package or create it manually.

**Create Debian package:**

```bash
sudo apt install git
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build debian
mv debian-template/wiringpi-3.0-1.deb .
```

**Install Debian package:**

```bash
sudo apt install ./wiringpi-3.0-1.deb
```

**Uninstall Debian package:**

```bash
sudo apt purge wiringpi
```


## PIN Numbering and Raspberry Pi Models

GPIOs: https://pinout.xyz/pinout/wiringpi

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

``wiringPiSetup`` uses Wiringpi numbering (WPI) of the GPIO's and accesses the GPIO register directly.  
``wiringPiSetupGpio`` uses BCM numbering of the GPIOs and accesses the GPIO registers directly.  
``wiringPiSetupPhys`` uses physical PIN numbering of the GPIOs and accesses the GPIO register directly.  
``wiringPiSetupSys`` uses BCM numbering and calls the new function ``wiringPiSetupGpioDevice`` from version 3.4 to ensure compatibility with new core. In version 2, the virtual file system/SYS/Class/GPIO was used. However, the GPIO's exports had to take place externally before the initialization! The function is outdated and should not be used!

**Since Version 3.4:**  
``wiringPiSetupPinType`` decides whether WiringPi, BCM or physical PIN numbering is now used, based on the parameter pinType. So it combines the first 3 setup functions together.
``wiringPiSetupGpioDevice`` is the successor to the ``wiringPiSetupSys`` function and now uses "GPIO Character Device Userspace API" in version 1 (from kernel 5.10). More information can be found at [docs.kernel.org/driver-api/gpio/driver.html](https://docs.kernel.org/driver-api/gpio/driver.html) on the parameter pintype, it is again decided which pin numbering is used.
In this variant, there is no direct access to the GPIO memory (DMA) but rather through a kernel interface that is available with user permissions. The disadvantage is the limited functionality and low performance.

<!-- ​​Attention code that uses the nine functions is no longer compatible with the older library version 2! -->

### wiringPiSetup V2 (outdated)

Inializating WiringPi in a classic way.
>>>
```C
int wiringPiSetupGpio(void)
```  

``Return Value``:  Error status

>  0 ... No Error  

**Example:**

```C
wiringPiSetupGpio();
```

### wiringPiSetup V3

Initializing WiringPi.  
>>>
```C
int wiringPiSetupPinType(enum WPIPinType pinType)
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
>>>
```C
void pinMode(int pin, int mode)
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

**Support:**  
`PM_OFF` resets the GPIO (Input) and releases it. PWM is stopped.
Raspberry Pi 5 does not support the PWM Bal (Balanced) mode. The MS mode is activated at `PWM_OUTPUT`.
`GPIO_CLOCK` is currently not yet supported in Raspberry Pi 5 (RP1).

**PWM Exit**  
`PWM_OUTPUT` Activates the specified PWM output with the settings:
 - Mode: BAL-Balanced (Pi0-4), MS-Mark/Space (Pi 5)
 - Range: 1024  
 - Divider: 32  

In order to make sure that the output starts without an active frequency, you should execute ``pwmWrite(PWM_GPIO, 0);`` before activating.
After that, the corresponding clock and range values ​​can be adapted, without a frequency already being output unintentionally.

### pinMode

### digitalWrite

Set the value of a GPIO pin.
>>>
```C
void digitalWrite(int pin, int value)
```

``pin``: The desired Pin (BCM-, Wiringpi- or PIN number).  
``value``: The logical value...
 - `HIGH` ... Value 1 (electrical ~ 3.3 V)  
 - `LOW` ... Value 0 (electrical ~0 V / GND)

**Example:**

```C
pinMode(17, OUTPUT);
DigitalWrite(17, HIGH);
```

### pullUpDnControl

Changes the internal pull-up / pull-down resistance.
>>>
```C
void pullUpDnControl (int pin, int pud)
```  

``pin``: The desired Pin (BCM-, WiringPi-, or Pin-number).  
``pud``: The resistance type...
 - `PUD_OFF` ... No resistance   
 - `PUD_UP` ... Pull-Up resistance (~50 kOhm)
 - `PUD_DOWN` ... Pull-Down resistance (~50 kOhm)

**Example:**

```C
pullUpDnControl(17, PUD_DOWN);
```

### digitalRead

Reads the value of one GPIO-Pin.
>>>
```C
int digitalRead(int pin)
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

>>>
```C
int wiringPiISR(int pin, int mode, void (*function)(void));
```

``pin``: The desired Pin (BCM-, WiringPi-, or Pin-number).  
``mode``: Triggering edge mode...
 - `INT_EDGE_RISING` ... Rising edge  
 - `INT_EDGE_FALLING` ... Falling edge  
 - `INT_EDGE_BOTH` ... Rising and falling edge  

``*function``: Function pointer for ISR  
``Return Value``:
 
> 0 ... Successful

<!-- > <>0 ... Error not implemented at the moment. -->

For example see **wiringPiISRStop**.


### wiringPiISRStop

Deregisters the Interrupt Service Routine (ISR) on a Pin.

>>>
```C
int wiringPiISRStop (int pin)
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

    wiringPiISR (17, INT_EDGE_RISING, &isr);

    Sleep(1000);
    printf("%d rinsing edges\n", );

    wiringPiISRStop(17);
}
```


### waitForInterrupt

Waits for a previously defined interrupt (wiringPiISR) on the GPIO pin. This function should not be used.

>>>
```C
int  waitForInterrupt (int pin, int mS)
```

``pin``: The desired Pin (BCM-, WiringPi-, or Pin-number).  
``mS``: Timeout in milliseconds.  
``Return Value``: Error  
> 0 ... Successful  
> -1 ... GPIO Device Chip not successfully opened  
> -2 ... ISR was not registered (WiringPiISR must be called)


## Hardware Pulse Width Modulation (PWM)

Available GPIOs:  https://pinout.xyz/pinout/pwm

### pwmWrite

Changes the PWM value of the pin. Possible values ​​are 0 -> { PWM Range }.
>>>
```C
pwmWrite(int pin, int value)
```

``pin``: The desired Pin (BCM-, WiringPi-, or Pin-number).  
``value``: PWM value

### pwmSetRange

Set the range for the PWM value of all PWM pins or PWM channels.
19200 / divisor / range applies to the calculation of the PWM frequency (m/s mode).
If ``pinMode(pin, PWM_OUTPUT)`` The value 1024 is automatically set for the divider.
>>>
```C
pwmSetRange (unsigned int range)
```

``range``: PWM Range

### pwmSetMode

Set the PWM mode for all PWM pins or PWM channels.
>>>
```C
pwmSetMode(int mode);
```

``mode``: PWM Mode
 - `PWM_MODE_MS` ... Mark / Space Mode (PWM Fixed Frequency)  
 - `PWM_MODE_BAL` ... Balanced Mode (PWM Variable Frequency)

**Support:**  
Raspberry Pi 5 does not support the balanced mode!


### pwmSetClock

Set the divider for the basic PWM. The base clock is standardized for all Raspberry Pi's to 1900 kHz.
19200 / divisor / range applies to the calculation of the PWM frequency (m/s mode).
If ``pinmode(pin, PWM_OUTPUT)`` The value 32 is automatically set for the divider.
>>>
```C
pwmSetClock(int divisor)
```

``divisor``: Divider (Raspberry Pi 4: 1-1456, all other 1-4095) 

- `0` ... Deactivates the PWM clock on Raspberry Pi 5, with other Pi's divisor `1` is used

**Support:**  
Due to its higher internal basic clock, the Raspberry Pi 4 only has a setting range of `1 - 1456`.
Otherwise, `0 - 4095` applies to a valid divider.

**Example:**

```C
int main (void) {
    wiringPiSetupGpio() ;

    pwmSetRange(1024);
    pwmSetClock(35);

    pwmWrite(18, 512);
    
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

>>>
```C
wiringPiI2CSetup(const int devId)
```

``devId``: I2C-Gerät / Slave Adresse.  
``Return Value``:  File Handle to I2C-Bus  

> -1 ... Error or EXIT (Programm termination)

**Example**
>>>
```C
int fd = wiringPiI2CSetup(0x20);
```

### wiringPiI2CSetupInterface

Opens the specified I2C bus and addresses the specified I2C device / slave.

>>>
```C
wiringPiI2CSetupInterface(const char *device, int devId)
```

``devId``: I2C-Gerät / Slave Adresse.  
``Return Value``:  File Handle to I2C-Bus 

> -1 ... Error or EXIT (Programm termination)

**Example**
>>>
```C
int fd = wiringPiI2CSetupInterface("/dev/i2c-1", 0x20);
```


### wiringPiI2CWrite / wiringPiI2CWriteReg8 / wiringPiI2CWriteReg16 / wiringPiI2CWriteBlockData

...

### wiringPiI2CRawWrite

Writing data about an I2C slave.
>>>
```C
int wiringPiI2CRawWrite(int fd, const uint8_t *values, uint8_t size)
```

``fd``: File Handle.  
``values``: Source buffer.  
``size``: Number of bytes of the source buffer that should be written.  
``Return Value``:  Number of bytes that were written.

**Example**
>>>
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

### wiringPiI2CRead / wiringPiI2CReadReg8 / wiringPiI2CReadReg16 / wiringPiI2CReadBlockData

...

### wiringPiI2CRawRead

Reading data from an I2C slave.
>>>
```C
int wiringPiI2CRawRead(int fd, uint8_t *values, uint8_t size)
```

``fd``: File Handle.  
``values``: Target buffer.  
``size``: Number of bytes that are to be read into the target buffer.  
``Return Value``:  Number of bytes that were read.

**Example**
>>>
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

Opens the specified SPI bus.

>>>
```C
int wiringPiSPISetup (int channel, int speed)
int wiringPiSPISetup (int channel, int speed, int mode)
int wiringPiSPIxSetupMode(const int number, const int channel, const int speed, const int mode)
```

``number``: SPI muber (typically 0, on Compute Module 0-7).  
``channel``: SPI channel (typically 0 or 1, on Compute Module 0-3).  
``speed``: SPI clock.  
``mode``: SPI mode (https://www.kernel.org/doc/Documentation/spi/spidev).  
``Return Value``:  File handle to the SPI bus
> -1 ... Error or EXIT (Programm termination)

**Example**
>>>
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

>>>
```C
int wiringPiSPIDataRW (int channel, unsigned char *data, int len)
int wiringPiSPIxDataRW (const int number, const int channel, unsigned char *data, const int len)
```

``number``: SPI muber (typically 0, on Compute Module 0-7).  
``channel``: SPI channel (typically 0 or 1, on Compute Module 0-3).  
``data``: Buffer  
``len``: Size of ``data`` buffer or data size.  
``Return Value``:  Return Value of ``ioctl`` function (https://man7.org/linux/man-pages/man2/ioctl.2.html)  
<0 ... Error, see ``errno`` for error number.

**Example**
>>>
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
if (returnvalue<=0) {
  printf("SPI transfer error: %d\n", errno);
}

wiringPiSPIxClose(0, spiChannel);
```

### wiringPiSPIGetFd / wiringPiSPIxGetFd

Returns the file handle to the opened SPI bus.

>>>
```C
int wiringPiSPIGetFd(int channel)
int wiringPiSPIxGetFd(const int number, int channel)
```

``number``: SPI muber (typically 0, on Compute Module 0-7).  
``channel``: SPI channel (typically 0 or 1, on Compute Module 0-3).  
``Return Value``:  File handle to the SPI bus 
> -1 ... Invalid or not opened

**Example**
>>>
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
