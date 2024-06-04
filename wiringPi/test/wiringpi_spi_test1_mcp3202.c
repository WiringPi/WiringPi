// WiringPi test program: SPI functions (need MCP3202 hardware @ CE1, ch0=Vdd, ch1=Vdd/2)
// Compile: gcc -Wall wiringpi_spi_test1_mcp3202.c -o wiringpi_spi_test1_mcp3202 -lwiringPi

#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include "wpi_test.h"
#include <wiringPiSPI.h>

const float fRefVoltage = 3.3f;
const float fResolution = 4096; //12-Bit
const int spiChannel = 1;
const int spiSpeedInit = 250000; // Hz


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


int main(int argc, char *argv []){

    int hSPI;
    int CH0,CH1;
    float value0, value1;
    int returnvalue;
    int spiSpeed;
    int major, minor;

    wiringPiVersion(&major, &minor);
    printf("Testing SPI functions with WiringPi %d.%d\n",major, minor);
    printf("------------------------------------------\n\n");

    wiringPiSetup();
    spiSpeed = spiSpeedInit;
    for (int testno=0; testno<=2; testno++) { 
        printf("\nTest 5d with %g MHz SPI Speed\n", spiSpeed/1000000.0f);
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
        
        delayMicroseconds(500000);
        returnvalue = -1;
        CH0 = AnalogRead(spiChannel, 0, &returnvalue);
        CheckSame("CH0 wiringPiSPIxDataRW return", returnvalue, 3);
        value0 = CH0 * fRefVoltage / fResolution;
        CheckSameFloat("CH0 value (VDD)", value0, 3.3f);
        printf("\n");
        
        delayMicroseconds(500000);
        returnvalue = -1;
        CH1 = AnalogRead(spiChannel, 1, &returnvalue);
        CheckSame("CH1 wiringPiSPIxDataRW return", returnvalue, 3);
        value1 = CH1 * fRefVoltage / fResolution;
        CheckSameFloat("CH1 value (1/2)", value1, 1.65f);
        printf("\n");

        ret = wiringPiSPIxClose(0, spiChannel);
        CheckSame("wiringPiSPIxClose result", ret, 0);
        if (ret!=0) {
            FailAndExitWithErrno("wiringPiSPIxClose", ret);
        }
        ret = wiringPiSPIxGetFd(0, spiChannel);
        CheckSame("Fd after wiringPiSPIxGetFd", ret, -1);

        ret = wiringPiSPIGetFd(spiChannel);
        CheckSame("Fd after wiringPiSPIGetFd", ret, -1);

        spiSpeed += spiSpeed;
    }

    printf("\n");
    hSPI = wiringPiSPISetup (3, spiSpeedInit);
    CheckSame("\nwiringPiSPISetup with wrong channel", hSPI, -22);

    // will result in exit! - not useful in test code
    //hSPI = wiringPiSPIxSetupMode (3, 0, spiSpeedInit,0);
    //CheckSame("\nwiringPiSPISetup with wrong channel", hSPI, -22);

    return UnitTestState();
}

