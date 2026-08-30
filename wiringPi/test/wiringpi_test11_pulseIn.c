// WiringPi test program: 11 pulseIn, ISR
// Compile: gcc -Wall wiringpi_test11_pulseIn.c -o wiringpi_test11_pulseIn -lwiringPi

#include "wpi_test.h"
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>


int pinOut = 19;
int pinIn = 26;

int pulse_time_high0_us = 38000;
int pulse_time_low_us   = 48000;
int pulse_time_high_us  = 28000;


void* pulse_generator(void* arg) {
    printf("Start Pulse...\n");
    delay(100);
    printf("H (38ms)\n");
    digitalWrite(pinOut, HIGH);
    delayMicroseconds(pulse_time_high0_us); // 38 ms

    printf("L (48ms)\n");
    digitalWrite(pinOut, LOW);
    delayMicroseconds(pulse_time_low_us); // 48 ms LOW (pulseIn time)

    printf("H (28ms)\n");
    digitalWrite(pinOut, HIGH);
    delayMicroseconds(pulse_time_high_us); // 28 ms HIGH (pulseIn time)
    printf("L\n");
    digitalWrite(pinOut, LOW);

    return NULL;
}


unsigned long pulseInPoll(int pin, int level, unsigned long timeout) {
  unsigned long startTime = micros();
  //0. missed pulse start - prevents bad readings
  while (digitalRead(pin) == level) {
    if ((micros() - startTime) > timeout) {
        printf("timeout 1");
        return 0;
    }
  }
  // 1. Wait for pulse to start.
  while (digitalRead(pin) != level) {
      if ((micros() - startTime) > timeout) {
        printf("timeout 2");
        return 0;
      }
  }
  unsigned long pulseStart = micros();
  // 2. Wait for pulse to end.
  while (digitalRead(pin) == level) {
      if ((micros() - startTime) > timeout*4) {
        printf("timeout 2");
        return 0;
      }
  }
  unsigned long pulseEnd = micros();
  return pulseEnd - pulseStart;
}


