// WiringPi test program: PWM range change test
// Missing is missing channel 0 & 1 @ Pi5 , pin 12, 13 missing too
// Compile: gcc -Wall wiringpi_test81_pwm.c -o wiringpi_test81_pwm -lwiringPi

#include "wpi_test.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

#define CHS 4

int PWM_OUT[CHS] = { 18, 12, 13, 19 };
int PWM_IN[CHS]  = { 17, 13, 12, 26 };

volatile int gCounter[CHS] = { 0, 0, 0, 0 };

//Interrupt Service Routine for FREQIN
void ISR_FREQIN0(void) {
    gCounter[0]++;
}

void ISR_FREQIN1(void) {
    gCounter[1]++;
}

void ISR_FREQIN2(void) {
    gCounter[2]++;
}

void ISR_FREQIN3(void) {
    gCounter[3]++;
}

void MeasureAndCheckFreqTolerance(const char* msg, double expect_freq[2], int tolerance) {
  double fFrequency[CHS];
  clock_t CPUClockBegin, CPUClockEnd;
  int CountBegin[CHS], CountEnd[CHS];
  double CPUClockInterval, CountInterval[CHS];
  double elapsed_time, CPULoad;
  uint64_t tbegin, tend;
  int SleepMs = 1200;

  CPUClockBegin = clock();
  tbegin = piMicros64();
  CountBegin[0] = gCounter[0];
  CountBegin[1] = gCounter[1];
  delay(SleepMs);
  CountEnd[0] = gCounter[0];
  CountEnd[1] = gCounter[1];
  CPUClockEnd = clock();
  tend = piMicros64();

  elapsed_time = (double)(tend-tbegin)/1.0e6;
  CountInterval[0] = CountEnd[0] - CountBegin[0];
  CountInterval[1] = CountEnd[1] - CountBegin[1];
  CPUClockInterval = CPUClockEnd - CPUClockBegin;
  CPULoad = CPUClockInterval*100.0 / CLOCKS_PER_SEC / elapsed_time;
  fFrequency[0] = CountInterval[0] / elapsed_time / 1000;
  fFrequency[1] = CountInterval[1] / elapsed_time / 1000;

  printf("\nInterval:  time: %.6f sec (CPU: %3.1f %%), count 0: %g  -> frequency 0: %.3f kHz\n",
    elapsed_time, CPULoad, CountInterval[0], fFrequency[0]);
  printf("Interval:  time: %.6f sec (CPU: %3.1f %%), count 1: %g  -> frequency 1: %.3f kHz\n",
    elapsed_time, CPULoad, CountInterval[1], fFrequency[1]);

  CheckSameDouble("Wait for freq. meas.", elapsed_time, SleepMs/1000.0, 0.1); //100ms tolerance. maybe problematic on high freq/cpu load
  CheckSameDouble(msg, fFrequency[0], expect_freq[0], (expect_freq[0]!=0.0) ? expect_freq[0]*tolerance/100 : 0.1); //x% tolerance
  CheckSameDouble(msg, fFrequency[1], expect_freq[1], (expect_freq[1]!=0.0) ? expect_freq[1]*tolerance/100 : 0.1); //x% tolerance
}


void MeasureAndCheckFreq(const char* msg, double expect_freq[2]) {
 MeasureAndCheckFreqTolerance(msg, expect_freq, 2);
}


void CalcFreq(double* freq, int pwmc, int* pwmr) {
  freq[0] = 19200.0/(double)pwmc/(double)pwmr[0];
  if (piBoard40Pin()) {
    freq[1] = 19200.0/(double)pwmc/(double)pwmr[1];
  }
}


