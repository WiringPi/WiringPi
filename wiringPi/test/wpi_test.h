
#include <wiringPi.h>
#include <stdio.h>

#define COLORDEF  "\x1B[0m"
#define COLORRED  "\x1B[31m"
#define COLORGRN  "\x1B[32m"

void CheckGPIO(int GPIO, int GPIOIN, int out) {
	int in  = digitalRead(GPIOIN);
	int read = digitalRead(GPIO);

	int pass = 0;
	if (out==in && in==read) {
		pass = 1;
	}
	if (pass) {
		printf("GPIO%d = %d (GPIO%d = %d)  -> %spassed%s\n", GPIOIN, in, GPIO, read, COLORGRN, COLORDEF );	
	} else {
		printf("GPIO%d = %d (GPIO%d = %d)  -> %sfailed%s\n", GPIOIN, in, GPIO, read, COLORRED, COLORDEF );
	}
}


void digitalWriteEx(int GPIO, int GPIOIN, int mode) {
	digitalWrite(GPIO, mode);
	printf("out = %d   ", mode);
	delayMicroseconds(5000);
	CheckGPIO(GPIO, GPIOIN, mode);
}


void pullUpDnControlEx (int GPIO, int GPIOIN, int mode) {
	pullUpDnControl (GPIO, mode);
	int out  = mode==PUD_UP ? 1:0;
	printf("in = %4s   ", mode==PUD_UP ? "up":"down");
	delayMicroseconds(5000);
	CheckGPIO(GPIO, GPIOIN, out);
}


void CheckSame(const char* msg, int value, int expect) {
	if (value==expect) {
		printf("%s -> %spassed%s\n", msg, COLORGRN, COLORDEF );	
	} else {
		printf("%s -> %sfailed%s\n", msg, COLORRED, COLORDEF );
	}
}
