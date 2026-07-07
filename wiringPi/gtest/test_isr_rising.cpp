#include <gtest/gtest.h>
#include <time.h>
#include <wiringPi.h>

#define TRIGGER_PIN  0  // BCM 17
#define LISTENER_PIN 1  // BCM 18
#define ANOTHER_LISTENER_PIN 26  // BCM 12

/**
 * Tests ISR for rising edges.
 */
class ISRRising : public ::testing::Test {
 public:
  static void SetUpTestSuite() {
    wiringPiSetup();
    pinMode(TRIGGER_PIN, OUTPUT);
    digitalWrite(TRIGGER_PIN, LOW);
    wiringPiISR(LISTENER_PIN, INT_EDGE_RISING, ISRRising::TriggerCount);
    delay(100);
  }

  static uint32_t count;

  static void TriggerCount() { ++count; }

  static void TriggerCountDown() { --count; }

 protected:
  virtual void SetUp() { count = 0; }

  virtual void TearDown() {
    digitalWrite(TRIGGER_PIN, LOW);
    delay(10);
  }
};

uint32_t ISRRising::count = 0;

TEST_F(ISRRising, Fundamental) {
  EXPECT_EQ(count, 0u);
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(10);
  EXPECT_EQ(count, 1u);
  digitalWrite(TRIGGER_PIN, LOW);
  delay(10);
  EXPECT_EQ(count, 1u);
}

TEST_F(ISRRising, ThreeTriggers) {
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(10);
  count = 0;
  for (int i = 0; i < 6; ++i) {
    delay(10);
    digitalWrite(TRIGGER_PIN, i % 2);
  }
  delay(10);
  EXPECT_EQ(count, 3u);
}

TEST_F(ISRRising, ISRInitConflict) {
  // this initialization should fail for ISR conflict
  EXPECT_EQ(
      wiringPiISR(LISTENER_PIN, INT_EDGE_FALLING, ISRRising::TriggerCountDown),
      -1);
  // ensure the initialization above has released the mutex lock
  EXPECT_EQ(wiringPiISR(ANOTHER_LISTENER_PIN, INT_EDGE_RISING,
                        ISRRising::TriggerCountDown),
            0);
  delay(100);

  // ensure the existing ISR has not broken
  EXPECT_EQ(count, 0u);
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(10);
  EXPECT_EQ(count, 1u);
  digitalWrite(TRIGGER_PIN, LOW);
  delay(10);
  EXPECT_EQ(count, 1u);
}
