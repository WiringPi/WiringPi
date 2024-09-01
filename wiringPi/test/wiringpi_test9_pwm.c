// WiringPi test program: PWM test
// Compile: gcc -Wall wiringpi_test9_pwm.c -o wiringpi_test9_pwm -lwiringPi

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
  CheckSameDouble(msg, fFrequency, expect_freq, expect_freq*2/100); //2% toleranc
  return fFrequency;
}


int tests_pwmc[7] = {1456, 1000,  512,  200, 2000, 3000, 4000};
int tests_duty[7] = { 512,  768,  682,  922,  256,  341,  102};
int tests_pwmr[12]= {  50,  100,  200,  512, 1024, 1456, 2000, 3000, 5000, 10000, 15000, 20000};
int tests_pwm[3]  = {  50,  25,  75};

int main (void) {

    int major, minor;
    char msg[255];
    int testruns = 4;
    int PWM, FREQIN;

    wiringPiVersion(&major, &minor);

    printf("WiringPi GPIO test program 9\n");
    printf("PWM/ISR test (WiringPi %d.%d)\n", major, minor);

    wiringPiSetupGpio() ;

    int rev, mem, maker, overVolted, RaspberryPiModel;
    piBoardId(&RaspberryPiModel, &rev, &mem, &maker, &overVolted);
    CheckNotSame("Model: ", RaspberryPiModel, -1);
    int Pi4 = 0;
    int Pi5 = 0;
    double MaxFreq = 100.0;
    switch(RaspberryPiModel) {
      case PI_MODEL_A:
      case PI_MODEL_B:
      case PI_MODEL_BP:
      case PI_MODEL_AP:
      case PI_MODEL_ZERO:
      case PI_MODEL_ZERO_W:
      case PI_MODEL_CM:
        MaxFreq = 13.0; // 12.5 kHz -> ~40% CPU@800 MHz
        printf(" - Pi1/BCM2835 detected, will skip tests with frequency above %g kHz\n", MaxFreq);
        break;
      case PI_MODEL_2:
        MaxFreq = 20.0;
        printf(" - Pi2/BCM2836 detected, will skip tests with frequency above %g kHz\n", MaxFreq);
        break;
      case PI_MODEL_3B:
      case PI_MODEL_CM3:
      case PI_MODEL_3BP:
      case PI_MODEL_3AP:
      case PI_MODEL_CM3P:
      case PI_MODEL_ZERO_2W:
        MaxFreq = 50.0;
        printf(" - Pi3/BCM2837 detected, will skip tests with frequency above %g kHz\n", MaxFreq);
        break;
      case PI_MODEL_4B:
      case PI_MODEL_400:
      case PI_MODEL_CM4:
      case PI_MODEL_CM4S:
        Pi4 = 1;
        break;
      case PI_MODEL_5:
         Pi5 = 1;
         break;
    }

    if (!piBoard40Pin()) {
        testruns = 1;  // only fist PWM0, supported
    }

    for (int testrun=0; testrun<testruns; testrun++) {
      PWM = PWM_OUT[testrun];
      FREQIN = PWM_IN[testrun];
      printf("using PWM@GPIO%d (output) and GPIO%d (input)\n", PWM, FREQIN);
      delay(1000);
      printf("\n");
      printf("*********************************\n");
      printf("*          PWM BAL mode         *\n");
      printf("*********************************\n");
      const int pmw = 512;
      int pmwr = 1024; //default!

      printf("Set pwm 50%% and enable PWM output (600 kHz?) \n");
      pwmWrite(PWM, pmw);  //50% Duty
      pinMode(PWM, PWM_OUTPUT);  //Mode BAL, pwmr=1024, pwmc=32
      printf("pwmc 4.8kHz\n");
      pwmSetClock(2000);
      delay(250);

      printf("Register ISR@%d\n", PWM);
    // INT_EDGE_BOTH, INT_EDGE_FALLING, INT_EDGE_RISING only one ISR per input
      int result = wiringPiISR(FREQIN, INT_EDGE_RISING, &ISR_FREQIN);
      CheckSame("Register ISR", result, 0);
      if (result < 0) {
        printf("Unable to setup ISR for GPIO %d (%s)\n\n", FREQIN, strerror(errno));
        return UnitTestState();
      }
      printf("Wait for start ...\n");
      delay(500);
      printf("Start:\n");
      //MeasureAndCheckFreq("50\% Duty (default)", 300.000);   //FAIL , freq (pwmc=32) to high for irq count
      if(!Pi5) {
      for (int c_duty=0, c_duty_end = sizeof(tests_duty)/sizeof(tests_duty[0]); c_duty<c_duty_end; c_duty++) {
        double tests_duty_corr;
        if (tests_duty[c_duty]>(pmwr/2)) {
          tests_duty_corr = pmwr-tests_duty[c_duty];
        } else {
          tests_duty_corr = tests_duty[c_duty];
        }

        double duty_fact = tests_duty_corr/(double)pmwr;
        printf("\n%d/%d set duty %d/%d\n",c_duty+1, c_duty_end, tests_duty[c_duty], pmwr);
        pwmWrite(PWM, tests_duty[c_duty]);

        for (int c_pwmc=0, end = sizeof(tests_pwmc)/sizeof(tests_pwmc[0]); c_pwmc<end; c_pwmc++) {
          int pwmc = tests_pwmc[c_pwmc];
          if (Pi4 && pwmc>1456) {
            printf("* Set clock (pwmc) %d not possible on BCM2711 system (OSC 54 MHz), ignore\n", pwmc);
            continue;
          }
          double freq = 19200.0/pwmc*duty_fact;
          if (freq>MaxFreq) {
            printf("* Set clock (pwmc) %d not possible on system (to slow to measure %g kHz with ISR), ignore\n", pwmc, freq);
            continue;
          }
          pwmSetClock(pwmc);
          delay(250);
          sprintf(msg, "Set Clock (pwmc) %d, %d%% duty", pwmc, tests_duty[c_duty]*100/pmwr);
          MeasureAndCheckFreq(msg, freq);
        }
      }
      }

      delay(250);
      printf("\n");
      printf("*********************************\n");
      printf("*          PWM MS mode          *\n");
      printf("*********************************\n");
      int pwmc = 10;
      printf("SetClock pwmc=%d and enable MS mode\n", pwmc);
      pwmSetClock(pwmc);
      pwmSetMode(PWM_MODE_MS);
      printf("Wait for start ...\n");
      delay(250);
      printf("Start:\n");
      for (int c_pmwr=0, c_pmwr_end = sizeof(tests_pwmr)/sizeof(tests_pwmr[0]); c_pmwr<c_pmwr_end; c_pmwr++) {
        int pwmr = tests_pwmr[c_pmwr];
        double freq = 19200.0/(double)pwmc/(double)pwmr;
        if (freq>MaxFreq) {
          printf("* Set Clock (pwmc, pwmr) %d, %d not possible on system (to slow to measure %g kHz with ISR), ignore\n", pwmc, pwmr, freq);
          continue;
        }
        sprintf(msg, "Set range (pwmr) %d", pwmr);
        pwmSetRange(pwmr);

        for (int c_pmw=0, c_pmw_end = sizeof(tests_pwm)/sizeof(tests_pwm[0]); c_pmw<c_pmw_end; c_pmw++) {
          int pwm = pwmr*tests_pwm[c_pmw]/100;
          sprintf(msg, "Set pwm %d/%d (%d %%)", pwm, pwmr, tests_pwm[c_pmw]);
          pwmWrite(PWM, pwm);
          delay(250);
          MeasureAndCheckFreq(msg, freq);
        }
      }

      result = wiringPiISRStop(FREQIN);
      CheckSame("\n\nRelease ISR", result, 0);
      if (result < 0) {
       printf("Unable to release ISR for GPIO %d (%s)\n\n", FREQIN, strerror(errno));
       return UnitTestState();
     }
     printf("set PWM@GPIO%d (output) back to input\n", PWM);
     pinMode(PWM, INPUT);
   }

   printf("\nDid %d PWM GPIO tests with model %d\n", testruns, RaspberryPiModel);

   return UnitTestState();
}
