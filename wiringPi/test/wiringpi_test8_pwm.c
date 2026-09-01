// WiringPi test program: PWM test
// Compile: gcc -Wall wiringpi_test8_pwm.c -o wiringpi_test8_pwm -lwiringPi

#include "wpi_test.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

int PWM_OUT[4] = { 18, 12, 13, 19 };
int PWM_IN[4]  = { 17, 13, 12, 26 };

volatile int gCounter = 0;
int gFreqinPin = 0;
int gUseLegacy = 0;   // -legacy: old gCounter ISR instead of frequencyIn()

//Interrupt Service Routine for FREQIN (legacy method only)
void ISR_FREQIN(void) {
    gCounter++;
}

double MeasureAndCheckFreqTolerance(const char* msg, double expect_freq, int tolerance) {
  double fFrequency;
  clock_t CPUClockBegin, CPUClockEnd;
  double CPUClockInterval;
  double elapsed_time, CPULoad;
  uint64_t tbegin, tend;
  int SleepMs = 1200;

  CPUClockBegin = clock();
  tbegin = piMicros64();

  if (gUseLegacy) {
    int CountBegin, CountEnd;
    CountBegin = gCounter;
    delay(SleepMs);
    CountEnd = gCounter;
    tend = piMicros64();
    elapsed_time = (double)(tend-tbegin)/1.0e6;
    fFrequency = (CountEnd - CountBegin) / elapsed_time / 1000;
  } else {
    unsigned long long freqHz = frequencyIn(gFreqinPin, (unsigned long)SleepMs);
    tend = piMicros64();
    elapsed_time = (double)(tend-tbegin)/1.0e6;
    fFrequency = (double)freqHz / 1000.0;
  }

  CPUClockEnd = clock();
  CPUClockInterval = CPUClockEnd - CPUClockBegin;
  CPULoad = CPUClockInterval*100.0 / CLOCKS_PER_SEC / elapsed_time;

  printf("\nInterval:  time: %.6f sec (CPU: %3.1f %%) [%s] -> frequency: %.3f kHz\n",
    elapsed_time, CPULoad, gUseLegacy ? "legacy ISR" : "frequencyIn", fFrequency);

  CheckSameDouble("Wait for freq. meas.", elapsed_time, SleepMs/1000.0, 0.1); //100ms tolerance. maybe problematic on high freq/cpu load
  CheckSameDouble(msg, fFrequency, expect_freq, (expect_freq!=0.0) ? expect_freq*tolerance/100 : 0.1); //x% tolerance
  return fFrequency;
}

double MeasureAndCheckFreq(const char* msg, double expect_freq) {
 return MeasureAndCheckFreqTolerance(msg, expect_freq, 2);
}


