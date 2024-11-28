// WiringPi test program: Kernel char device interface / sysfs successor
// Compile: gcc -Wall wiringpi_test1_device.c -o wiringpi_test1_device -lwiringPi

#include "wpi_test.h"
#include <signal.h>
#include <time.h>
#include <sys/time.h>


int GPIO = 19;
int GPIOIN = 26;
const int ToggleValue = 4;
int RaspberryPiModel = -1;


void SetAndCheckMode(int pin, int mode) {
	enum WPIPinAlt AltGpio = WPI_ALT_UNKNOWN;

	switch(mode) {
		case INPUT:
			pinMode(pin, INPUT);
			AltGpio = getPinModeAlt(pin);
			CheckSame("Pin mode input", AltGpio, WPI_ALT_INPUT);
			break;
		case OUTPUT:
			pinMode(pin, OUTPUT);
			AltGpio = getPinModeAlt(pin);
			CheckSame("Pin mode output", AltGpio, WPI_ALT_OUTPUT);
			break;
		case PM_OFF:
			pinMode(pin, PM_OFF);
			AltGpio = getPinModeAlt(pin);
			CheckSame("Pin mode off(input)", AltGpio, (PI_MODEL_5 == RaspberryPiModel) ?  WPI_NONE : WPI_ALT_INPUT);
			break;
		default:
			pinMode(pin, mode);
			printf("pinmode %d of pin %d not checked", mode, pin);
			break;
	}
}


int main (void) {

	printf("WiringPi GPIO test program 1 (using GPIO%d (output) and GPIO%d (input))\n", GPIO, GPIOIN);
	printf(" testing digitalWrite, digitalRead and pullUpDnControl\n");

	if (wiringPiSetupGpio()  == -1) {
		printf("wiringPiSetupGpio failed\n\n");
		exit(EXIT_FAILURE);
	}

	int rev, mem, maker, overVolted;
	piBoardId(&RaspberryPiModel, &rev, &mem, &maker, &overVolted);
	CheckNotSame("Model: ", RaspberryPiModel, -1);
	if (PI_MODEL_5 == RaspberryPiModel) {
		printf("Raspberry Pi 5 with RP1 found\n");
	} else {
		printf("Raspberry Pi with BCM GPIO found (not Pi 5)\n");
	}
	if (!piBoard40Pin()) {
		GPIO = 23;
		GPIOIN = 24;
	}


	enum WPIPinAlt AltGpio = WPI_ALT_UNKNOWN;
	AltGpio = getPinModeAlt(23);
	CheckSame("Pin mode default", AltGpio, PI_MODEL_5 == RaspberryPiModel ? WPI_NONE : WPI_ALT_INPUT);

	SetAndCheckMode(GPIOIN, INPUT);
	SetAndCheckMode(GPIO, OUTPUT);

	printf("toggle %d times ...\n", ToggleValue);
	for (int loop=1; loop<ToggleValue; loop++) {
		digitalWriteEx(GPIO, GPIOIN, LOW);
		delayMicroseconds(600000);
		digitalWriteEx(GPIO, GPIOIN, HIGH);
		delayMicroseconds(600000);
	}

	digitalWrite(GPIO, LOW);

	printf("\nWiringPi GPIO test program (using GPIO%d (input pull up/down) and GPIO%d (input))\n", GPIO, GPIOIN);
	pullUpDnControl (GPIO, PUD_UP);
	SetAndCheckMode(GPIO, INPUT);


	delayMicroseconds(3000000);
	pullUpDnControl (GPIOIN, PUD_OFF);

     for (int loop=1; loop<ToggleValue; loop++) {
		pullUpDnControlEx (GPIO, GPIOIN, PUD_DOWN);
		delayMicroseconds(600000);
		pullUpDnControlEx (GPIO, GPIOIN, PUD_UP);
		delayMicroseconds(600000);
    }

	//Error wrong direction - only for fun
	digitalWrite(GPIO, LOW);

	SetAndCheckMode(GPIO, OUTPUT);
	SetAndCheckMode(GPIO, PM_OFF);
	//pinModeAlt (GPIO, 0x1F);
	//AltGpio = getPinModeAlt(GPIO);
	//CheckSame("Pin mode off(default)", AltGpio, 0x1F);

	return UnitTestState();
}
