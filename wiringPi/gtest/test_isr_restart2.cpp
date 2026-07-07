#include <gtest/gtest.h>
#include <time.h>
#include <wiringPi.h>

#define TRIGGER_PIN  0  // BCM 17
#define LISTENER_PIN 1  // BCM 18
#define ANOTHER_LISTENER_PIN 26  // BCM 12

struct counters {
  int32_t *primary;
  int32_t *secondary;
};

/**
 * Tests restarting ISR using function wiringPiISR2().
 */
class ISRRestart2 : public ::testing::Test {
 public:
  static void SetUpTestSuite() {
    wiringPiSetup();
    pinMode(TRIGGER_PIN, OUTPUT);
    digitalWrite(TRIGGER_PIN, LOW);
    counters_a.primary = &count;
    counters_a.secondary = &count2;
    counters_b.primary = &count;
    counters_b.secondary = &count3;
    wiringPiISR2(LISTENER_PIN, INT_EDGE_RISING, ISRRestart2::TriggerCount, 1000,
                 &counters_a);
    delay(100);
  }

  static int32_t count;
  static int32_t count2;
  static int32_t count3;

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

int32_t ISRRestart2::count = 0;
int32_t ISRRestart2::count2 = 0;
int32_t ISRRestart2::count3 = 0;
struct counters ISRRestart2::counters_a;
struct counters ISRRestart2::counters_b;

TEST_F(ISRRestart2, RestartISR2) {
  // fundamental ISR2 test
  EXPECT_EQ(count, 0);
  EXPECT_EQ(count2, 0);
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(20);
  EXPECT_EQ(count, 1);
  EXPECT_EQ(count2, 1);
  digitalWrite(TRIGGER_PIN, LOW);
  delay(20);
  EXPECT_EQ(count, 1);
  EXPECT_EQ(count2, 1);

  // stop the ISR
  EXPECT_EQ(wiringPiISRStop(LISTENER_PIN), 0);
  delay(100);
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(20);
  EXPECT_EQ(count, 1);
  EXPECT_EQ(count2, 1);
  EXPECT_EQ(count3, 0);
  digitalWrite(TRIGGER_PIN, LOW);

  // start another ISR2 on the same pin
  EXPECT_EQ(wiringPiISR2(LISTENER_PIN, INT_EDGE_FALLING,
                         ISRRestart2::TriggerCountDown, 0, &counters_b),
            0);
  delay(100);
  digitalWrite(TRIGGER_PIN, HIGH);
  delay(20);
  EXPECT_EQ(count, 1);
  EXPECT_EQ(count2, 1);
  EXPECT_EQ(count3, 0);

  digitalWrite(TRIGGER_PIN, LOW);
  delay(20);
  EXPECT_EQ(count, 2);
  EXPECT_EQ(count2, 1);
  EXPECT_EQ(count3, -1);
}
