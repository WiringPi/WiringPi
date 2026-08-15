#include <gtest/gtest.h>
#include <cstdlib>
#include <string>
#include <wiringPi.h>

/**
 * Regression tests for #446: under /dev/gpiomem (no /dev/mem access), the
 * bcm2835-gpiomem kernel driver ignores the mmap offset and always remaps
 * the GPIO register page for any requested offset. Before the fix, wiringPi
 * still mmap'd PWM/CLOCK/PADS/TIMER "normally" in this case, so those
 * pointers silently aliased into GPIO registers instead of failing.
 *
 * These tests only exercise the intended path when run as a non-root user
 * with /dev/gpiomem access (e.g. member of the `gpio` group) and without
 * access to /dev/mem. Run as root, wiringPiGlobalMemoryAccess() reports full
 * access and the fallback never triggers - the tests then skip themselves
 * rather than reporting a false failure.
 */
class GpioMemFallback : public ::testing::Test {
 public:
  static void SetUpTestSuite() {
    fullAccess = (wiringPiGlobalMemoryAccess() != WPI_GLOBAL_MEM_NONE);
    if (!fullAccess) {
      ASSERT_EQ(wiringPiSetupGpio(), 0);
    }
  }

  static bool fullAccess;
};

bool GpioMemFallback::fullAccess = false;

TEST_F(GpioMemFallback, PwmClkPadsTimerAreNullUnderGpiomem) {
  if (fullAccess) {
    GTEST_SKIP() << "running with full /dev/mem access (root?) - fallback path not exercised";
  }

  EXPECT_NE(_wiringPiGpio, nullptr) << "GPIO must still map fine via /dev/gpiomem";
  EXPECT_EQ(_wiringPiPwm, nullptr)   << "PWM would alias into GPIO registers under /dev/gpiomem - must stay NULL (#446)";
  EXPECT_EQ(_wiringPiClk, nullptr)   << "CLK would alias into GPIO registers under /dev/gpiomem - must stay NULL (#446)";
  EXPECT_EQ(_wiringPiPads, nullptr)  << "PADS would alias into GPIO registers under /dev/gpiomem - must stay NULL (#446)";
  EXPECT_EQ(_wiringPiTimer, nullptr) << "TIMER would alias into GPIO registers under /dev/gpiomem - must stay NULL (#446)";
}

TEST_F(GpioMemFallback, PinModePwmAbortsInsteadOfCorruptingRegisters) {
  if (fullAccess) {
    GTEST_SKIP() << "running with full /dev/mem access (root?) - fallback path not exercised";
  }

  // Before the fix, this call would have gone on to write through the
  // (aliased) pwm/clk pointers straight into live GPIO registers instead
  // of aborting.
  EXPECT_EXIT(pinMode(1, PWM_OUTPUT), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Unable to do this when using /dev/gpiomem");
}

TEST_F(GpioMemFallback, SetPadDriveIsNoOpInsteadOfHittingGpclr1) {
  if (fullAccess) {
    GTEST_SKIP() << "running with full /dev/mem access (root?) - fallback path not exercised";
  }

  // Before the fix this wrote *(pads + 11), byte offset 0x2C, which aliases
  // GPCLR1 (GPIO output-clear for pins 32-63) and could force real pins low.
  // With pads == NULL it must just warn and return.
  testing::internal::CaptureStderr();
  setPadDrive(0, 0);
  std::string err = testing::internal::GetCapturedStderr();
  EXPECT_NE(err.find("no pads memory available, ignoring"), std::string::npos);
}
