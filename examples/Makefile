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
LIBS    = -lwiringPi

# Should not alter anything below this line
###############################################################################

SRC	=	test1.c test2.c speed.c lcd.c wfi.c piface.c

OBJ	=	test1.o test2.o speed.o lcd.o wfi.o piface.o

all:		test1 test2 speed lcd wfi piface

test1:	test1.o
	@echo [link]
	$(CC) -o $@ test1.o $(LDFLAGS) $(LIBS)
	
test2:	test2.o
	@echo [link]
	$(CC) -o $@ test2.o $(LDFLAGS) $(LIBS)

speed:	speed.o
	@echo [link]
	$(CC) -o $@ speed.o $(LDFLAGS) $(LIBS)

lcd:	lcd.o
	@echo [link]
	$(CC) -o $@ lcd.o $(LDFLAGS) $(LIBS)

wfi:	wfi.o
	@echo [link]
	$(CC) -o $@ wfi.o $(LDFLAGS) $(LIBS) -lpthread

piface:	piface.o
	@echo [link]
	$(CC) -o $@ piface.o $(LDFLAGS) $(LIBS) -lpthread


.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) *~ core tags test1 test2 speed lcd wfi piface

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
