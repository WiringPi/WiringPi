#ifndef _CPI_H_
#define _CPI_H_

extern int wiringPiSetupRaw (void);
extern void CPiBoardId (int *model, int *rev, int *mem, int *maker, int *warranty);
extern int CPi_get_gpio_mode(int pin);
extern int CPi_digitalRead(int pin);
extern void CPi_digitalWrite(int pin, int value);
extern void CPiReadAll(void);
extern void CPiReadAllRaw(void);

#endif
