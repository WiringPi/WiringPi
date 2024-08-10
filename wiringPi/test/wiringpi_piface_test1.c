// WiringPi piface program IN, OUT, PULL
// Compile: gcc -Wall wiringpi_piface_test1.c -o wiringpi_piface_test1 -lwiringPi -lwiringPiDev

#include "wpi_test.h"
#include <piFace.h>

// Use 200 as the pin-base for the PiFace board, and change all pins
//	for the LED and relays

const int PIFACE = 200;          //Mapped wiringpi IO address
const int defaultsleep = 200000; // 200 ms

void ReadUntilTimeout(int GPIO, int expect, int timeoutSec) {
	const int intervaluS =  250000; //250ms
	int in;
	const char* strexpect = expect ? "HIGH" : "LOW";
	for(int loop=0, end=(timeoutSec*1000000/intervaluS); loop<end; ++loop) {
		in = digitalRead(GPIO);
		if (in==expect) {
			printf( "took %g sec to set %s\n", loop*intervaluS/1000000.0, strexpect);
			break;
		}
		delayMicroseconds(intervaluS);
		printf(".");fflush(stdout);
	}
	if (in!=expect) {
		printf( "timeout reached %d sec to set %s\n", timeoutSec, strexpect);
	}
	CheckGPIO(GPIO, -1, expect) ;
}

int main (int argc, char *argv []) {
    int major, minor;

    wiringPiVersion(&major, &minor);
    printf("Testing piface functions with WiringPi %d.%d\n",major, minor);
    printf("------------------------------------------\n\n");

	// initialise wiringPi
	if (wiringPiSetupSys()  == -1) {
		printf("wiringPiSetupSys failed\n\n");
		exit(EXIT_FAILURE);
	}

	piFaceSetup (PIFACE); // Setup the PiFace board with default addr 0, 0

	const int RELAY0 = PIFACE+0;
	const int RELAY0IN = PIFACE+6;
	const int RELAY1 = PIFACE+1;
	const int RELAY1IN = PIFACE+7;


	printf("\nRelays async:\n");
	pullUpDnControl(RELAY0IN, PUD_UP);
	pullUpDnControl(RELAY1IN, PUD_UP);
	for (int loop = 0, end=3 ; loop<end ; ++loop) {
		int sleep = defaultsleep*(end-loop);
		printf("sleep %d ms:\n", sleep/1000);
		digitalWrite (RELAY0, HIGH);
		delayMicroseconds(sleep);
		CheckInversGPIO(RELAY0IN, -1, HIGH) ;
		digitalWrite (RELAY0, LOW);
		delayMicroseconds(sleep);
		CheckInversGPIO(RELAY0IN, -1, LOW);

		digitalWrite (RELAY1, HIGH);
		delayMicroseconds(sleep);
		CheckInversGPIO(RELAY1IN, -1, HIGH);
		digitalWrite (RELAY1, LOW);
		delayMicroseconds(sleep);
		CheckInversGPIO(RELAY1IN, -1, LOW);
	}

	const int OUT7 = PIFACE+7;
	const int OUT7IN = PIFACE+4;
	const int OUT6 = PIFACE+6;
	const int OUT6IN = PIFACE+5;

	printf("\nOUT6/7 sync:\n");
	delayMicroseconds(defaultsleep);
	for (int loop = 0, end=3 ; loop<end ; ++loop) {
		digitalWrite (OUT7, HIGH);
		delayMicroseconds(defaultsleep);
		CheckInversGPIO(OUT7IN, -1, HIGH);

		digitalWrite (OUT7, LOW);
		delayMicroseconds(defaultsleep);
		CheckInversGPIO(OUT7IN, -1, LOW);

		digitalWrite (OUT6, HIGH);
		delayMicroseconds(defaultsleep);
		CheckInversGPIO(OUT6IN, -1, HIGH);

		digitalWrite (OUT6, LOW);
		delayMicroseconds(defaultsleep);
		CheckInversGPIO(OUT6IN, -1, LOW);
	}

	printf("\nRelays sync:\n");
	for (int loop = 0, end=3 ; loop<end ; ++loop) {
		int sleep = defaultsleep*(end-loop);
		printf("sleep %d ms:\n", sleep/1000);
		digitalWrite (RELAY0, HIGH);
		digitalWrite (RELAY1, HIGH);
		delayMicroseconds(sleep);
		CheckInversGPIO(RELAY0IN, -1, HIGH) ;
		CheckInversGPIO(RELAY1IN, -1, HIGH) ;

		digitalWrite (RELAY0, LOW);
		digitalWrite (RELAY1, LOW);
		delayMicroseconds(sleep);
		CheckInversGPIO(RELAY0IN, -1, LOW) ;
		CheckInversGPIO(RELAY1IN, -1, LOW) ;
	}

	printf("\nInput pull up/down resistor:\n");
	for (int IN = 0 ; IN <= 7 ; ++IN) {
		if (4==IN || 5==IN) {
			continue;  //4 & 5 connected from out to in -> test not possible
		}
		//6 & 7 connected from relais NO (normaly open) to in  -> test possible
		delayMicroseconds(defaultsleep);
		pullUpDnControl (PIFACE + IN, PUD_UP) ;
		ReadUntilTimeout(PIFACE + IN, HIGH, 2) ;
		pullUpDnControl (PIFACE + IN, PUD_DOWN) ;
		// cool down very slowly, connect 680 kOhm pull down resistor to make ist faster
		ReadUntilTimeout(PIFACE + IN, LOW, 60) ;
		pullUpDnControl (PIFACE + IN, PUD_UP) ;  // finally up
		ReadUntilTimeout(PIFACE + IN, HIGH, 2) ;
	}

	return UnitTestState();
}
