// WiringPi test program: 6.1 new ISR2 function with Kernel char device interface / sysfs successor
// Compile: gcc -Wall wiringpi_test61_isr2.c -o wiringpi_test61_isr2 -lwiringPi

#include "wpi_test.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>


int GPIO = 19;
int GPIOIN = 26;
const int ToggleValue = 4;
float irq_timstamp_duration_ms = 0;
float accuracy = 0.015;
float bounce_acc = 1.0;

static volatile int globalCounter;
volatile long long gStartTime, gEndTime;
struct WPIWfiStatus wfiStatusOld;

static void ISR(void) { 
  struct timeval now;

  gettimeofday(&now, 0);
  if (0==gStartTime) {
    gStartTime = now.tv_sec*1000000LL + now.tv_usec;
  } else {
    gEndTime = now.tv_sec*1000000LL + now.tv_usec;
  }
  globalCounter++;
}



static void ISR2(struct WPIWfiStatus wfiStatus, void* userdata) {
  struct timeval now;
  char strEdge[3][10] = {"unknown", "falling", "raising"};
  int strEdgeidx = 0; //default
  int* localCounter = (int*) userdata;

  gettimeofday(&now, 0);
  if (0==gStartTime) {
    gStartTime = now.tv_sec*1000000LL + now.tv_usec;
  } else {
    gEndTime = now.tv_sec*1000000LL + now.tv_usec;
  }
  globalCounter++;
  if (localCounter) {
    (*localCounter)++;
  }
  switch(wfiStatus.edge) {
    case INT_EDGE_FALLING: strEdgeidx = INT_EDGE_FALLING; break;
    case INT_EDGE_RISING:  strEdgeidx = INT_EDGE_RISING;  break;
  }
  if (wfiStatusOld.statusOK>0 && wfiStatusOld.pinBCM==wfiStatus.pinBCM) {
    irq_timstamp_duration_ms = (wfiStatus.timeStamp_us - wfiStatusOld.timeStamp_us) / 1000.0f;
  } else {
    irq_timstamp_duration_ms = 0.0f;
  }
  printf("ISR occured @ %lld us: statusOK=%d, pin=%u, edge=%s (%d), duration=%g ms, userdata=%p\n",
    wfiStatus.timeStamp_us, wfiStatus.statusOK, wfiStatus.pinBCM, strEdge[strEdgeidx], wfiStatus.edge, irq_timstamp_duration_ms, userdata);
  wfiStatusOld = wfiStatus;
}


void digitalWriteBounce(int OUTpin, int value, int bounce) {
  digitalWrite(OUTpin, value);
  if (bounce>0) {
    delay(1);
    digitalWrite(OUTpin, !value);
    delay(1);
    digitalWrite(OUTpin, value);
  }
}


void DelayAndSumDuration(int ms, float* irq_timstamp_sum, const int doSum) {
  delay(20);
  if (doSum) {
    *irq_timstamp_sum += irq_timstamp_duration_ms;
  }
  delay(ms-20);
}


double StartSequence2(int Edge, int OUTpin, int INpin, int bounce, int* localCounter) {
  int expected;
  float timeExpected_ms;
  float irq_timstamp_sum = 0.0f;

  gStartTime = 0;
  gEndTime = 0;
  globalCounter = 0;
  *localCounter = 0;
  printf("Start\n");
  digitalWriteBounce(OUTpin, HIGH, bounce);
  delay(200);

  digitalWriteBounce(OUTpin, LOW, bounce);
  DelayAndSumDuration(100, &irq_timstamp_sum, INT_EDGE_BOTH == Edge);

  digitalWriteBounce(OUTpin, HIGH, bounce);
  DelayAndSumDuration(200, &irq_timstamp_sum, INT_EDGE_RISING == Edge || INT_EDGE_BOTH == Edge);

  digitalWriteBounce(OUTpin, LOW, bounce);
  DelayAndSumDuration(100, &irq_timstamp_sum, INT_EDGE_FALLING == Edge || INT_EDGE_BOTH == Edge);

  printf("Stop\n");
  int globalCounterCopy = globalCounter;

  if (INT_EDGE_BOTH == Edge) {
    expected = 4;  
    timeExpected_ms = bounce ? 508: 500;
  } else {
    expected = 2;  
    timeExpected_ms = bounce ? 304: 300;
  }

  CheckSame("Global counted IRQ", globalCounter, expected);
  CheckSame("Userdata pointer / Local counted IRQ ", *localCounter, expected);

  if (globalCounter==expected) {
    char str[1024];
    float fTime = (gEndTime - gStartTime) / 1000.0;
    sprintf(str, "IRQ measured  %g msec (~%g expected)", fTime, timeExpected_ms);
    CheckSameFloat(str, fTime, timeExpected_ms, timeExpected_ms*accuracy);
    sprintf(str, "IRQ timestamp %g msec (~%g expected)", irq_timstamp_sum, timeExpected_ms);
    CheckSameFloat(str, irq_timstamp_sum, timeExpected_ms, timeExpected_ms*accuracy);
    // new data struct
    CheckSame("GPIO IRQ pin", wfiStatusOld.pinBCM, INpin);
    if (INT_EDGE_FALLING==Edge || INT_EDGE_RISING==Edge) {
      CheckSame("GPIO IRQ edge", wfiStatusOld.edge, Edge);
    }
    CheckSame("GPIO IRQ status", wfiStatusOld.statusOK, 1);
    return fTime;
  } else {
    printf("IRQ not worked got %d iterations (%d exprected)\n\n", globalCounterCopy, expected);
    return 0;
  }
}


