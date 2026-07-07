#include <gtest/gtest.h>
#include <time.h>
#include <wiringPi.h>

#define TRIGGER_PIN  0  // BCM 17
#define LISTENER_PIN 1  // BCM 18

/**
 * Tests ISR for falling edges.
 */
class ISRFalling : public ::testing::Test {
 public:
  static void SetUpTestSuite() {
    wiringPiSetup();
    pinMode(TRIGGER_PIN, OUTPUT);
    digitalWrite(TRIGGER_PIN, HIGH);
    wiringPiISR(LISTENER_PIN, INT_EDGE_FALLING, ISRFalling::TriggerCount);
    delay(100);
  }

  static void TearDownTestSuite() { digitalWrite(TRIGGER_PIN, LOW); }

  static uint32_t count;
  static void TriggerCount() { ++count; }

 protected:
  virtual void SetUp() { count = 0; }

  virtual void TearDown() {
    digitalWrite(TRIGGER_PIN, HIGH);
    delay(10);
  }
};

uint32_t ISRFalling::count = 0;

TEST_F(ISRFalling, Fundamental) {
  EXPECT_EQ(count, 0u);
  digitalWrite(TRIGGER_PIN, LOW);
  delay(10);
  EXPECT_EQ(count, 1u);
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(10);
  EXPECT_EQ(count, 1u);
}

TEST_F(ISRFalling, ThreeTriggers) {
  digitalWrite(TRIGGER_PIN, LOW);
  delay(10);
  count = 0;
  for (int i = 0; i < 6; ++i) {
    delay(10);
    digitalWrite(TRIGGER_PIN, (i + 1) % 2);
  }
  delay(10);
  EXPECT_EQ(count, 3u);
}