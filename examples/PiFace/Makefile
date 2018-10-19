#
# Makefile:
#	wiringPi - A "wiring" library for the Raspberry Pi
#	https://projects.drogon.net/wiring-pi
#
#	Copyright (c) 2012 Gordon Henderson
#################################################################################
# This file is part of wiringPi:
#	A "wiring" library for the Raspberry Pi
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
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm

# Should not alter anything below this line
###############################################################################

SRC	=	blink.c buttons.c reaction.c ladder.c metro.c motor.c

OBJ	=	$(SRC:.c=.o)

BINS	=	$(SRC:.c=)

all:	$(BINS)

blink:	blink.o
	$Q echo [link]
	$Q $(CC) -o $@ blink.o $(LDFLAGS) $(LDLIBS)

buttons:	buttons.o
	$Q echo [link]
	$Q $(CC) -o $@ buttons.o $(LDFLAGS) $(LDLIBS)

reaction:	reaction.o
	$Q echo [link]
	$Q $(CC) -o $@ reaction.o $(LDFLAGS) $(LDLIBS)

ladder:	ladder.o
	$Q echo [link]
	$Q $(CC) -o $@ ladder.o $(LDFLAGS) $(LDLIBS)

metro:	metro.o
	$Q echo [link]
	$Q $(CC) -o $@ metro.o $(LDFLAGS) $(LDLIBS)

motor:	motor.o
	$Q echo [link]
	$Q $(CC) -o $@ motor.o $(LDFLAGS) $(LDLIBS)

.c.o:
	$Q echo [CC] $<
	$Q $(CC) -c $(CFLAGS) $< -o $@

clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJ) *~ core tags $(BINS)

tags:	$(SRC)
	$Q echo [ctags]
	$Q ctags $(SRC)

depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
