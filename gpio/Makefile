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
DEBUG	= -O2
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LIBS    = -lwiringPi -lpthread -lm

# May not need to  alter anything below this line
###############################################################################

SRC	=	gpio.c

OBJ	=	$(SRC:.c=.o)

all:		gpio

gpio:	gpio.o
	@echo [Link]
	@$(CC) -o $@ gpio.o $(LDFLAGS) $(LIBS)
	
.c.o:
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONEY:	clean
clean:
	rm -f $(OBJ) gpio *~ core tags *.bak

.PHONEY:	tags
tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

.PHONEY:	install
install:
	@echo "[Install]"
	@cp gpio /usr/local/bin
	@chown root.root /usr/local/bin/gpio
	@chmod 4755 /usr/local/bin/gpio
	@mkdir -p /usr/local/man/man1
	@cp gpio.1 /usr/local/man/man1

.PHONEY:	uninstall
uninstall:
	@echo "[UnInstall]"
	@rm -f /usr/local/bin/gpio
	@rm -f /usr/local/man/man1/gpio.1

.PHONEY:	depend
depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
