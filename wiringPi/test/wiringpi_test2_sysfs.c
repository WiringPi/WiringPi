// WiringPi test program: Kernel char device interface / sysfs successor
// Compile: gcc -Wall wiringpi_test2_device.c -o wiringpi_test2_device -lwiringPi

#include "wpi_test.h"
#include <signal.h>
#include <time.h>
#include <sys/time.h>


int GPIO = 19;
int GPIOIN = 26;
const int ToggleValue = 4;


int main (void) {

	printf("WiringPi GPIO test program 2 (using GPIO%d (output) and GPIO%d (input) via sys)\n", GPIO, GPIOIN);
    printf(" testing pullUpDnControl and pinMode PM_OFF\n");

	if (wiringPiSetupSys()  == -1) {
		printf("wiringPiSetupSys failed\n\n");
		exit(EXIT_FAILURE);
	}
	if (!piBoard40Pin()) {
		GPIO = 23;
		GPIOIN = 24;
	}
	
	pinMode(GPIOIN, INPUT);
	pinMode(GPIO, OUTPUT);

	printf("\nTest output\n");
	digitalWriteEx(GPIO, GPIOIN, HIGH);
	delayMicroseconds(600000);
	digitalWriteEx(GPIO, GPIOIN, LOW);
	delayMicroseconds(600000);

    printf("\nTest output off with pull up\n");
    pinMode(GPIO, OUTPUT);
    digitalWriteEx(GPIO, GPIOIN, LOW);
    pullUpDnControl (GPIO, PUD_UP);
    pinMode(GPIO, PM_OFF);
    delayMicroseconds(600000);
    printf("out = off ");
    CheckGPIO(GPIO, GPIOIN, HIGH);
    delayMicroseconds(600000);

	printf("\nTest output off with pull down\n");
	pullUpDnControl (GPIO, PUD_DOWN);
	pinMode(GPIO, PM_OFF);
	delayMicroseconds(600000);
	printf("out = off ");
	CheckGPIO(GPIO, GPIOIN, LOW);
	delayMicroseconds(600000);

  return UnitTestState();
}
