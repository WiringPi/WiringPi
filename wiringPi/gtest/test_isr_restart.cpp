#include <gtest/gtest.h>
#include <time.h>
#include <wiringPi.h>

#define TRIGGER_PIN  0  // BCM 17
#define LISTENER_PIN 1  // BCM 18
#define ANOTHER_LISTENER_PIN 6  // BCM 25

/**
 * Tests restarting ISR.
 */
class ISRRestart : public ::testing::Test {
 public:
  static void SetUpTestSuite() {
    wiringPiSetup();
    pinMode(TRIGGER_PIN, OUTPUT);
    digitalWrite(TRIGGER_PIN, LOW);
    wiringPiISR(LISTENER_PIN, INT_EDGE_RISING, ISRRestart::TriggerCount);
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

uint32_t ISRRestart::count = 0;

TEST_F(ISRRestart, RestartISR) {
  // fundamental ISR test
  EXPECT_EQ(count, 0u);
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(10);
  EXPECT_EQ(count, 1u);
  digitalWrite(TRIGGER_PIN, LOW);
  delay(10);
  EXPECT_EQ(count, 1u);

  // stop the ISR
  EXPECT_EQ(wiringPiISRStop(LISTENER_PIN), 0);
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(10);
  EXPECT_EQ(count, 1u);
  digitalWrite(TRIGGER_PIN, LOW);

  // start another ISR on the same pin
  EXPECT_EQ(
      wiringPiISR(LISTENER_PIN, INT_EDGE_FALLING, ISRRestart::TriggerCountDown),
      0);
  count=1;
  delay(100);
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(10);
  EXPECT_EQ(count, 1u);
  digitalWrite(TRIGGER_PIN, LOW);
  delay(10);
  EXPECT_EQ(count, 0u);
}
