
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#define COLORDEF  "\x1B[0m"
#define COLORRED  "\x1B[31m"
#define COLORGRN  "\x1B[32m"
#define BOLD      "\x1B[1m"
#define FINALCOLRED  "\x1B[7;49;91m"
#define FINALCOLGRN  "\x1B[7;49;32m"


unsigned int globalError = 0;

void CheckGPIO(int GPIO, int GPIOIN, int out) {

    int in = out;
    if (GPIOIN>=0) {
		in = digitalRead(GPIOIN);
    }
    int readback = digitalRead(GPIO);

    int pass = 0;
	if (out==readback && in==out) {
		pass = 1;
	}

    if (GPIOIN>=0) {
        printf("set GPIO%02d = %d (readback %d), in GPIO%02d = %d         ", GPIO, out, readback, GPIOIN, in);
    } else {
        printf("set GPIO%02d = %d (readback %d)                        ", GPIO, out, readback);
    }

    if (pass) {
        printf("-> %spassed%s\n", COLORGRN, COLORDEF );
    } else {
        globalError=1;
        printf("-> %sfailed%s\n", COLORRED, COLORDEF );
    }
}


void CheckInversGPIO(int GPIO, int GPIOIN, int out) {
	CheckGPIO(GPIO, GPIOIN, out==HIGH ? LOW : HIGH);
}


void digitalWriteEx(int GPIO, int GPIOIN, int mode) {
    digitalWrite(GPIO, mode);
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


void CheckSameText(const char* msg, const char*  value, const char*  expect) {
    if (!strcmp(value, expect)) {
        printf("%39s (%10s==%10s) -> %spassed%s\n", msg, value, expect, COLORGRN, COLORDEF);
    } else {
        printf("%39s (%10s<>%10s) -> %sfailed%s\n", msg, value, expect, COLORRED, COLORDEF);
        globalError=1;
    }
}


void CheckSame(const char* msg, int value, int expect) {
    if (value==expect) {
        printf("%39s (% 3d==% 3d) -> %spassed%s\n", msg, value, expect, COLORGRN, COLORDEF);
    } else {
        printf("%39s (% 3d<>% 3d) -> %sfailed%s\n", msg, value, expect, COLORRED, COLORDEF);
        globalError=1;
    }
}


void CheckNotSame(const char* msg, int value, int expect) {
    if (value!=expect) {
        printf("%39s (% 3d<>% 3d) -> %spassed%s\n", msg, value, expect, COLORGRN, COLORDEF);
    } else {
        printf("%39s (% 3d==% 3d) -> %sfailed%s\n", msg, value, expect, COLORRED, COLORDEF);
        globalError=1;
    }
}


void CheckSameFloat(const char* msg, float value, float expect, float epsilon) {
    if (fabs(value-expect)<epsilon) {
        printf("%35s (%.3f==%.3f) -> %spassed%s \n", msg, value, expect, COLORGRN, COLORDEF);
    } else {
        printf("%35s (%.3f<>%.3f) -> %sfailed%s \n" , msg, value, expect, COLORRED, COLORDEF);
        globalError=1;
    }
}

void CheckSameFloatX(const char* msg, float value, float expect) {
  return CheckSameFloat(msg, value, expect, 0.08f);
}

void CheckSameDouble(const char* msg, double value, double expect, double epsilon) {
    if (fabs(value-expect)<epsilon) {
        printf("%35s (%.3f==%.3f) -> %spassed%s \n", msg, value, expect, COLORGRN, COLORDEF);
    } else {
        printf("%35s (%.3f<>%.3f) -> %sfailed%s \n" , msg, value, expect, COLORRED, COLORDEF);
        globalError=1;
    }
}


int UnitTestState() {
    printf("\n\nUNIT TEST STATE: ");
    if (globalError) {
        printf(" %sFAILED%s\n\n", FINALCOLRED, COLORDEF);
        return EXIT_FAILURE;
    } else {
        printf(" %sPASSED%s\n\n", FINALCOLGRN, COLORDEF);
        return EXIT_SUCCESS;
    }
}


void FailAndExitWithErrno(const char* msg, int ret) {
    printf("%s (Return=%d, Err: %s) -> %sfailed%s \n" , msg, ret, strerror(errno), COLORRED, COLORDEF);
    globalError=1;
    exit(UnitTestState());
}