int main(void)
{
	int major=0, minor=0;

	wiringPiVersion(&major, &minor);
	printf("WiringPi GPIO test program 11 (using GPIO%d (output) and GPIO%d (input))\n", pinOut, pinIn);
	printf("pulseIn test (WiringPi %d.%d)\n", major, minor);

	wiringPiSetupGpio();

    int _is40pin = piBoard40Pin();
    CheckNotSame("40-Pin board: ", _is40pin, -1);
    if (_is40pin==0) {
        printf("Old 28pin system\n");
        pinOut = 17;
        pinIn = 18;
    }

    int rev, mem, maker, overVolted, RaspberryPiModel;
    piBoardId(&RaspberryPiModel, &rev, &mem, &maker, &overVolted);
    CheckNotSame("Model: ", RaspberryPiModel, -1);

    int toleranceTimeout_us = 20000;  //22ms
    float tolerancePulseIn = 0.012;
    switch(RaspberryPiModel) {
        case PI_MODEL_A:
        case PI_MODEL_B:     //ARM=800MHz
        case PI_MODEL_BP:
        case PI_MODEL_AP:
        case PI_MODEL_CM:
        case PI_MODEL_ZERO:
        case PI_MODEL_ZERO_W: //ARM=1000MHz
            tolerancePulseIn = 0.09;
            break;
        case PI_MODEL_2:
            tolerancePulseIn = 0.06;
            break;
        case PI_MODEL_3B:
        case PI_MODEL_3AP:
        case PI_MODEL_3BP:
        case PI_MODEL_ZERO_2W:
        case PI_MODEL_CM0:
            tolerancePulseIn = 0.02;
            break;
    }
    printf("Use tolerance of %g %%\n", tolerancePulseIn*100);


    pinMode(pinOut, OUTPUT);
    pinMode(pinIn, INPUT);

    const uint64_t timeout_ns      = 2000000000ULL; // 2 s
    const uint64_t timeoutshort_ns =  330000000ULL; // 0.33 s
    const unsigned int timeout_us      = timeout_ns/1000;
    const unsigned int timeoutshort_us = timeoutshort_ns/1000;
    uint64_t duration_ns;
    unsigned int duration_us;
    unsigned int start_time;
    int time_us;
    pthread_t th;

    // nanoseconds test
    digitalWrite(pinOut, HIGH);
    sleep(1);

    pthread_create(&th, NULL, pulse_generator, NULL);
    printf("Wait for pulse high...\n");
    duration_ns = pulseInNS(pinIn, HIGH, timeout_ns);
    CheckAlmostSameX("pulseInNS()", duration_ns, pulse_time_high_us*1000, tolerancePulseIn);
    pthread_join(th, NULL);
    sleep(1);

    pthread_create(&th, NULL, pulse_generator, NULL);
    printf("Wait for pulse low...\n");
    duration_ns = pulseInNS(pinIn, LOW, timeout_ns);
    CheckAlmostSameX("pulseInNS()", duration_ns, pulse_time_low_us*1000, tolerancePulseIn);
    pthread_join(th, NULL);
    sleep(1);

    pthread_create(&th, NULL, pulse_generator, NULL);
    printf("Wait for pulse high...\n");
    duration_ns = pulseInNS(pinIn, HIGH, timeout_ns);
    CheckAlmostSameX("pulseInNS()", duration_ns, pulse_time_high0_us*1000, tolerancePulseIn);
    pthread_join(th, NULL);
    sleep(1);

    start_time = micros();
    printf("Wait for time out pulse ...\n");
    duration_ns = pulseInNS(pinIn, LOW, timeoutshort_ns);
    time_us = (micros() - start_time);
    CheckSame("pulseInNS()", duration_ns, 0);
    CheckBetween("pulseInNS() timeout us", time_us, timeoutshort_ns/1000, timeoutshort_ns/1000+toleranceTimeout_us);
    sleep(1);

    // microseconds test
    digitalWrite(pinOut, HIGH);
    sleep(1);

    pthread_create(&th, NULL, pulse_generator, NULL);
    start_time = micros();
    printf("Wait for pulse high ...\n");
    duration_us = pulseIn(pinIn, HIGH, timeout_us);
    CheckAlmostSameX("pulseIn()", duration_us, pulse_time_high_us, tolerancePulseIn);
    pthread_join(th, NULL);
    sleep(1);

    pthread_create(&th, NULL, pulse_generator, NULL);
    start_time = micros();
    printf("Wait for pulse low ...\n");
    duration_us = pulseIn(pinIn, LOW, timeout_us);
    CheckAlmostSameX("pulseIn()", duration_us, pulse_time_low_us, tolerancePulseIn);
    pthread_join(th, NULL);
    sleep(1);

    pthread_create(&th, NULL, pulse_generator, NULL);
    start_time = micros();
    printf("Wait for pulse high ...\n");
    duration_us = pulseIn(pinIn, HIGH, timeout_us);
    CheckAlmostSameX("pulseIn()", duration_us, pulse_time_high0_us, tolerancePulseIn);
    pthread_join(th, NULL);
    sleep(1);

    start_time = micros();
    printf("Wait for time out pulse ...\n");
    duration_us = pulseIn(pinIn, LOW, timeoutshort_us);
    time_us = (micros() - start_time);
    CheckSame("pulseIn()", duration_us, 0);
    CheckBetween("pulseIn() timeout us", time_us, timeoutshort_us, timeoutshort_us+toleranceTimeout_us);
    sleep(1);

    // old version

    digitalWrite(pinOut, HIGH);
    sleep(1);
    pthread_create(&th, NULL, pulse_generator, NULL);
    printf("Wait for pulse high...\n");
    duration_us = pulseInPoll(pinIn, HIGH, timeout_us);
    CheckAlmostSameX("pulseInPoll()", (int)duration_us, pulse_time_high_us, tolerancePulseIn);
    pthread_join(th, NULL);
    sleep(1);

    pthread_create(&th, NULL, pulse_generator, NULL);
    printf("Wait for pulse low...\n");
    duration_us = pulseInPoll(pinIn, LOW, timeout_us);
    CheckAlmostSameX("pulseInPoll()", (int)duration_us, pulse_time_low_us, tolerancePulseIn);
    pthread_join(th, NULL);

    pthread_create(&th, NULL, pulse_generator, NULL);
    printf("Wait for pulse high...\n");
    duration_us = pulseInPoll(pinIn, HIGH, timeout_us);
    CheckAlmostSameX("pulseInPoll()", (int)duration_us, pulse_time_high0_us, tolerancePulseIn);
    pthread_join(th, NULL);
    sleep(1);


    return 0;
}

