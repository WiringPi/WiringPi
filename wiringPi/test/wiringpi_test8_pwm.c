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

//Interrupt Service Routine for FREQIN
void ISR_FREQIN(void) {
    gCounter++;
}

double MeasureAndCheckFreq(const char* msg, double expect_freq) {
  double fFrequency;
  clock_t CPUClockBegin, CPUClockEnd;
  int CountBegin, CountEnd;
  double CPUClockInterval, CountInterval;
  double elapsed_time, CPULoad;
  uint64_t tbegin, tend;
  int SleepMs = 1200;

  CPUClockBegin = clock();
  tbegin = piMicros64();
  CountBegin = gCounter;
  delay(SleepMs);
  CountEnd = gCounter;
  CPUClockEnd = clock();
  tend = piMicros64();

  elapsed_time = (double)(tend-tbegin)/1.0e6;
  CountInterval = CountEnd - CountBegin;
  CPUClockInterval = CPUClockEnd - CPUClockBegin;
  CPULoad = CPUClockInterval*100.0 / CLOCKS_PER_SEC / elapsed_time;
  fFrequency = CountInterval / elapsed_time / 1000;

  printf("\nInterval:  time: %.6f sec (CPU: %3.1f %%), count: %g  -> frequency: %.3f kHz\n",
    elapsed_time, CPULoad, CountInterval, fFrequency);

  CheckSameDouble("Wait for freq. meas.", elapsed_time, SleepMs/1000.0, 0.1); //100ms tolerance. maybe problematic on high freq/cpu load
  CheckSameDouble(msg, fFrequency, expect_freq, (expect_freq!=0.0) ? expect_freq*2/100:0.1); //2% tolerance
  return fFrequency;
}


int main (void) {

    int major, minor;
//    char msg[255];
    int PWM, FREQIN;

    wiringPiVersion(&major, &minor);

    printf("WiringPi PWM GPIO test program 8\n");
    printf("PWM/ISR test (WiringPi %d.%d)\n", major, minor);

    wiringPiSetupGpio() ;

    int rev, mem, maker, overVolted, RaspberryPiModel;
    piBoardId(&RaspberryPiModel, &rev, &mem, &maker, &overVolted);
    CheckNotSame("Model: ", RaspberryPiModel, -1);
    switch(RaspberryPiModel) {
      case PI_MODEL_5:
        return UnitTestState();  //not supported so far
    }

    PWM = 18;
    FREQIN = 17;

    printf("Register ISR@%d\n", PWM);
    // INT_EDGE_BOTH, INT_EDGE_FALLING, INT_EDGE_RISING only one ISR per input
    int result = wiringPiISR(FREQIN, INT_EDGE_RISING, &ISR_FREQIN);
    CheckSame("Register ISR", result, 0);
    if (result < 0) {
        printf("Unable to setup ISR for GPIO %d (%s)\n\n", FREQIN, strerror(errno));
        return UnitTestState();
    }

    printf("Set pwm 0%% and enable PWM output\n");
    pwmWrite(PWM, 0);  // <--  Allways start with 0 Hz
    pinMode(PWM, PWM_OUTPUT);  //Mode BAL, pwmr=1024, pwmc=32
    delay(250);
    double duty_fact = 0.0;
    double freq = 0.0;
    MeasureAndCheckFreq("PMW BAL without change", freq);

    pwmSetMode(PWM_MODE_MS);
    delay(250);
    MeasureAndCheckFreq("PWM MS without change", freq);

    // Now Change values 
    pwmSetMode(PWM_MODE_BAL);
    int pwmc  = 1000;
    int pwmr  = 1024;
    int pwm   =  512;
    pwmSetClock(pwmc);
    pwmSetRange(pwmr);
    pwmWrite(PWM, pwm);
    delay(250);   
    duty_fact = (double)pwm/(double)pwmr;
    printf("duty factor: %g\n", duty_fact);
    freq = 19200.0/pwmc*duty_fact;
    MeasureAndCheckFreq("PWM BAL with settings", freq);

    pwmSetMode(PWM_MODE_MS);
    pwmc  = 10;
    pwmr  = 256;
    pwm   =  171;
    pwmSetClock(pwmc);
    pwmSetRange(pwmr);
    pwmWrite(PWM, pwm);
    delay(250);   
    duty_fact = (double)pwm/(double)pwmr;
    printf("duty factor: %g\n", duty_fact);
    freq = 19200.0/(double)pwmc/(double)pwmr;
    MeasureAndCheckFreq("PWM BAL with settings", freq);

    printf("set PWM@GPIO%d (output) off\n", PWM);
    pinMode(PWM, PM_OFF);
    delay(1000); 
    MeasureAndCheckFreq("PMW off", 0.0);

    printf("Set pwm settings and enable PWM\n");
    pwmc  = 800;
    pwmr  = 2048;
    pwm   =  768;
    pwmSetRange(pwmr);
    pwmSetClock(pwmc);
    pwmWrite(PWM, pwm); 
    pinMode(PWM, PWM_BAL_OUTPUT);
    delay(250); 
    duty_fact = (double)pwm/(double)pwmr;
    printf("duty factor: %g\n", duty_fact);
    freq = 19200.0/pwmc*duty_fact;
    MeasureAndCheckFreq("PMW BAL start values", freq);

    printf("set PWM@GPIO%d (output) off\n", PWM);
    pinMode(PWM, PM_OFF);
    delay(1000); 
    MeasureAndCheckFreq("PMW off", 0.0);

    printf("Set pwm settings and enable PWM\n");
    pwmc  = 5;
    pwmr  = 1024;
    pwm   =  768;
    pwmSetRange(pwmr);
    pwmSetClock(pwmc);
    pwmWrite(PWM, pwm); 
    pinMode(PWM, PWM_MS_OUTPUT);
    delay(250); 
    duty_fact = (double)pwm/(double)pwmr;
    printf("duty factor: %g\n", duty_fact);
    freq = 19200.0/(double)pwmc/(double)pwmr;
    MeasureAndCheckFreq("PMW MS start values", freq);

    result = wiringPiISRStop(FREQIN);
    CheckSame("\n\nRelease ISR", result, 0);
    if (result < 0) {
      printf("Unable to release ISR for GPIO %d (%s)\n\n", FREQIN, strerror(errno));
      return UnitTestState();
    }

    return UnitTestState();
}