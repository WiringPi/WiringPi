/*
 * Helper program of test_conflict_between_processes.
 *
 * The program configures ISR on pin 1 (BCM 18) and polls for a termination request.
 */
#include <stdint.h>
#include <stdio.h>
#include <wiringPi.h>

#define LISTENER_PIN 1  // BCM 18

static uint8_t ok_to_stop = 0;
static void noop() {
  fprintf(stderr, "interrupted\n");
  ok_to_stop = 1;
}

int main(int, char *[]) {
  fprintf(stderr, "start\n");
  wiringPiSetup();
  if (wiringPiISR(LISTENER_PIN, INT_EDGE_RISING, noop)) {
    fprintf(stderr, "failed to start listener\n");
    return -1;
  }
  fprintf(stderr, "start listening\n");

  while (!ok_to_stop) {
    delay(1000);
  }
  fprintf(stderr, "bye\n");
}