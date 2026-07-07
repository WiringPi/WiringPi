#include <gtest/gtest.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <wiringPi.h>

#define TRIGGER_PIN  0  // BCM 17
#define LISTENER_PIN 1  // BCM 18
#define ANOTHER_LISTENER_PIN 6  // BCM 25

static void *remote_isr(void *) {
  std::system("./bin/run_isr");
  return NULL;
}

/**
 * Tests ISR setup conflict between two processes.
 *
 * Another program 'run_isr' starts first that occupies the pin 31 for an ISR.
 * This program should fail to start ISR on the same pin.
 */
class ISRConflict : public ::testing::Test {
 public:
  static void SetUpTestSuite() {
    wiringPiSetup();
    digitalWrite(TRIGGER_PIN, LOW);

    pthread_t tid;
    pthread_create(&tid, NULL, remote_isr, NULL);
    delay(1000);
  }

  static void TearDownTestSuite() {
      delay(1000);
  }

  static uint32_t count;

  static void TriggerCount() { ++count; }
  static void TriggerCount2(WPIWfiStatus, void *userdata) {
    ++*(uint32_t *)userdata;
  }

 protected:
  virtual void SetUp() { count = 0; }

  virtual void TearDown() {
    digitalWrite(TRIGGER_PIN, LOW);
    delay(10);
  }
};

uint32_t ISRConflict::count = 0;

TEST_F(ISRConflict, TestConflictBetweenProcesses) {
  // The ISRs should fail to start because run_isr is listening on the
  // same port
  EXPECT_EQ(
      wiringPiISR(LISTENER_PIN, INT_EDGE_FALLING, ISRConflict::TriggerCount),
      -1);
  EXPECT_EQ(wiringPiISR2(LISTENER_PIN, INT_EDGE_FALLING,
                         ISRConflict::TriggerCount2, 0, &ISRConflict::count),
            -1);

  // to exit run_isr
  digitalWrite(TRIGGER_PIN, HIGH);
}
