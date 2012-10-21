#!/bin/bash
#
# test.sh:
#	Simple test: Assumes LEDs on Pins 0-7 and lights them
#	in-turn.
#################################################################################
# This file is part of wiringPi:
#	Wiring Compatable library for the Raspberry Pi
#
#    wiringPi is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    wiringPi is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
#################################################################################

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
