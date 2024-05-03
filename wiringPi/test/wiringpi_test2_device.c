// WiringPi test program: Kernel char device interface / sysfs successor
// Compile: gcc -Wall wiringpi_test2_device.c -o wiringpi_test2_device -lwiringPi

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


const int GPIO = 19;
const int GPIOIN = 26;
const int  ToggleValue = 4;

void CheckGPIO(int out) {
	int in  = digitalRead(GPIOIN);
	int read = digitalRead(GPIO);

	int pass = 0;
	if (out==in && in==read) {
		pass = 1;
	}
	printf("GPIO%d = %d (GPIO%d = %d)  -> %s\n", GPIOIN, in, GPIO, read, pass ? "passed":"failed" );
}

void digitalWriteEx(int pin, int mode) {
	digitalWrite(pin, mode);
	printf("out = %d   ", mode);
	delayMicroseconds(5000);
	CheckGPIO(mode);
}

void pullUpDnControlEx (int pin ,int mode) {
	pullUpDnControl (pin, mode);
	int out  = mode==PUD_UP ? 1:0;
	printf("in = %4s   ", mode==PUD_UP ? "up":"down");
	delayMicroseconds(5000);
	CheckGPIO(out);
}

int main (void) {

	printf("WiringPi GPIO test program 2 (using GPIO%d (output) and GPIO%d (input) via sys)\n", GPIO, GPIOIN);
    printf(" testing pullUpDnControl and pinMode PM_OFF\n");

	if (wiringPiSetupSys()  == -1) {
		printf("wiringPiSetupSys failed\n\n");
		exit(EXIT_FAILURE);
	}
	pinMode(GPIOIN, INPUT);
	pinMode(GPIO, OUTPUT);

	printf("\nTest output\n");
	digitalWriteEx(GPIO, HIGH);
	delayMicroseconds(600000);
	digitalWriteEx(GPIO, LOW);
	delayMicroseconds(600000);

    printf("\nTest output off with pull up\n");
    pinMode(GPIO, OUTPUT);
    digitalWriteEx(GPIO, LOW);
    pullUpDnControl (GPIO, PUD_UP);
    pinMode(GPIO, PM_OFF);
    delayMicroseconds(600000);
    printf("out = off ");
    CheckGPIO(HIGH);
    delayMicroseconds(600000);

	printf("\nTest output off with pull down\n");
	pullUpDnControl (GPIO, PUD_DOWN);
	pinMode(GPIO, PM_OFF);
	delayMicroseconds(600000);
	printf("out = off ");
	CheckGPIO(LOW);
	delayMicroseconds(600000);

	return(EXIT_SUCCESS);
}
