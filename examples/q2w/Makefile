#
# Makefile:
#	wiringPi - A "wiring" library for the Raspberry Pi
#	https://projects.drogon.net/wiring-pi
#
#	Copyright (c) 2012-2013 Gordon Henderson
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
CC	?= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe $(EXTRA_CFLAGS)

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm

###############################################################################

SRC	=	blink.c button.c blink-io.c volts.c bright.c

OBJ	=	$(SRC:.c=.o)

BINS	=	$(SRC:.c=)

all:	$(BINS)

blink:	blink.o
	$Q echo [link]
	$Q $(CC) -o $@ blink.o $(LDFLAGS) $(LDLIBS)

blink-io:	blink-io.o
	$Q echo [link]
	$Q $(CC) -o $@ blink-io.o $(LDFLAGS) $(LDLIBS)

button:	button.o
	$Q echo [link]
	$Q $(CC) -o $@ button.o $(LDFLAGS) $(LDLIBS)

volts:	volts.o
	$Q echo [link]
	$Q $(CC) -o $@ volts.o $(LDFLAGS) $(LDLIBS)

bright:	bright.o
	$Q echo [link]
	$Q $(CC) -o $@ bright.o $(LDFLAGS) $(LDLIBS)


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
