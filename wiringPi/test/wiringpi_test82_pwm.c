// WiringPi test program: PWM test
// Compile: gcc -Wall wiringpi_test82_pwm.c -o wiringpi_test82_pwm -lwiringPi

#include "wpi_test.h"
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

int PWM_OUT[4] = { 18, 12, 13, 19 };
int PWM_IN[4]  = { 17, 13, 12, 26 };

int gFreqinPin = 0;

double MeasureAndCheckFreq(const char* msg, double expect_freq) {
  double fFrequency;
  clock_t CPUClockBegin, CPUClockEnd;
  double CPUClockInterval;
  double elapsed_time, CPULoad;
  uint64_t tbegin, tend;
  const double TargetEdges = 25.0;   // aim to capture this many edges for a stable reading
  const int MinSleepMs = 300;
  const int MaxSleepMs = 1200;
  int SleepMs = (expect_freq>0.0) ? (int)(TargetEdges/expect_freq) : MaxSleepMs;
  if (SleepMs<MinSleepMs) SleepMs = MinSleepMs;
  if (SleepMs>MaxSleepMs) SleepMs = MaxSleepMs;

  CPUClockBegin = clock();
  tbegin = piMicros64();

  unsigned long long freqHz = frequencyIn(gFreqinPin, (unsigned long)SleepMs);
  tend = piMicros64();
  elapsed_time = (double)(tend-tbegin)/1.0e6;
  fFrequency = (double)freqHz / 1000.0;

  CPUClockEnd = clock();
  CPUClockInterval = CPUClockEnd - CPUClockBegin;
  CPULoad = CPUClockInterval*100.0 / CLOCKS_PER_SEC / elapsed_time;

  printf("\nInterval:  time: %.6f sec (CPU: %3.1f %%) -> frequency: %.3f kHz\n",
    elapsed_time, CPULoad, fFrequency);

  CheckBetweenDouble("Wait for freq. meas.", elapsed_time, SleepMs/1000.0, 2.0*SleepMs/1000.0); //no undershoot allowed, up to 2x overrun ok (RT-prio ISR thread can delay main thread on busy/weak hardware)
  CheckSameDouble(msg, fFrequency, expect_freq, expect_freq*2/100); //2% toleranc
  return fFrequency;
}


int tests_pwmc[7] = {1456, 1000,  512,  200, 2000, 3000, 4000};
int tests_duty[7] = { 512,  768,  682,  922,  256,  341,  102};
int tests_pwmr[5] = {  50,  512, 2000, 5000, 20000};
int tests_pwm[3]  = {  50,  25,  75};

#define DUTY_TEST_PWMC 1000  // fixed clock used for the duty-cycle formula sweep (BAL mode)

