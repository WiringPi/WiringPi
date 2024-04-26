// WiringPi test program: Kernel char device interface / sysfs successor
// Compile: gcc -Wall wiringpi_test1_device.c -o wiringpi_test1_device -lwiringPi

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

	printf("WiringPi GPIO test program 1 (using GPIO%d (output) and GPIO%d (input) via sys)\n", GPIO, GPIOIN);
	printf(" testing digitalWrite, digitalRead and pullUpDnControl\n");

	if (wiringPiSetupSys()  == -1) {
		printf("wiringPiSetupGpioDevice failed\n\n");
		exit(EXIT_FAILURE);
	}
	pinMode(GPIOIN, INPUT);
	pinMode(GPIO, OUTPUT);

	printf("toggle %d times ...\n", ToggleValue);
	for (int loop=1; loop<ToggleValue; loop++) {
		digitalWriteEx(GPIO, LOW);
		delayMicroseconds(600000);
		digitalWriteEx(GPIO, HIGH);
		delayMicroseconds(600000);
	}

	digitalWrite(GPIO, LOW);

	printf("\nWiringPi GPIO test program (using GPIO%d (input pull up/down) and GPIO%d (input) via sys)\n", GPIO, GPIOIN);
	pullUpDnControl (GPIO, PUD_UP);
	pinMode(GPIO, INPUT);
	delayMicroseconds(3000000);
	pullUpDnControl (GPIOIN, PUD_OFF);

     for (int loop=1; loop<ToggleValue; loop++) {
		pullUpDnControlEx (GPIO, PUD_DOWN);
		delayMicroseconds(600000);
		pullUpDnControlEx (GPIO, PUD_UP);
		delayMicroseconds(600000);
    }

	//Error wrong direction - only for fun
	digitalWrite(GPIO, LOW);

	return(EXIT_SUCCESS);
}

