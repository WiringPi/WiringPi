// WiringPi test program: PWM test
// Compile: gcc -Wall wiringpi_test9_pwm.c -o wiringpi_test9_pwm -lwiringPi

#include "wpi_test.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>


int PWM0 = 12;
int FREQIN = 6;

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

  printf("Interval:  time: %.6f sec (CPU: %3.1f %%), count: %g  -> frequency: %.3f kHz\n", 
    elapsed_time, CPULoad, CountInterval, fFrequency);

  CheckSameDouble("Wait for freq. meas.", elapsed_time, SleepMs/1000.0, 0.1); //100ms tolerance. maybe problematic on high freq/cpu load 
  CheckSameDouble(msg, fFrequency, expect_freq, expect_freq*2/100); //2% tolerance

  return fFrequency;
}


int tests_pwmc[7] = {1456, 1000,  512,  100, 2000, 3000, 4000};
int tests_duty[7] = { 512,  768,  682,  922,  256,  341,  102};
int tests_pwmr[10]= { 100,  512, 1024, 1456, 2000, 3000, 5000, 10000, 15000, 20000};

int main (void) {

	int major, minor;
  char msg[255];

	wiringPiVersion(&major, &minor);

	printf("WiringPi GPIO test program 9 (using PWM@GPIO%d (output) and GPIO%d (input))\n", PWM0, FREQIN);
	printf(" pwm0 irq time\n");

	printf("\nPWM/ISR test (WiringPi %d.%d)\n", major, minor);

	wiringPiSetupGpio() ;

	int rev, mem, maker, overVolted, RaspberryPiModel;
	piBoardId(&RaspberryPiModel, &rev, &mem, &maker, &overVolted);
	CheckNotSame("Model: ", RaspberryPiModel, -1);

	if (!piBoard40Pin()) {
		PWM0 = 18;
		FREQIN = 17;
	}
// INT_EDGE_BOTH, INT_EDGE_FALLING, INT_EDGE_RISING only one ISR per input
	int result = wiringPiISR(FREQIN, INT_EDGE_RISING, &ISR_FREQIN);
  CheckSame("Register ISR", result, 0);
   if (result < 0) {
		printf("Unable to setup ISR for GPIO %d (%s)\n\n", 
		  FREQIN, strerror(errno));
    return UnitTestState();
	} 

  int Pi4 = 0;
  switch(RaspberryPiModel) {
    case PI_MODEL_4B:
    case PI_MODEL_400:
    case PI_MODEL_CM4:
    case PI_MODEL_CM4S:   
      Pi4 = 1;
      break;
    case PI_MODEL_5:
      return UnitTestState();  //not supported so far
  }

  printf("\nPWM0 BAL mode:\n");
  printf("==============\n");

	pinMode(PWM0, PWM_OUTPUT);  //pwmr=1024, pwmc=32
  const int pmw = 512;
  int pmwr = 1024;
  pwmWrite(PWM0, pmw);  //50% Duty
  //MeasureAndCheckFreq("50\% Duty (default)", 300.000);   //FAIL , freq (pwmc=32) to high for irq count

  for (int c_duty=0, c_duty_end = sizeof(tests_duty)/sizeof(tests_duty[0]); c_duty<c_duty_end; c_duty++) {

    double tests_duty_corr;
    if (tests_duty[c_duty]>(pmwr/2)) {
      tests_duty_corr = pmwr-tests_duty[c_duty];  
    } else {
      tests_duty_corr = tests_duty[c_duty];
    }

    double duty_fact = tests_duty_corr/(double)pmwr;
    printf("\n%d/%d set duty %d/%d\n",c_duty+1, c_duty_end, tests_duty[c_duty], pmwr);
    pwmWrite(PWM0, tests_duty[c_duty]);

    for (int c_pwmc=0, end = sizeof(tests_pwmc)/sizeof(tests_pwmc[0]); c_pwmc<end; c_pwmc++) {
      if (Pi4 && tests_pwmc[c_pwmc]>1456) {
        printf("* Set Clock (pwmc) %d not possible on BCM2711 system (OSC 54 MHz), ignore\n", tests_pwmc[c_pwmc]);
        continue;
      }
      pwmSetClock(tests_pwmc[c_pwmc]);
      delay(250);
      double freq = 19200.0/tests_pwmc[c_pwmc]*duty_fact;
      sprintf(msg, "Set Clock (pwmc) %d, %d%% duty", tests_pwmc[c_pwmc], tests_duty[c_duty]*100/pmwr);
      MeasureAndCheckFreq(msg, freq);
    }

  }

  delay(250);
  pwmSetMode(PWM_MODE_MS) ;
  printf("\nPWM0 MS mode:\n");
  printf("==============\n");

  int pwmc = 10; 
  pwmSetClock(pwmc);
  delay(2500);

  for (int c_pmwr=0, c_pmwr_end = sizeof(tests_pwmr)/sizeof(tests_pwmr[0]); c_pmwr<c_pmwr_end; c_pmwr++) {
    pwmWrite(PWM0, tests_pwmr[c_pmwr]/2); 
    pwmSetRange(tests_pwmr[c_pmwr]) ;
    delay(250);
    double freq = 19200.0/(double)pwmc/(double)tests_pwmr[c_pmwr];
    sprintf(msg, "Set range (pwmr) %d", tests_pwmr[c_pmwr]);
    MeasureAndCheckFreq(msg, freq);
  }


	return UnitTestState();
}
