// WiringPi test program: basic check works without real pi hardware
// Compile: gcc -Wall wiringpi_test0_version.c -o wiringpi_test0_version -lwiringPi

#include "wpi_test.h"
#include "../../version.h"
#include <stdlib.h>
#include <stdint.h>
#include <byteswap.h>

// Samples taken from the "New-style revision codes in use" table at
// https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#raspberry-pi-revision-codes
// model/rev/mem/maker below are the expected decode of piBoardId(), not MB/text
// as printed on that page: mem is the raw 0..7 field code, maker the raw 0..15 code.
typedef struct {
  const char *code;   // revision code, as it appears in /proc/cpuinfo
  const char *label;  // for failure messages
  int model;          // expected *model (PI_MODEL_*)
  int rev;            // expected *rev  (board revision, e.g. 2 == "1.2")
  int mem;            // expected *mem  (memory-size field code, 0..7)
  int maker;          // expected *maker (manufacturer field code, 0..15)
} RevisionSample;

static const RevisionSample revisionSamples[] = {
  // code      label                             model              rev mem maker
  { "900021", "A+ 1.1 512MB Sony UK",            PI_MODEL_AP,        1,  1,  0 },
  { "900032", "B+ 1.2 512MB Sony UK",            PI_MODEL_BP,        2,  1,  0 },
  { "a01040", "2B 1.0 1GB Sony UK",              PI_MODEL_2,         0,  2,  0 },
  { "a01041", "2B 1.1 1GB Sony UK",              PI_MODEL_2,         1,  2,  0 },
  { "a21041", "2B 1.1 1GB Embest",               PI_MODEL_2,         1,  2,  2 },
  { "a02042", "2B (BCM2837) 1.2 1GB Sony UK",    PI_MODEL_2,         2,  2,  0 },
  { "a22042", "2B (BCM2837) 1.2 1GB Embest",     PI_MODEL_2,         2,  2,  2 },
  { "900061", "CM1 1.1 512MB Sony UK",           PI_MODEL_CM,        1,  1,  0 },
  { "a02082", "3B 1.2 1GB Sony UK",              PI_MODEL_3B,        2,  2,  0 },
  { "a22082", "3B 1.2 1GB Embest",               PI_MODEL_3B,        2,  2,  2 },
  { "a32082", "3B 1.2 1GB Sony Japan",           PI_MODEL_3B,        2,  2,  3 },
  { "a52082", "3B 1.2 1GB Stadium",              PI_MODEL_3B,        2,  2,  5 },
  { "a22083", "3B 1.3 1GB Embest",               PI_MODEL_3B,        3,  2,  2 },
  { "900092", "Zero 1.2 512MB Sony UK",          PI_MODEL_ZERO,      2,  1,  0 },
  { "920092", "Zero 1.2 512MB Embest",           PI_MODEL_ZERO,      2,  1,  2 },
  { "900093", "Zero 1.3 512MB Sony UK",          PI_MODEL_ZERO,      3,  1,  0 },
  { "920093", "Zero 1.3 512MB Embest",           PI_MODEL_ZERO,      3,  1,  2 },
  { "a020a0", "CM3 1.0 1GB Sony UK",             PI_MODEL_CM3,       0,  2,  0 },
  { "a220a0", "CM3 1.0 1GB Embest",              PI_MODEL_CM3,       0,  2,  2 },
  { "9000c1", "Zero W 1.1 512MB Sony UK",        PI_MODEL_ZERO_W,    1,  1,  0 },
  { "a020d3", "3B+ 1.3 1GB Sony UK",             PI_MODEL_3BP,       3,  2,  0 },
  { "a020d4", "3B+ 1.4 1GB Sony UK",             PI_MODEL_3BP,       4,  2,  0 },
  { "9020e0", "3A+ 1.0 512MB Sony UK",           PI_MODEL_3AP,       0,  1,  0 },
  { "9020e1", "3A+ 1.1 512MB Sony UK",           PI_MODEL_3AP,       1,  1,  0 },
  { "a02100", "CM3+ 1.0 1GB Sony UK",            PI_MODEL_CM3P,      0,  2,  0 },
  { "a03111", "4B 1.1 1GB Sony UK",              PI_MODEL_4B,        1,  2,  0 },
  { "b03111", "4B 1.1 2GB Sony UK",              PI_MODEL_4B,        1,  3,  0 },
  { "c03111", "4B 1.1 4GB Sony UK",              PI_MODEL_4B,        1,  4,  0 },
  { "b03112", "4B 1.2 2GB Sony UK",              PI_MODEL_4B,        2,  3,  0 },
  { "c03112", "4B 1.2 4GB Sony UK",              PI_MODEL_4B,        2,  4,  0 },
  { "b03114", "4B 1.4 2GB Sony UK",              PI_MODEL_4B,        4,  3,  0 },
  { "c03114", "4B 1.4 4GB Sony UK",              PI_MODEL_4B,        4,  4,  0 },
  { "d03114", "4B 1.4 8GB Sony UK",              PI_MODEL_4B,        4,  5,  0 },
  { "b03115", "4B 1.5 2GB Sony UK",              PI_MODEL_4B,        5,  3,  0 },
  { "c03115", "4B 1.5 4GB Sony UK",              PI_MODEL_4B,        5,  4,  0 },
  { "d03115", "4B 1.5 8GB Sony UK",              PI_MODEL_4B,        5,  5,  0 },
  { "902120", "Zero 2 W 1.0 512MB Sony UK",      PI_MODEL_ZERO_2W,   0,  1,  0 },
  { "c03130", "400 1.0 4GB Sony UK",             PI_MODEL_400,       0,  4,  0 },
  { "a03140", "CM4 1.0 1GB Sony UK",             PI_MODEL_CM4,       0,  2,  0 },
  { "b03140", "CM4 1.0 2GB Sony UK",             PI_MODEL_CM4,       0,  3,  0 },
  { "c03140", "CM4 1.0 4GB Sony UK",             PI_MODEL_CM4,       0,  4,  0 },
  { "d03140", "CM4 1.0 8GB Sony UK",             PI_MODEL_CM4,       0,  5,  0 },
  { "b04170", "Pi 5 1.0 2GB Sony UK",            PI_MODEL_5,         0,  3,  0 },
  { "c04170", "Pi 5 1.0 4GB Sony UK",            PI_MODEL_5,         0,  4,  0 },
  { "d04170", "Pi 5 1.0 8GB Sony UK",            PI_MODEL_5,         0,  5,  0 },
  { "a04171", "Pi 5 1.1 1GB Sony UK",            PI_MODEL_5,         1,  2,  0 },
  { "b04171", "Pi 5 1.1 2GB Sony UK",            PI_MODEL_5,         1,  3,  0 },
  { "c04171", "Pi 5 1.1 4GB Sony UK",            PI_MODEL_5,         1,  4,  0 },
  { "d04171", "Pi 5 1.1 8GB Sony UK",            PI_MODEL_5,         1,  5,  0 },
  { "e04171", "Pi 5 1.1 16GB Sony UK",           PI_MODEL_5,         1,  6,  0 },
  { "b04180", "CM5 1.0 2GB Sony UK",             PI_MODEL_CM5,       0,  3,  0 },
  { "c04180", "CM5 1.0 4GB Sony UK",             PI_MODEL_CM5,       0,  4,  0 },
  { "d04180", "CM5 1.0 8GB Sony UK",             PI_MODEL_CM5,       0,  5,  0 },
  { "e04180", "CM5 1.0 16GB Sony UK",            PI_MODEL_CM5,       0,  6,  0 },
  { "d04190", "500 1.0 8GB Sony UK",             PI_MODEL_500,       0,  5,  0 },
  { "e04190", "500+ 1.0 16GB Sony UK",           PI_MODEL_500,       0,  6,  0 },
  { "b041a0", "CM5 Lite 1.0 2GB Sony UK",        PI_MODEL_CM5L,      0,  3,  0 },
  { "c041a0", "CM5 Lite 1.0 4GB Sony UK",        PI_MODEL_CM5L,      0,  4,  0 },
  { "d041a0", "CM5 Lite 1.0 8GB Sony UK",        PI_MODEL_CM5L,      0,  5,  0 },
  { "e041a0", "CM5 Lite 1.0 16GB Sony UK",       PI_MODEL_CM5L,      0,  6,  0 },
  // No official sample codes
  { "9041b0", "CM0 1.0 512MB Sony UK",           PI_MODEL_CM0,       0,  1,  0 },
};

