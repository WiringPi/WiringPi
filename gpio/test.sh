#!/bin/bash

# Simple test - assumes LEDs on Pins 0-7.

for i in `seq 0 7`;
do
  gpio mode $i out
done

while true;
do
  for i in `seq 0 7`;
  do
    gpio write $i 1
    sleep 0.1
  done

  for i in `seq 0 7`;
  do
    gpio write $i 0
    sleep 0.1
  done
done
