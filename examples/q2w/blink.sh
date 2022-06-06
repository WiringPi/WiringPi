#!/bin/sh -e
#
# blink.sh:
#	Standard "blink" program in wiringPi. Blinks an LED connected
#	to the LED on the Quick2Wire board
#
# Copyright (c) 2012-2013 Gordon Henderson.
#######################################################################
# This file is part of wiringPi:
#       https://github.com/WiringPi/WiringPi
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
#######################################################################

# LED Pin - wiringPi pin 1 is BCM_GPIO 18.

LED=1

gpio mode $LED out

while true; do
  gpio write $LED 1
  sleep 0.5
  gpio write $LED 0
  sleep 0.5
done