int main (int argc, char *argv[]) {

    int major, minor;
    int PWM, FREQIN;

    gUseLegacy = (argc > 1 && strcmp(argv[1], "-legacy") == 0);

    wiringPiVersion(&major, &minor);

    printf("WiringPi PWM GPIO test program 8\n");
    printf("PWM/ISR test (WiringPi %d.%d)\n", major, minor);
    printf("Frequency measurement method: %s\n", gUseLegacy ? "legacy gCounter ISR (-legacy)" : "frequencyIn()");

    wiringPiSetupGpio() ;

    int rev, mem, maker, overVolted, RaspberryPiModel;
    piBoardId(&RaspberryPiModel, &rev, &mem, &maker, &overVolted);
    CheckNotSame("Model: ", RaspberryPiModel, -1);

    PWM = 18;
    FREQIN = 17;
    gFreqinPin = FREQIN;

    if (gUseLegacy) {
      printf("Register ISR@%d\n", PWM);
      // INT_EDGE_BOTH, INT_EDGE_FALLING, INT_EDGE_RISING only one ISR per input
      int result = wiringPiISR(FREQIN, INT_EDGE_RISING, &ISR_FREQIN);
      CheckSame("Register ISR", result, 0);
      if (result < 0) {
          printf("Unable to setup ISR for GPIO %d (%s)\n\n", FREQIN, strerror(errno));
          return UnitTestState();
      }
    }

    printf("\n==> Set pwm 0%% and enable PWM output with PWM_OUTPUT (default mode)\n");
    pwmWrite(PWM, 0);  // <--  Allways start with 0 Hz
    pinMode(PWM, PWM_OUTPUT);  //Mode BAL, pwmr=1024, pwmc=32
    delay(250);
    double duty_fact = 0.0;
    double freq = 0.0;
    MeasureAndCheckFreq("PMW Pi0-4:BAL/Pi5:MS without change", freq);

    printf("Keep pwm 0%% and set mode MS\n");
    pwmSetMode(PWM_MODE_MS);
    delay(250);
    MeasureAndCheckFreq("PWM MS without change", freq);

    int pwmc;
    int pwmr;
    int pwm;

    if (!piRP1Model()) {
      pwmSetMode(PWM_MODE_BAL);
      pwmc  = 2000;
      pwmr  = 1024;
      pwm   =  512;
      duty_fact = (double)pwm/(double)pwmr;
      printf("\n==> set mode BAL, pwmc=%d, pwmr=%d, pwm=%d, duty=%g%%\n", pwmc, pwmr, pwm, duty_fact*100);
      pwmSetClock(pwmc);
      pwmSetRange(pwmr);
      pwmWrite(PWM, pwm);
      delay(250);
      freq = 19200.0/pwmc*duty_fact;
      MeasureAndCheckFreq("PWM BAL with settings", freq);
    }

    pwmSetMode(PWM_MODE_MS);
    pwmc  = 10;
    pwmr  = 256;
    pwm   =  171;
    duty_fact = (double)pwm/(double)pwmr;
    printf("\n==> set mode MS, pwmc=%d, pwmr=%d, pwm=%d, duty=%g%%\n", pwmc, pwmr, pwm, duty_fact*100);
    pwmSetClock(pwmc);
    pwmSetRange(pwmr);
    pwmWrite(PWM, pwm);
    delay(250);
    freq = 19200.0/(double)pwmc/(double)pwmr;
    MeasureAndCheckFreq("PWM BAL with settings", freq);

    printf("set PWM@GPIO%d (output) off\n", PWM);
    pinMode(PWM, PM_OFF);
    delay(1000);
    MeasureAndCheckFreq("PMW off", 0.0);

    if (!piRP1Model()) {
      pwmc  = 800;
      pwmr  = 2048;
      pwm   =  768;
      duty_fact = (double)pwm/(double)pwmr;
      printf("\n==> set mode PWM_BAL_OUTPUT, pwmc=%d, pwmr=%d, pwm=%d, duty=%g%%\n", pwmc, pwmr, pwm, duty_fact*100);
      pwmSetRange(pwmr);
      pwmSetClock(pwmc);
      pwmWrite(PWM, pwm);
      pinMode(PWM, PWM_BAL_OUTPUT);
      delay(250);
      freq = 19200.0/pwmc*duty_fact;
      MeasureAndCheckFreq("PMW BAL start values", freq);
    }

    printf("set PWM@GPIO%d (output) off\n", PWM);
    pinMode(PWM, PM_OFF);
    delay(1000);
    MeasureAndCheckFreq("PMW off", 0.0);

    printf("Set pwm settings and enable PWM\n");
    //pwmc  = 5;  //Problem with Pi0/1 after setting, PWM stops working, maybe IRQ problem or PWM BAL with that high freq (>2 MHz)
    pwmc  = 35;   //PWM BAL would be >400 kHz
    pwmr  = 1024;
    pwm   =  768;
    duty_fact = (double)pwm/(double)pwmr;
    printf("\n==> set mode PWM_MS_OUTPUT, pwmc=%d, pwmr=%d, pwm=%d, duty=%g%%\n", pwmc, pwmr, pwm, duty_fact*100);
    pwmSetRange(pwmr);
    pwmSetClock(pwmc);
    pwmWrite(PWM, pwm);
    pinMode(PWM, PWM_MS_OUTPUT);
    delay(250);
    freq = 19200.0/(double)pwmc/(double)pwmr;
    MeasureAndCheckFreq("PMW MS start values", freq);

    printf("set PWM@GPIO%d (output) off\n", PWM);
    pinMode(PWM, PM_OFF);
    delay(1000);
    MeasureAndCheckFreq("PMW off", 0.0);

    printf("set PWM0 CLK off @ Pi5\n");
    pwmSetClock(0);

    if (gUseLegacy) {
      int result = wiringPiISRStop(FREQIN);
      CheckSame("\n\nRelease ISR", result, 0);
      if (result < 0) {
        printf("Unable to release ISR for GPIO %d (%s)\n\n", FREQIN, strerror(errno));
        return UnitTestState();
      }
    }

    return UnitTestState();
}
