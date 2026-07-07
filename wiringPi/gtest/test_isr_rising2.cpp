#include <gtest/gtest.h>
#include <time.h>
#include <wiringPi.h>

#define TRIGGER_PIN  0  // BCM 17
#define LISTENER_PIN 1  // BCM 18
#define ANOTHER_LISTENER_PIN 26  // BCM 12

struct counters {
  uint32_t *primary;
  uint32_t *secondary;
};

/**
 * Tests ISR for rising edges.
 * Function wiringPiISR2 is used for the configuraiton.
 */
class ISRRising2 : public ::testing::Test {
 public:
  static void SetUpTestSuite() {
    wiringPiSetup();
    pinMode(TRIGGER_PIN, OUTPUT);
    digitalWrite(TRIGGER_PIN, LOW);
    counters_a.primary = &count;
    counters_a.secondary = &count2;
    counters_b.primary = &count;
    counters_b.secondary = &count3;
    wiringPiISR2(LISTENER_PIN, INT_EDGE_RISING, ISRRising2::TriggerCount, 1000,
                 &counters_a);
    delay(100);
  }

  static uint32_t count;
  static uint32_t count2;
  static uint32_t count3;

  static counters counters_a;
  static counters counters_b;

  static void TriggerCount(WPIWfiStatus, void *userdata) {
    struct counters *counters = (struct counters *)userdata;
    ++*counters->primary;
    ++*counters->secondary;
  }

  static void TriggerCountDown(WPIWfiStatus, void *userdata) {
    struct counters *counters = (struct counters *)userdata;
    ++*counters->primary;
    --*counters->secondary;
  }

 protected:
  virtual void SetUp() {
    count = 0;
    count2 = 0;
    count3 = 0;
  }

  virtual void TearDown() {
    digitalWrite(TRIGGER_PIN, LOW);
    delay(10);
  }
};

uint32_t ISRRising2::count = 0;
uint32_t ISRRising2::count2 = 0;
uint32_t ISRRising2::count3 = 0;
struct counters ISRRising2::counters_a;
struct counters ISRRising2::counters_b;

TEST_F(ISRRising2, Fundamental) {
  EXPECT_EQ(count, 0u);
  EXPECT_EQ(count2, 0u);
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(20);
  EXPECT_EQ(count, 1u);
  EXPECT_EQ(count2, 1u);
  digitalWrite(TRIGGER_PIN, LOW);
  delay(20);
  EXPECT_EQ(count, 1u);
  EXPECT_EQ(count2, 1u);
}

TEST_F(ISRRising2, ThreeTriggers) {
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(10);
  count = 0;
  for (int i = 0; i < 6; ++i) {
    digitalWrite(TRIGGER_PIN, i % 2);
    delay(20);
  }
  EXPECT_EQ(count, 3u);
}

TEST_F(ISRRising2, Debounce) {
  delay(20);
  for (int i = 0; i < 7; ++i) {
    digitalWrite(TRIGGER_PIN, (i + 1) % 2);
  }
  delay(20);
  EXPECT_EQ(count, 1u);
}

TEST_F(ISRRising2, ISRInitConflict) {
  // this initialization should fail for ISR conflict
  EXPECT_EQ(wiringPiISR2(LISTENER_PIN, INT_EDGE_FALLING,
                         ISRRising2::TriggerCountDown, 0, &counters_b),
            -1);
  // ensure the initialization above has released the mutex lock
  EXPECT_EQ(wiringPiISR2(ANOTHER_LISTENER_PIN, INT_EDGE_BOTH,
                         ISRRising2::TriggerCountDown, 0, &counters_b),
            0);
  delay(100);

  // ensure the existing ISR has not broken
  EXPECT_EQ(count, 0u);
  EXPECT_EQ(count2, 0u);
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(20);
  EXPECT_EQ(count, 1u);
  EXPECT_EQ(count2, 1u);
  digitalWrite(TRIGGER_PIN, LOW);
  delay(20);
  EXPECT_EQ(count, 1u);
  EXPECT_EQ(count2, 1u);

  delay(20);
  count = 0;
  for (int i = 0; i < 7; ++i) {
    delay(1);
    digitalWrite(TRIGGER_PIN, (i + 1) % 2);
  }
  delay(20);
  EXPECT_EQ(count, 1u);
}