int main (void) {

    int major, minor;
    char msg[255];
    int testruns = 4;
    int PWM;

    wiringPiVersion(&major, &minor);
    printf("WiringPi PWM test program 8.2 (BAL & MS Mode, different frequencys, up to 4 PWM pins) - WiringPi %d.%d\n", major, minor);
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
      case PI_MODEL_CM:
        MaxFreq = 5.0; // 4.8 kHz -> ~26% CPU@800 MHz
        printf("Pi1/BCM2835 detected, will skip tests with frequency above %g kHz\n", MaxFreq);
        break;
      case PI_MODEL_ZERO:
      case PI_MODEL_ZERO_W:
        MaxFreq = 13.0; // 12.5 kHz -> ~42% CPU@1000 MHz
        printf("PiZero/BCM2835 detected, will skip tests with frequency above %g kHz\n", MaxFreq);
        break;
      case PI_MODEL_2:
        MaxFreq = 20.0;
        printf("Pi2/BCM2836 detected, will skip tests with frequency above %g kHz\n", MaxFreq);
        break;
      case PI_MODEL_3B:
      case PI_MODEL_CM3:
      case PI_MODEL_3BP:
      case PI_MODEL_3AP:
      case PI_MODEL_CM3P:
      case PI_MODEL_ZERO_2W:
        MaxFreq = 50.0;
        printf("Pi3/BCM2837 detected, will skip tests with frequency above %g kHz\n", MaxFreq);
        break;
      case PI_MODEL_4B:
      case PI_MODEL_400:
      case PI_MODEL_CM4:
      case PI_MODEL_CM4S:
        Pi4 = 1;
        break;
      case PI_MODEL_5:
      case PI_MODEL_CM5:
      case PI_MODEL_500:
      case PI_MODEL_CM5L:
         Pi5 = 1;
         break;
    }

    if (!piBoard40Pin()) {
        testruns = 1;  // only fist PWM0, supported
    }

    for (int testrun=0; testrun<testruns; testrun++) {
      PWM = PWM_OUT[testrun];
      gFreqinPin = PWM_IN[testrun];
      printf("\n--- using PWM@GPIO%d (output) and GPIO%d (input)---\n", PWM, gFreqinPin);
      delay(1000);
      printf("\n");
      printf("*********************************\n");
      printf("*  PWM BAL mode  @  GPIO%02d      *\n", PWM);
      printf("*********************************\n");
      const int pmw = 512;
      int pmwr = 1024; //default!

      printf("Set pwm 50%% and enable PWM output (600 kHz?) \n");
      pwmWrite(PWM, pmw);  //50% Duty
      pinMode(PWM, PWM_OUTPUT);  //Mode BAL, pwmr=1024, pwmc=32
      printf("pwmc 4.8kHz\n");
      pwmSetClock(2000);
      delay(200);
      printf("Wait for start ...\n");
      delay(1000);
      if (!Pi5) {
        printf("Start:\n");
        //MeasureAndCheckFreq("50\% Duty (default)", 300.000);   //FAIL , freq (pwmc=32) to high for irq count
        printf("\n--- Clock (pwmc) sweep at fixed 50%% duty ---\n");
        pwmWrite(PWM, pmwr/2);  // 50% duty
        for (int c_pwmc=0, end = sizeof(tests_pwmc)/sizeof(tests_pwmc[0]); c_pwmc<end; c_pwmc++) {
          int pwmc = tests_pwmc[c_pwmc];
          if (Pi4 && pwmc>1456) {
            printf("* Set clock (pwmc) %d not possible on BCM2711 system (OSC 54 MHz), ignore\n", pwmc);
            continue;
          }
          double freq = 19200.0/pwmc*0.5;
          if (freq>MaxFreq) {
            printf("* Set clock (pwmc) %d not possible on system (to slow to measure %g kHz with ISR), ignore\n", pwmc, freq);
            continue;
          }
          pwmSetClock(pwmc);
          delay(250);
          sprintf(msg, "Set Clock (pwmc) %d, 50%% duty", pwmc);
          MeasureAndCheckFreq(msg, freq);
        }

        printf("\n--- Duty sweep at fixed pwmc=%d ---\n", DUTY_TEST_PWMC);
        pwmSetClock(DUTY_TEST_PWMC);
        for (int c_duty=0, c_duty_end = sizeof(tests_duty)/sizeof(tests_duty[0]); c_duty<c_duty_end; c_duty++) {
          double tests_duty_corr;
          if (tests_duty[c_duty]>(pmwr/2)) {
            tests_duty_corr = pmwr-tests_duty[c_duty];
          } else {
            tests_duty_corr = tests_duty[c_duty];
          }

          double duty_fact = tests_duty_corr/(double)pmwr;
          double freq = 19200.0/DUTY_TEST_PWMC*duty_fact;
          if (freq>MaxFreq) {
            printf("* Duty %d not possible on system (to slow to measure %g kHz with ISR), ignore\n", tests_duty[c_duty], freq);
            continue;
          }
          printf("\n%d/%d set duty %d/%d\n",c_duty+1, c_duty_end, tests_duty[c_duty], pmwr);
          pwmWrite(PWM, tests_duty[c_duty]);
          delay(250);
          sprintf(msg, "Set duty %d%%, pwmc=%d", tests_duty[c_duty]*100/pmwr, DUTY_TEST_PWMC);
          MeasureAndCheckFreq(msg, freq);
        }
      } else {
        printf("skipped, Pi5 has no support for BAL mode...\n");
      }

      delay(250);
      printf("\n");
      printf("*********************************\n");
      printf("*  PWM MS  mode  @  GPIO%02d     *\n", PWM);
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

     printf("set PWM@GPIO%d (output) back to input\n", PWM);
     pinMode(PWM, INPUT);
   }

   printf("\nDid %d PWM GPIO tests with model %d\n", testruns, RaspberryPiModel);

   return UnitTestState();
}