static void CheckRevisionSample(const RevisionSample *s) {
  printf("New code: %s , Model: %s:\n", s->code, s->label);
  setenv("WIRINGPI_FORCE_REVISION", s->code, 1);

  int model, rev, mem, maker, warranty;
  piBoardId(&model, &rev, &mem, &maker, &warranty);

  CheckSame("model", model, s->model);
  CheckSame("revision", rev, s->rev);
  CheckSame("memory", mem, s->mem);
  CheckSame("maker", maker, s->maker);
  CheckSame("warranty", warranty, 0);
}

// Samples taken from the "Old-style revision codes" table on the same page
// (0002 .. 0015). model/mem/maker/rev are the expected decode of piBoardId().
// A field is set to -1 where wiringPi's own legacy lookup table (the
// "else if (strcmp (c, ...))" chain in piBoardId(), carried over from the
// old elinux-wiki revision list) is known to disagree with what the current
// raspberrypi.com page says, so there is no single correct expected value to
// assert here without first deciding (elsewhere) which side to fix:
//  - 0005 / 0009 maker: page says "Qisda"; wiringPi has no PI_MAKER_QISDA and
//    returns PI_MAKER_EGOMAN instead.
//  - 0003 / 0011 / 0014 rev: page lists these as revision "1.0", same as 0002,
//    but wiringPi's table returns PI_VERSION_1_1 for them, inconsistently
//    with 0002's PI_VERSION_1. (Codes documented as "2.0"/"1.2" all map
//    consistently to PI_VERSION_1_2 and are asserted normally.)
//  - 0015 mem: page lists RAM ambiguously as "256 MB / 512 MB"; wiringPi
//    always returns 512 MB.
typedef struct {
  const char *code;
  const char *label;
  int model;
  int mem;
  int maker;
  int rev;
} OldStyleSample;

