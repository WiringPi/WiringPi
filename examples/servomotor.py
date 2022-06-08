#! /usr/bin/env python3

import RPi.GPIO as GPIO
import time

servoPIN = 12
GPIO.setmode(GPIO.BCM)
GPIO.setup(servoPIN, GPIO.OUT)

print("Start");
p = GPIO.PWM(servoPIN, 50) # GPIO 12 for PWM with 50Hz

dutyMin=2.1
dutyMid=6.0
dutyMax=10.9

dutyStep=-0.1
dutySet=dutyMid

print("Center");
p.start(dutyMid)
time.sleep(3.0)

try:

  while True:

    print("Duty cycle " + str(dutySet));
    p.ChangeDutyCycle(dutySet)
    time.sleep(0.1)

    dutySet += dutyStep

    if dutySet > dutyMax or dutySet < dutyMin:
        dutyStep = -dutyStep
        dutySet += dutyStep
        dutySet += dutyStep
        print("");
        time.sleep(1)

except KeyboardInterrupt:
  print("\nCenter");
  p.ChangeDutyCycle(dutyMid)
  time.sleep(1.0)
  p.stop()
  GPIO.cleanup()
