#
# Makefile:
#	wiringPi - Wiring Compatable library for the Raspberry Pi
#	https://projects.drogon.net/wiring-pi
#
#	Copyright (c) 2012 Gordon Henderson
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


#DEBUG	= -g -O0
DEBUG	= -O3
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm

# Should not alter anything below this line
###############################################################################

SRC	=	blink.c blink8.c blink12.c					\
		blink12drcs.c							\
		pwm.c								\
		speed.c wfi.c isr.c isr-osc.c					\
		lcd.c lcd-adafruit.c clock.c					\
		nes.c								\
		softPwm.c softTone.c 						\
		delayTest.c serialRead.c serialTest.c okLed.c ds1302.c		\
		rht03.c piglow.c

OBJ	=	$(SRC:.c=.o)

BINS	=	$(SRC:.c=)

all:	
	@cat README.TXT
	@echo "    $(BINS)" | fmt
	@echo ""

really-all:	$(BINS)

blink:	blink.o
	@echo [link]
	@$(CC) -o $@ blink.o $(LDFLAGS) $(LDLIBS)

blink8:	blink8.o
	@echo [link]
	@$(CC) -o $@ blink8.o $(LDFLAGS) $(LDLIBS)

blink12drcs:	blink12drcs.o
	@echo [link]
	@$(CC) -o $@ blink12drcs.o $(LDFLAGS) $(LDLIBS)

blink12:	blink12.o
	@echo [link]
	@$(CC) -o $@ blink12.o $(LDFLAGS) $(LDLIBS)

speed:	speed.o
	@echo [link]
	@$(CC) -o $@ speed.o $(LDFLAGS) $(LDLIBS)

lcd:	lcd.o
	@echo [link]
	@$(CC) -o $@ lcd.o $(LDFLAGS) $(LDLIBS)

lcd-adafruit:	lcd-adafruit.o
	@echo [link]
	@$(CC) -o $@ lcd-adafruit.o $(LDFLAGS) $(LDLIBS)

clock:	clock.o
	@echo [link]
	@$(CC) -o $@ clock.o $(LDFLAGS) $(LDLIBS)

wfi:	wfi.o
	@echo [link]
	@$(CC) -o $@ wfi.o $(LDFLAGS) $(LDLIBS)

isr:	isr.o
	@echo [link]
	@$(CC) -o $@ isr.o $(LDFLAGS) $(LDLIBS)

isr-osc:	isr-osc.o
	@echo [link]
	@$(CC) -o $@ isr-osc.o $(LDFLAGS) $(LDLIBS)

nes:	nes.o
	@echo [link]
	@$(CC) -o $@ nes.o $(LDFLAGS) $(LDLIBS) 

rht03:	rht03.o
	@echo [link]
	@$(CC) -o $@ rht03.o $(LDFLAGS) $(LDLIBS) 

pwm:	pwm.o
	@echo [link]
	@$(CC) -o $@ pwm.o $(LDFLAGS) $(LDLIBS)

softPwm:	softPwm.o
	@echo [link]
	@$(CC) -o $@ softPwm.o $(LDFLAGS) $(LDLIBS)

softTone:	softTone.o
	@echo [link]
	@$(CC) -o $@ softTone.o $(LDFLAGS) $(LDLIBS)

delayTest:	delayTest.o
	@echo [link]
	@$(CC) -o $@ delayTest.o $(LDFLAGS) $(LDLIBS)

serialRead:	serialRead.o
	@echo [link]
	@$(CC) -o $@ serialRead.o $(LDFLAGS) $(LDLIBS)

serialTest:	serialTest.o
	@echo [link]
	@$(CC) -o $@ serialTest.o $(LDFLAGS) $(LDLIBS)

okLed:	okLed.o
	@echo [link]
	@$(CC) -o $@ okLed.o $(LDFLAGS) $(LDLIBS)

tone:	tone.o
	@echo [link]
	@$(CC) -o $@ tone.o $(LDFLAGS) $(LDLIBS)

ds1302:	ds1302.o
	@echo [link]
	@$(CC) -o $@ ds1302.o $(LDFLAGS) $(LDLIBS)

piglow:	piglow.o
	@echo [link]
	@$(CC) -o $@ piglow.o $(LDFLAGS) $(LDLIBS)


.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo "[Clean]"
	@rm -f $(OBJ) *~ core tags $(BINS)

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