double DurationTime(int Enge, int OUTpin, int IRQpin, int bounce) {
  struct timeval now;
  double fTime = 0.0;
  const char* strOp = INT_EDGE_RISING == Enge ? "rising" : "fallling";
  gStartTime = 0;
  gEndTime = 0;
  globalCounter = 0;
  //printf("Start\n");

  digitalWrite(OUTpin, INT_EDGE_RISING == Enge ? LOW : HIGH);
  if (bounce>=0) {
    printf("\nnew function, bounce time %d ms, %s :\n", bounce, strOp);
    wiringPiISR2(IRQpin, Enge, &ISR2, bounce*1000, NULL);
  } else {
    printf("\nclassic function, %s :\n", strOp);
    wiringPiISR(IRQpin, Enge, &ISR);
  }
  sleep(1);
  gettimeofday(&now, 0);
  gStartTime = now.tv_sec*1000000LL + now.tv_usec;
  digitalWrite(OUTpin, INT_EDGE_RISING == Enge ? HIGH : LOW);
  delay(20);
  digitalWrite(OUTpin, LOW);
  delay(20);
  fTime = (gEndTime - gStartTime);
  if (bounce<=0) {
    printf("IRQ detection time %g usec", fTime);
  } else {
    printf("IRQ detection time %.1f msec", fTime/1000.0);
  }
  if (bounce>=0) {
    // bounce time + 7ms (addtional bounce time) + 100 us (basic time)
    CheckBetween("IRQ detection time with bounce [us]", fTime, 0, bounce*1000+ (bounce>0 ? 7000*bounce_acc : 0)+(100*bounce_acc));
  } else {
    CheckBetween("IRQ detection time [us]", fTime, 0, 100*bounce_acc);
  }
  wiringPiISRStop(IRQpin);
  //printf("Stop\n");
  return fTime;
}


int main (void) {

	int major=0, minor=0;

	wiringPiVersion(&major, &minor);

  int _is40pin = piBoard40Pin();
  CheckNotSame("40-Pin board: ", _is40pin, -1);
  if (_is40pin==0) {
      printf("Old 28pin system\n");
		//GPIO = 23;
		//GPIOIN = 24;
	  GPIO = 17;
    GPIOIN = 18;
  }

	printf("WiringPi GPIO test program 6.2 (using GPIO%d (output) and GPIO%d (input))\n", GPIO, GPIOIN);
	printf("ISR and ISR2 test (WiringPi %d.%d)\n", major, minor);

	wiringPiSetupGpio();

  int RaspberryPiModel, rev, mem, maker, overVolted;
  piBoardId(&RaspberryPiModel, &rev, &mem, &maker, &overVolted);
  CheckNotSame("Model: ", RaspberryPiModel, -1);
  switch(RaspberryPiModel) {
    case PI_MODEL_A:
    case PI_MODEL_B:     //ARM=800MHz
    case PI_MODEL_BP:
    case PI_MODEL_AP:
    case PI_MODEL_CM:
      accuracy = 0.02;
      bounce_acc = 2.7;
      break;
    case PI_MODEL_ZERO:
    case PI_MODEL_ZERO_W: //ARM=1000MHz
      accuracy = 0.02;
      bounce_acc = 2.7;
      break;
    default:
      accuracy = 0.012;
      bounce_acc = 1.0;
      break;
  }
	int IRQpin = GPIOIN;
	int OUTpin = GPIO;

  //wiringPiISR2(13, INT_EDGE_RISING, &ISR2, 0); // next pin
	int localCounter;
  memset(&wfiStatusOld, 0, sizeof(wfiStatusOld));

	pinMode(IRQpin, INPUT);
	pinMode(OUTpin, OUTPUT);
	digitalWrite (OUTpin, LOW) ;

  for (int bounce=0; bounce<=1; bounce++) {
    unsigned long bouncetime = 0;
    if (0==bounce) {
      printf("! --- default test (no bounce) --- !\n");
    } else {
      bouncetime = 2000; //2 ms
      printf("! --- test with bounce on input --- !\n");
    }

    printf("\nTesting IRQ @ GPIO%d with trigger @ GPIO%d rising\n", IRQpin, OUTpin);
    wiringPiISR2(IRQpin, INT_EDGE_RISING, &ISR2, bouncetime, &localCounter);
    sleep(1);
    StartSequence2(INT_EDGE_RISING, OUTpin, IRQpin, bounce, &localCounter);
    printf("Stopp IRQ\n");
    wiringPiISRStop(IRQpin);

    printf("\nTesting IRQ @ GPIO%d with trigger @ GPIO%d falling\n", IRQpin, OUTpin);
    wiringPiISR2(IRQpin, INT_EDGE_FALLING, &ISR2, bouncetime, &localCounter);
    sleep(1);
    StartSequence2(INT_EDGE_FALLING, OUTpin, IRQpin, bounce, &localCounter);
    printf("Stopp IRQ\n");
    wiringPiISRStop(IRQpin);

    printf("\nTesting IRQ @ GPIO%d with trigger @ GPIO%d both\n", IRQpin, OUTpin);
    wiringPiISR2(IRQpin, INT_EDGE_BOTH, &ISR2, bouncetime, &localCounter);
    sleep(1);
    StartSequence2(INT_EDGE_BOTH, OUTpin, IRQpin, bounce, &localCounter);
    printf("Stopp IRQ\n");
    wiringPiISRStop(IRQpin);
  }

  printf("Measuring duration IRQ @ GPIO%d with trigger @ GPIO%d rising and falling\n", IRQpin, OUTpin);
	for (int bounce=-1; bounce<=5; bounce++) {
	  DurationTime(INT_EDGE_RISING, OUTpin, IRQpin, bounce);
	  DurationTime(INT_EDGE_FALLING, OUTpin, IRQpin, bounce);
	}
	pinMode(OUTpin, INPUT);

	return UnitTestState();
}
