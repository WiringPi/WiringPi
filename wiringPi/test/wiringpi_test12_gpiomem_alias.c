#include "wpi_test.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <unistd.h>

/*
 * Root-cause demonstration for #446, deliberately NOT using wiringPi's own
 * setup - it mmaps /dev/gpiomem twice by hand, exactly like the old (buggy)
 * wiringPiSetup() did: once at the GPIO offset, once at the TIMER offset.
 *
 * The bcm2835-gpiomem kernel driver ignores the mmap offset entirely and
 * always remaps the same one GPIO register page, no matter what offset is
 * requested. So the "timer" mapping below is not the real system timer at
 * all - it is the exact same physical page as "gpio". A write through one
 * pointer must therefore be visible through the other.
 *
 * BCM only (Pi 0-4). RP1 (Pi 5) has a real, offset-respecting /dev/gpiomem0
 * and is not affected.
 */

#define GPIOMEM_DEVICE     "/dev/gpiomem"
#define BLOCK_SIZE         (4*1024)
#define GPIO_BASE_OFFSET   0x00200000u  // BCM GPIO registers,   offset from GPIO peripheral base
#define GPIO_TIMER_OFFSET  0x0000B000u  // BCM system timer regs, offset from GPIO peripheral base
#define TIMER_CONTROL      (0x408 >> 2) // word indices used by the old wiringPiSetup() timer init
#define TIMER_PRE_DIV      (0x41C >> 2)

int main (void) {
  int RaspberryPiModel = -1;
	int rev, mem, maker, overVolted;

	piBoardId(&RaspberryPiModel, &rev, &mem, &maker, &overVolted);
  if (-1==RaspberryPiModel || piRP1Model()) {
    printf("Test only valid for Pi0-4 models!");
    return UnitTestState();
  }


  int fd = open(GPIOMEM_DEVICE, O_RDWR | O_SYNC | O_CLOEXEC);
  if (fd < 0)
    FailAndExitWithErrno("open " GPIOMEM_DEVICE, fd);

  volatile uint32_t *gpio = (volatile uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_BASE_OFFSET);
  if (gpio == MAP_FAILED)
    FailAndExitWithErrno("mmap GPIO", -1);

  volatile uint32_t *timer = (volatile uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_TIMER_OFFSET);
  if (timer == MAP_FAILED)
    FailAndExitWithErrno("mmap TIMER", -1);

  close(fd); // both mappings stay valid after close()

  uint32_t gpioBefore, timerBefore, gpioAfter;

  // --- TIMER_CONTROL, byte offset 0x408 ---
  gpioBefore  = gpio[TIMER_CONTROL];
  timerBefore = timer[TIMER_CONTROL];
  printf("TIMER_CONTROL before write: gpio=0x%08x  timer=0x%08x\n", gpioBefore, timerBefore);
  CheckSame("TIMER_CONTROL: before write, timer view == gpio view (same page)", (int)timerBefore, (int)gpioBefore);

  // Access 0x408, far beyond last gpio address, bug until 3.20  
  // Last Pi0-3: 0x9C (GPPUDCLK1)
  // Last Pi4  : 0xF0 (GPIO_PUP_PDN_CNTRL_REG3)
  *(timer + TIMER_CONTROL) = 0x0000280;

  gpioAfter = gpio[TIMER_CONTROL];
  printf("TIMER_CONTROL after write:  gpio=0x%08x  timer=0x%08x\n", gpioAfter, timer[TIMER_CONTROL]);
  CheckSame("TIMER_CONTROL: after write via timer, gpio view sees the same value (aliased)", (int)gpioAfter, 0x0000280);

  *(timer + TIMER_CONTROL) = gpioBefore;
  printf("TIMER_CONTROL after restore: gpio=0x%08x\n", gpio[TIMER_CONTROL]);
  CheckSame("TIMER_CONTROL: after restore, gpio view back to original value", (int)gpio[TIMER_CONTROL], (int)gpioBefore);


  // --- TIMER_PRE_DIV, byte offset 0x41C ---
  gpioBefore  = gpio[TIMER_PRE_DIV];
  timerBefore = timer[TIMER_PRE_DIV];
  printf("TIMER_PRE_DIV before write: gpio=0x%08x  timer=0x%08x\n", gpioBefore, timerBefore);
  CheckSame("TIMER_PRE_DIV: before write, timer view == gpio view (same page)", (int)timerBefore, (int)gpioBefore);

  // Access 0x41C far beyond last gpio address, bug until 3.20
  // Last Pi0-3: 0x9C (GPPUDCLK1)
  // Last Pi4  : 0xF0 (GPIO_PUP_PDN_CNTRL_REG3)
  *(timer + TIMER_PRE_DIV) = 0x00000F9;

  gpioAfter = gpio[TIMER_PRE_DIV];
  printf("TIMER_PRE_DIV after write:  gpio=0x%08x  timer=0x%08x\n", gpioAfter, timer[TIMER_PRE_DIV]);
  CheckSame("TIMER_PRE_DIV: after write via timer, gpio view sees the same value (aliased)", (int)gpioAfter, 0x00000F9);

  *(timer + TIMER_PRE_DIV) = gpioBefore;
  printf("TIMER_PRE_DIV after restore: gpio=0x%08x\n", gpio[TIMER_PRE_DIV]);
  CheckSame("TIMER_PRE_DIV: after restore, gpio view back to original value", (int)gpio[TIMER_PRE_DIV], (int)gpioBefore);

  munmap((void *)gpio, BLOCK_SIZE);
  munmap((void *)timer, BLOCK_SIZE);

  return UnitTestState();
}