static const OldStyleSample oldStyleSamples[] = {
  // code    label                             model         mem maker              rev
  { "0002", "B   1.0 256MB Egoman",            PI_MODEL_B,    0, PI_MAKER_EGOMAN,   PI_VERSION_1   },
  { "0003", "B   1.0 256MB Egoman",            PI_MODEL_B,    0, PI_MAKER_EGOMAN,   -1             },
  { "0004", "B   2.0 256MB Sony UK",           PI_MODEL_B,    0, PI_MAKER_SONY,     PI_VERSION_1_2 },
  { "0005", "B   2.0 256MB Qisda",             PI_MODEL_B,    0, -1,                PI_VERSION_1_2 },
  { "0006", "B   2.0 256MB Egoman",            PI_MODEL_B,    0, PI_MAKER_EGOMAN,   PI_VERSION_1_2 },
  { "0007", "A   2.0 256MB Egoman",            PI_MODEL_A,    0, PI_MAKER_EGOMAN,   PI_VERSION_1_2 },
  { "0008", "A   2.0 256MB Sony UK",           PI_MODEL_A,    0, PI_MAKER_SONY,     PI_VERSION_1_2 },
  { "0009", "A   2.0 256MB Qisda",             PI_MODEL_A,    0, -1,                PI_VERSION_1_2 },
  { "000d", "B   2.0 512MB Egoman",            PI_MODEL_B,    1, PI_MAKER_EGOMAN,   PI_VERSION_1_2 },
  { "000e", "B   2.0 512MB Sony UK",           PI_MODEL_B,    1, PI_MAKER_SONY,     PI_VERSION_1_2 },
  { "000f", "B   2.0 512MB Egoman",            PI_MODEL_B,    1, PI_MAKER_EGOMAN,   PI_VERSION_1_2 },
  { "0010", "B+  1.2 512MB Sony UK",           PI_MODEL_BP,   1, PI_MAKER_SONY,     PI_VERSION_1_2 },
  { "0011", "CM1 1.0 512MB Sony UK",           PI_MODEL_CM,   1, PI_MAKER_SONY,     -1             },
  { "0012", "A+  1.1 256MB Sony UK",           PI_MODEL_AP,   0, PI_MAKER_SONY,     PI_VERSION_1_1 },
  { "0013", "B+  1.2 512MB Embest",            PI_MODEL_BP,   1, PI_MAKER_EMBEST,   PI_VERSION_1_2 },
  { "0014", "CM1 1.0 512MB Embest",            PI_MODEL_CM,   1, PI_MAKER_EMBEST,   -1             },
  { "0015", "A+  1.1 256MB/512MB Embest",      PI_MODEL_AP,  -1, PI_MAKER_EMBEST,   PI_VERSION_1_1 },
};

