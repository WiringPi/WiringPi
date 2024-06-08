#include "wpi_test.h"
#include "../../version.h"

// PiFace dummy

int main (void) {
  int major, minor;


  wiringPiVersion(&major, &minor);
  printf("Testing piface functions with WiringPi %d.%d\n",major, minor);
  printf("------------------------------------------\n\n");



  return UnitTestState();
}
