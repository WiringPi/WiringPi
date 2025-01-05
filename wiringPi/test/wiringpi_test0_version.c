#include "wpi_test.h"
#include "../../version.h"

int main (void) {
  int major, minor;

  wiringPiVersion(&major, &minor);

  CheckSame("version major", major, VERSION_MAJOR);
  CheckSame("version minor", minor, VERSION_MINOR);

  return UnitTestState();
}