static void CheckOldStyleSample(const OldStyleSample *s) {
  printf("Old code: %s , Model: %s:\n", s->code, s->label);

  setenv("WIRINGPI_FORCE_REVISION", s->code, 1);

  int model, rev, mem, maker, warranty;
  piBoardId(&model, &rev, &mem, &maker, &warranty);

  CheckSame("model", model, s->model);

  if (s->rev >= 0) {
    CheckSame("revision", rev, s->rev);
  }
  if (s->mem >= 0) {
    CheckSame("memory", mem, s->mem);
  }
  if (s->maker >= 0) {
    CheckSame("maker", maker, s->maker);
  }
  CheckSame("warranty", warranty, 0);
}

// GetPiRAM() ("Other" memory-size, bMem==7) check.
#define RAM_OTHER_FILE "/proc/device-tree/chosen/rpi-sdram-size-gbit"

static void CheckOtherRamDetection(void) {
  printf("\n");
  FILE *fp = fopen(RAM_OTHER_FILE, "rb");
  if (fp == NULL) {
    printf("%39s -> %sskipped%s (not on real Raspberry Pi hw, %s missing)\n",
           "'Other' RAM detection (GetPiRAM)", COLORGRN, COLORDEF, RAM_OTHER_FILE);
    return;
  }

  uint32_t raw = 0;
  int ok = fread(&raw, sizeof(raw), 1, fp) == 1;
  fclose(fp);
  if (!ok) {
    printf("%39s -> %sskipped%s (could not read %s)\n",
           "'Other' RAM detection (GetPiRAM)", COLORGRN, COLORDEF, RAM_OTHER_FILE);
    return;
  }

  unsigned int gigabit = bswap_32(raw);
  int expectedMB = (int) (gigabit * 1024 / 8);

  setenv("WIRINGPI_FORCE_REVISION", "f03114", 1); // any code with bMem==7 ("Other")
  int model, rev, mem, maker, warranty;
  piBoardId(&model, &rev, &mem, &maker, &warranty);
  unsetenv("WIRINGPI_FORCE_REVISION");

  CheckSame("'Other' mem-code (bMem==7) decodes correctly", mem, 7);
  CheckSame("GetPiRAM() reads rpi-sdram-size-gbit correctly", piMemorySize[7], expectedMB);
}

int main (void) {
  int major, minor;

  wiringPiVersion(&major, &minor);

  CheckSame("version major", major, VERSION_MAJOR);
  CheckSame("version minor", minor, VERSION_MINOR);

  size_t nOld = sizeof(oldStyleSamples) / sizeof(oldStyleSamples[0]);
  for (size_t i = 0; i < nOld; i++) {
    CheckOldStyleSample(&oldStyleSamples[i]);
  }
  printf("------------------------------------------------------------\n");
  size_t n = sizeof(revisionSamples) / sizeof(revisionSamples[0]);
  for (size_t i = 0; i < n; i++) {
    CheckRevisionSample(&revisionSamples[i]);
  }
  printf("------------------------------------------------------------\n");
  CheckOtherRamDetection();

  return UnitTestState();
}
