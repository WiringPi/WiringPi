#
# Makefile:
#	wiringPi - A "wiring" library for the Raspberry Pi
#	https://github.com/wiringPi/wiringPi
#
#	Copyright (c) 2012-2015 Gordon Henderson
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

ifneq ($V,1)
Q ?= @
endif

#DEBUG	= -g -O0
DEBUG	= -O3
CC	?= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe $(EXTRA_CFLAGS)

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm -lcrypt -lrt

# Should not alter anything below this line
###############################################################################

SRC	= \
  blink12.c blink12drcs.c blink6drcs.c blink8.c blink8-drcn.c blink.c blink-thread.c \
  buttons.c buttons2.c buttons3.c buttons4.c \
  clock.c \
  delayTest.c \
  ds1302.c \
  getBanks.c \
  isr1.c isr.c isr-osc.c \
  lcd-adafruit.c lcd.c \
  lowPower.c \
  max31855.c \
  nes.c \
  okLed.c \
  pwm.c \
  rht03.c \
  serialRead.c serialTest.c \
  servo.c \
  softPwm.c softTone.c \
  speed.c spiSpeed.c \
  wfi.c

OBJ	=	$(SRC:.c=.o)

BINS	=	$(SRC:.c=)

all:	
	$Q cat README.TXT
	$Q echo "    $(BINS)" | fmt
	$Q echo ""

really-all:	$(BINS)

blink:	blink.o
	$Q echo [link]
	$Q $(CC) -o $@ blink.o $(LDFLAGS) $(LDLIBS)

blink8:	blink8.o
	$Q echo [link]
	$Q $(CC) -o $@ blink8.o $(LDFLAGS) $(LDLIBS)

blink12drcs:	blink12drcs.o
	$Q echo [link]
	$Q $(CC) -o $@ blink12drcs.o $(LDFLAGS) $(LDLIBS)

blink12:	blink12.o
	$Q echo [link]
	$Q $(CC) -o $@ blink12.o $(LDFLAGS) $(LDLIBS)

speed:	speed.o
	$Q echo [link]
	$Q $(CC) -o $@ speed.o $(LDFLAGS) $(LDLIBS)

lcd:	lcd.o
	$Q echo [link]
	$Q $(CC) -o $@ lcd.o $(LDFLAGS) $(LDLIBS)

lcd-adafruit:	lcd-adafruit.o
	$Q echo [link]
	$Q $(CC) -o $@ lcd-adafruit.o $(LDFLAGS) $(LDLIBS)

clock:	clock.o
	$Q echo [link]
	$Q $(CC) -o $@ clock.o $(LDFLAGS) $(LDLIBS)

wfi:	wfi.o
	$Q echo [link]
	$Q $(CC) -o $@ wfi.o $(LDFLAGS) $(LDLIBS)

isr:	isr.o
	$Q echo [link]
	$Q $(CC) -o $@ isr.o $(LDFLAGS) $(LDLIBS)

isr-osc:	isr-osc.o
	$Q echo [link]
	$Q $(CC) -o $@ isr-osc.o $(LDFLAGS) $(LDLIBS)

nes:	nes.o
	$Q echo [link]
	$Q $(CC) -o $@ nes.o $(LDFLAGS) $(LDLIBS) 

rht03:	rht03.o
	$Q echo [link]
	$Q $(CC) -o $@ rht03.o $(LDFLAGS) $(LDLIBS) 

pwm:	pwm.o
	$Q echo [link]
	$Q $(CC) -o $@ pwm.o $(LDFLAGS) $(LDLIBS)

softPwm:	softPwm.o
	$Q echo [link]
	$Q $(CC) -o $@ softPwm.o $(LDFLAGS) $(LDLIBS)

softTone:	softTone.o
	$Q echo [link]
	$Q $(CC) -o $@ softTone.o $(LDFLAGS) $(LDLIBS)

delayTest:	delayTest.o
	$Q echo [link]
	$Q $(CC) -o $@ delayTest.o $(LDFLAGS) $(LDLIBS)

serialRead:	serialRead.o
	$Q echo [link]
	$Q $(CC) -o $@ serialRead.o $(LDFLAGS) $(LDLIBS)

serialTest:	serialTest.o
	$Q echo [link]
	$Q $(CC) -o $@ serialTest.o $(LDFLAGS) $(LDLIBS)

okLed:	okLed.o
	$Q echo [link]
	$Q $(CC) -o $@ okLed.o $(LDFLAGS) $(LDLIBS)

tone:	tone.o
	$Q echo [link]
	$Q $(CC) -o $@ tone.o $(LDFLAGS) $(LDLIBS)

servo:	servo.o softServo.o
	$Q echo [link]
	$Q $(CC) -o $@ servo.o softServo.o $(LDFLAGS) $(LDLIBS)

softServo.o: ../wiringPi/softServo.c ../wiringPi/softServo.h
	$Q echo [softServo]
	$Q $(CC) -c $(CFLAGS) $< -o softServo.o

ds1302:	ds1302.o
	$Q echo [link]
	$Q $(CC) -o $@ ds1302.o $(LDFLAGS) $(LDLIBS)

max31855:	max31855.o
	$Q echo [link]
	$Q $(CC) -o $@ max31855.o $(LDFLAGS) $(LDLIBS)

.c.o:
	$Q echo [CC] $<
	$Q $(CC) -c $(CFLAGS) $< -o $@

clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJ) *.o *~ core tags $(BINS)

echo:
	$Q echo "[BINS] $(BINS)"

tags:	$(SRC)
	$Q echo [ctags]
	$Q ctags $(SRC)

depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
