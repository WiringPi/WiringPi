// WiringPi test program: 11 pulseIn, ISR
// Compile: gcc -Wall wiringpi_test11_pulseIn.c -o wiringpi_test11_pulseIn -lwiringPi

#include "wpi_test.h"
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>


int pinOut = 19;
int pinIn = 26;


void* pulse_generator(void* arg) {
    printf("Start Pulse...\n");
    delay(100);
    printf("H (38ms)\n");
    digitalWrite(pinOut, HIGH);
    delayMicroseconds(38000); // 38 ms

    printf("L (48ms)\n");
    digitalWrite(pinOut, LOW);
    delayMicroseconds(48000); // 48 ms LOW (pulseIn time)

    printf("H (28ms)\n");
    digitalWrite(pinOut, HIGH);
    delayMicroseconds(28000); // 28 ms HIGH (pulseIn time)
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

    float tolerancePulseIn = 0.01;
    switch(RaspberryPiModel) {
        case PI_MODEL_A:
        case PI_MODEL_B:     //ARM=800MHz
        case PI_MODEL_BP:
        case PI_MODEL_AP:
        case PI_MODEL_CM:
        case PI_MODEL_ZERO:
        case PI_MODEL_ZERO_W: //ARM=1000MHz
            tolerancePulseIn = 0.08;
            break;
        case PI_MODEL_2:
            tolerancePulseIn = 0.06;
            break;
        case PI_MODEL_3B:
        case PI_MODEL_3AP:
        case PI_MODEL_3BP:
        case PI_MODEL_ZERO_2W:
            tolerancePulseIn = 0.02;
            break;
    }
    printf("Use tolerance of %g %%\n", tolerancePulseIn*100);


    pinMode(pinOut, OUTPUT);
    pinMode(pinIn, INPUT);

    digitalWrite(pinOut, HIGH);

    const uint64_t timeout_ns = 2000000000ULL; // 6s Timeout
    const unsigned int timeout_ms = timeout_ns/1000;
    uint64_t duration_ns;
    unsigned int duration_ms;
    pthread_t th;
    pthread_create(&th, NULL, pulse_generator, NULL);
    printf("Wait for pulse high...\n");
    duration_ns = pulseIn64(pinIn, HIGH, timeout_ns);
    CheckAlmostSameX("pulseIn()", duration_ns, 28000000, tolerancePulseIn);
    pthread_join(th, NULL);

    sleep(1);
    pthread_create(&th, NULL, pulse_generator, NULL);
    printf("Wait for pulse low...\n");
    duration_ns = pulseIn64(pinIn, LOW, timeout_ns);
    CheckAlmostSameX("pulseIn()", duration_ns, 48000000, tolerancePulseIn);
    pthread_join(th, NULL);

    sleep(1);
    const uint64_t timeoutshort_ns = 2000000ULL; // 2s Timeout
    unsigned int start_time = micros();
    printf("Wait for time out pulse ...\n");
    duration_ns = pulseIn64(pinIn, LOW, timeoutshort_ns);
    int time_ms = (micros() - start_time)/1000;
    CheckSame("pulseIn()", duration_ns, 0);
    CheckAlmostSame("pulseIn() timeout us", timeoutshort_ns/1000, time_ms);

    digitalWrite(pinOut, HIGH);
    sleep(1);
    pthread_create(&th, NULL, pulse_generator, NULL);
    printf("Wait for pulse high...\n");
    duration_ms = pulseInPoll(pinIn, HIGH, timeout_ms);
    CheckAlmostSameX("pulseInPoll()", (int)duration_ms, 28000, tolerancePulseIn);
    pthread_join(th, NULL);

    sleep(1);
    pthread_create(&th, NULL, pulse_generator, NULL);
    printf("Wait for pulse low...\n");
    duration_ms = pulseInPoll(pinIn, LOW, timeout_ms);
    CheckAlmostSameX("pulseInPoll()", (int)duration_ms, 48000, tolerancePulseIn;
    pthread_join(th, NULL);

    return 0;
}

