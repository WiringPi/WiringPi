// WiringPi test program: I2C functions (need PCF8574)
// Compile: gcc -Wall wiringpi_i2c_test1.c -o wiringpi_i2c_test1 -lwiringPi

#include "wpi_test.h"
#include "pcf8574.h"
#include "wiringPiI2C.h"

const int pinBase = 1020;
const int i2cAdress = 0x20;

int ShowAll() {
  int in;
  int value = 0;

  printf("pin: 0  1  2  3  4  5  6  7\nval: ");
  for (int pin=0; pin<=7; ++pin) {
    in = digitalRead(pinBase + pin);
    printf("%d  ", in);
    if(in==HIGH) { value |= (0x01<<pin); }
  }
  printf(" = 0x%02X\n", value);
  return value;
}


void testPin(const char* msg, int fd, int pin , int value) {
  printf("%s:\n", msg);
  int in = digitalRead(pinBase + pin);
  CheckSame("digitalRead", in, value); 

  int expect =  HIGH==value ? (0x1<<pin) : 0;
  int pinmask = 0x01<<pin;
  int i2cread = wiringPiI2CRead(fd);
  CheckSame("wiringPiI2CRead", i2cread & pinmask, expect);
  //printf("Value = 0x%X\n",i2cread);

  uint8_t i2cvalue = HIGH==value ? 0x00 : 0xFF;
  int result = wiringPiI2CRawRead(fd, &i2cvalue, 1);
  CheckSame("wiringPiI2CRawRead result", result, 1);
  CheckSame("wiringPiI2CRawRead", i2cvalue & pinmask, expect);
  //printf("Value = 0x%X\n",i2cvalue);
}


int main (void) {
  int major, minor;
 
  wiringPiVersion(&major, &minor);
  printf("Testing I2C functions with PCF8574 (WiringPi %d.%d)\n",major, minor);
  printf("-------------------------------------------------\n\n");


  int ret = pcf8574Setup (pinBase, i2cAdress);
  if (ret!=1) {
    FailAndExitWithErrno("pcf8574Setup", ret);
  }
  int fd = wiringPiI2CSetup (i2cAdress);
  if (fd<=0) {
    FailAndExitWithErrno("wiringPiI2CSetup", fd);
  }
  CheckSame("I2C fd", fd, 4);

  ShowAll();

  int pin = 3;
  testPin("Test pin 3 high", fd, pin , HIGH);
  testPin("Test pin 4 high", fd, pin+1, HIGH);

  digitalWrite(pinBase + pin, LOW);
  testPin("Test pin 3 low", fd, pin , LOW);
  testPin("Test pin 4 high", fd, pin+1, HIGH);

  ShowAll();

  digitalWrite(pinBase + pin, HIGH);
  testPin("Test pin 3 high", fd, pin , HIGH);
  testPin("Test pin 4 high", fd, pin+1, HIGH);

  ShowAll();

  printf("\nwiringPiI2CReadReg8:\n");
  int i2cin, expect;
  i2cin = wiringPiI2CReadReg8(fd, 0x00);
  expect = ShowAll();
  CheckSame("all low wiringPiI2CReadReg8", i2cin, expect);

  i2cin = wiringPiI2CReadReg8(fd, 0xFF);
  expect =ShowAll();
  CheckSame("all high wiringPiI2CReadReg8", i2cin, expect);

  printf("\nwiringPiI2CReadBlockData:\n");
  uint8_t value;
  int result;
  value = 0xFF;
  result = wiringPiI2CReadBlockData(fd, 0x00, &value, 1);
  CheckSame("wiringPiI2CReadBlockData result", result, 1);
  expect = ShowAll();
  CheckSame("all high wiringPiI2CReadBlockData", value, expect);

  printf("\n");
  value = 0x00;
  result = wiringPiI2CReadBlockData(fd, 0xFF, &value, 1);
  CheckSame("wiringPiI2CReadBlockData result", result, 1);
  expect = ShowAll();
  CheckSame("all low wiringPiI2CReadBlockData", value, expect);

  return UnitTestState();
}




