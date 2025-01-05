// WiringPi test program: SPI functions (need XO hardware)
// Compile: gcc -Wall wiringpi_xotest_test1_spi.c -o wiringpi_xotest_test1_spi -lwiringPi

#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include "wpi_test.h"
#include <wiringPiSPI.h>

#define TRUE                (1==1)
#define FALSE               (!TRUE)
#define CHAN_CONFIG_SINGLE  8
#define CHAN_CONFIG_DIFF    0

const float fRefVoltage = 3.3f;
const float fResolution = 4096; //12-Bit
const int spiChannel = 1;
const int spiSpeed = 1000000; // MHz



int AnalogRead(int spiChannel, int analogChannel, int* returnvalue) {
    if (analogChannel<0 || analogChannel>1) {
        return -1;
    }

    unsigned char spiData[3];
    unsigned char chanBits;

    if (analogChannel == 0) {
        chanBits = 0b11010000;
    } else {
        chanBits = 0b11110000;
    }
    spiData[0] = chanBits;
    spiData[1] = 0;
    spiData[2] = 0;
    *returnvalue = wiringPiSPIxDataRW(0, spiChannel, spiData, 3);
    return ((spiData [0] << 9) | (spiData [1] << 1) | (spiData[2] >> 7)) & 0xFFF;
}

void checkVoltage(float expect, const char* szexpect) {
    int returnvalue;

    //int CH0 = AnalogRead(spiChannel, 0, &returnvalue);
    int CH1 = AnalogRead(spiChannel, 1, &returnvalue);
    //float value0 = CH0 * fRefVoltage / fResolution;
    float value1 = CH1 * fRefVoltage / fResolution;
    CheckSameFloat(szexpect, value1, expect, 0.1);
    delayMicroseconds(300);
}


int main(int argc, char *argv []){

    const int GPIOIn = 29;
    int hSPI;
    //int CH0;
    int CH1;
    int major, minor;

    wiringPiVersion(&major, &minor);
    printf("Testing SPI functions with WiringPi %d.%d\n",major, minor);
    printf("------------------------------------------\n\n");

    wiringPiSetup();

    if ((hSPI = wiringPiSPISetup (spiChannel, spiSpeed)) < 0) {
        FailAndExitWithErrno("wiringPiSPISetup", hSPI);
    }

    int hSPIOld=hSPI;
    //printf("\nSPI fd = %d\n call close now\n", hSPI);
    int ret = wiringPiSPIClose(spiChannel);
    if (ret!=0) {
        FailAndExitWithErrno("wiringPiSPIClose", ret);
    }

    if ((hSPI = wiringPiSPIxSetupMode(0, spiChannel, spiSpeed, 0)) < 0) {
        FailAndExitWithErrno("wiringPiSPIxSetup", hSPI);
    }

    CheckSame("SPISetup, Close and SPIxSetup handle", hSPI, hSPIOld);

    int returnvalue;
    //CH0 = AnalogRead(spiChannel, 0, &returnvalue);
    CH1 = AnalogRead(spiChannel, 1, &returnvalue);
    CheckSame("SPI reading ioctl result  (byte count) ", returnvalue, 3);
    //float value0 = CH0 * fRefVoltage / fResolution;
    //float value1 = CH1 * fRefVoltage / fResolution;

    pinMode(21, OUTPUT);
    pinMode(22, INPUT);
    pinMode(24, INPUT);
    pinMode(25, INPUT);
    pinMode(27, INPUT);
    pinMode(28, INPUT);
    pinMode(GPIOIn, INPUT);


    digitalWriteEx(21, GPIOIn, LOW);
    checkVoltage(0.1f, "Analog value 1xLow");
    checkVoltage(0.1f, "Analog value 1xLow");

    digitalWriteEx(21, GPIOIn, HIGH);
    checkVoltage(3.1f, "Analog value 1xHigh");

    pinMode(22, OUTPUT);
    digitalWriteEx(22, -1, LOW);
    checkVoltage(1.65f, "Analog value Half (1H/1L)");

    digitalWriteEx(22, GPIOIn, HIGH);
    checkVoltage(3.2f, "Analog value 2xHigh");

    pinMode(24, OUTPUT);
    digitalWriteEx(24, GPIOIn, HIGH);
    checkVoltage(3.2f, "Analog value 3xHigh");

    digitalWriteEx(24, -1, LOW);
    checkVoltage(2.2f, "Analog value 2xHigh/1xLow");
    checkVoltage(2.2f, "Analog value 2xHigh/1xLow");

    pinMode(25, OUTPUT);
    digitalWriteEx(25, GPIOIn, HIGH);
    checkVoltage(2.475f, "Analog value 3xHigh/1xLow");

    digitalWriteEx(25, -1, LOW);
    checkVoltage(1.65f, "Analog value Half (2H/2L)");

    pinMode(27, OUTPUT);
    digitalWriteEx(27, GPIOIn, HIGH);
    checkVoltage(1.98f, "Analog value 3xHigh/2xLow");

    digitalWriteEx(27, -1, LOW);
    checkVoltage(1.32f, "Analog value Half (2H/3L)");

    pinMode(28, OUTPUT);
    digitalWriteEx(28, GPIOIn, LOW);
    checkVoltage(1.100f, "Analog value 2xHigh/4xLow");

    digitalWriteEx(28, GPIOIn, HIGH);
    checkVoltage(1.65f, "Analog value Half (3H/3L)");

    digitalWriteEx(27, GPIOIn, HIGH);
    checkVoltage(2.2f, "Analog value  4xHigh/2xLow");

    digitalWriteEx(25, GPIOIn, HIGH);
    checkVoltage(2.75f, "Analog value  5xHigh/1xLow");

    digitalWriteEx(24, GPIOIn, HIGH);
    checkVoltage(3.3f, "Analog value  6xHigh");

    CH1 = AnalogRead(3, 1, &returnvalue);
    CheckSame("\nReading Wrong channel 3 result         ", CH1, 0);
    CheckSame("\nReading Wrong channel 3 ioctl result   ", returnvalue, -EINVAL);
    CH1 = AnalogRead(2, 1, &returnvalue);
    CheckSame("\nReading Wrong channel 2 result         ", CH1, 0);
    CheckSame("\nReading Wrong channel 3  ioctl result  ", returnvalue, -EBADF);

    pinMode(22, INPUT);
    pinMode(21, INPUT);
    pinMode(24, INPUT);
    pinMode(25, INPUT);
    pinMode(27, INPUT);
    pinMode(28, INPUT);

    ret = wiringPiSPIxClose(0, spiChannel);
    CheckSame("wiringPiSPIxClose result", ret, 0);
    if (ret!=0) {
        FailAndExitWithErrno("wiringPiSPIxClose", ret);
    }
    ret = wiringPiSPIxGetFd(0, spiChannel);
    CheckSame("Fd after close", ret, -1);

    return UnitTestState();
}

