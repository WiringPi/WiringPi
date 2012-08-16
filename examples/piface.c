
/*
 * piface.c:
 *	Simple test for the PiFace
 *
 *	Read the buttons and output the same to the LEDs
 */

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int outputs [4] = { 0,0,0,0 } ;

void scanButton (int button)
{
  if (digitalRead (button) == LOW)
  {
    outputs [button] ^= 1 ;
    digitalWrite (button, outputs [button]) ;
  }

  while (digitalRead (button) == LOW)
    delay (1) ;
}


int main (void)
{
  int pin, button ;

  printf ("Raspberry Pi wiringPiFace test program\n") ;

  if (wiringPiSetupPiFace () == -1)
    exit (1) ;

// Enable internal pull-ups

  for (pin = 0 ; pin < 8 ; ++pin)
    pullUpDnControl (pin, PUD_UP) ;


  for (;;)
  {
    for (button = 0 ; button < 4 ; ++button)
      scanButton (button) ;
    delay (1) ;
  }

  return 0 ;
}