int main (void) {

    int major, minor;
    int PWM[2], FREQIN[2], PWMCH[2];
  

    wiringPiVersion(&major, &minor);

    printf("WiringPi PWM test program 8.1 (Set Range channel & pin)\n");
    printf("PWM/ISR test (WiringPi %d.%d)\n", major, minor);

    wiringPiSetupGpio() ;

    int rev, mem, maker, overVolted, RaspberryPiModel;
    piBoardId(&RaspberryPiModel, &rev, &mem, &maker, &overVolted);
    CheckNotSame("Model: ", RaspberryPiModel, -1);

    PWM[0] = 18;
    FREQIN[0] = 17;
    PWM[1] = 19;
    FREQIN[1] = 26;
    if (piRP1Model()) {
      PWMCH[0] = 2; //Pin 18 use channel 2
      PWMCH[1] = 3; //Pin 19 use channel 3
    } else {
      PWMCH[0] = 0; //Pin 18 use channel 0
      PWMCH[1] = 1; //Pin 19 use channel 1
    }

    printf("Register PWM channel 0 ISR@%d\n", PWM[0]);
    // INT_EDGE_BOTH, INT_EDGE_FALLING, INT_EDGE_RISING only one ISR per input
    int result = wiringPiISR(FREQIN[0], INT_EDGE_RISING, &ISR_FREQIN0);
    CheckSame("Register ISR", result, 0);
    if (result < 0) {
        printf("Unable to setup ISR for GPIO %d (%s)\n\n", FREQIN[0], strerror(errno));
        return UnitTestState();
    }

    printf("Register PWM channel 1 ISR@%d\n", PWM[1]);
    // INT_EDGE_BOTH, INT_EDGE_FALLING, INT_EDGE_RISING only one ISR per input
    result = wiringPiISR(FREQIN[1], INT_EDGE_RISING, &ISR_FREQIN1);
    CheckSame("Register ISR", result, 0);
    if (result < 0) {
        printf("Unable to setup ISR for GPIO %d (%s)\n\n", FREQIN[1], strerror(errno));
        return UnitTestState();
    }

    printf("\n==> Set pwm 0%% and enable PWM output with PWM_OUTPUT (default mode)\n");
    pwmWrite(PWM[0], 0);  // <--  Allways start with 0 Hz
    pwmWrite(PWM[1], 0);
    pinMode(PWM[0], PWM_OUTPUT);  //Mode BAL, pwmr=1024, pwmc=32
    pinMode(PWM[1], PWM_OUTPUT);  
    delay(10);
    double duty_fact[2] = { 0.0, 0.0 };
    double freq[2] = { 0.0, 0.0 };
    MeasureAndCheckFreq("PMW Pi0-4:BAL/Pi5:MS without change", freq);

    printf("Keep pwm 0%% and set mode MS\n");
    pwmSetMode(PWM_MODE_MS);
    delay(10);
    MeasureAndCheckFreq("PWM MS without change", freq);

    int pwmc = 10;
    int pwm = 171;
    int pwmr[2];

    pwmSetMode(PWM_MODE_MS);
    pwmr[0] = 256;
    pwmr[1] = 512;
    duty_fact[0] = (double)pwm/(double)pwmr[0];
    duty_fact[1] = (double)pwm/(double)pwmr[1];
    printf("\n==> set mode MS, ch0, pwmc=%d, pwmr=%d, pwm=%d, duty=%g%%\n", pwmc, pwmr[0], pwm, duty_fact[0]*100);
    printf("==> set mode MS, ch1, pwmc=%d, pwmr=%d, pwm=%d, duty=%g%%\n", pwmc, pwmr[1], pwm, duty_fact[1]*100);
    delay(10);

    pwmSetClock(pwmc);
    pwmSetChannelRange(PWMCH[0], pwmr[0]);
    pwmSetChannelRange(PWMCH[1], pwmr[1]);
    pwmWrite(PWM[0], pwm);
    pwmWrite(PWM[1], pwm);
    CalcFreq(freq, pwmc, pwmr);
    delay(10);
    MeasureAndCheckFreq("PWM pwmSetChannelRange", freq);

    pwmr[0] *= 2;
    pwmr[1] *= 2;
    pwmSetChannelRange(PWMCH[0], pwmr[0]);
    pwmSetChannelRange(PWMCH[1], pwmr[1]);
    CalcFreq(freq, pwmc, pwmr);
    delay(10);
    MeasureAndCheckFreq("PWM pwmSetChannelRange", freq);

    pwmr[0] *= 2;
    pwmr[1] *= 2;
    pwmSetPinRange(PWM[0], pwmr[0]);
    pwmSetPinRange(PWM[1], pwmr[1]);
    CalcFreq(freq, pwmc, pwmr);
    delay(10);
    MeasureAndCheckFreq("PWM pwmSetPinRange", freq);

    pwmr[0] *= 2;
    pwmr[1] *= 2;
    pwmSetPinRange(PWM[0], pwmr[0]);
    pwmSetPinRange(PWM[1], pwmr[1]);
    CalcFreq(freq, pwmc, pwmr);
    delay(10);
    MeasureAndCheckFreq("PWM pwmSetPinRange", freq);


    freq[0] = 0.0;
    freq[1] = 0.0;
    printf("set PWM@GPIO%d (output) off\n", PWM[0]);
    pinMode(PWM[0], PM_OFF);
    printf("set PWM@GPIO%d (output) off\n", PWM[1]);
    pinMode(PWM[1], PM_OFF);
    delay(250);
    MeasureAndCheckFreq("PMW off", freq);

    printf("set PWM0 CLK off @ Pi5\n");
    pwmSetClock(0);

    result = wiringPiISRStop(FREQIN[0]);
    CheckSame("\n\nRelease ISR", result, 0);
    if (result < 0) {
      printf("Unable to release ISR for GPIO %d (%s)\n\n", FREQIN[0], strerror(errno));
      return UnitTestState();
    }
    result = wiringPiISRStop(FREQIN[1]);
    CheckSame("\n\nRelease ISR", result, 0);
    if (result < 0) {
      printf("Unable to release ISR for GPIO %d (%s)\n\n", FREQIN[1], strerror(errno));
      return UnitTestState();
    }

    return UnitTestState();
}
