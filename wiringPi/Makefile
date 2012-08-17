#
# Makefile:
#	wiringPi - Wiring Compatable library for the Raspberry Pi
#
#	Copyright (c) 2012 Gordon Henderson
#################################################################################
# This file is part of wiringPi:
#	https://projects.drogon.net/raspberry-pi/wiringpi/
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


TARGET=libwiringPi.a

#DEBUG	= -g -O0
DEBUG	= -O3
CC	= gcc
INCLUDE	= -I.
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LIBS    =

# Should not alter anything below this line
###############################################################################

SRC	=	wiringPi.c wiringPiFace.c wiringSerial.c wiringShift.c lcd.c piHiPri.c piThread.c

OBJ	=	wiringPi.o wiringPiFace.o wiringSerial.o wiringShift.o lcd.o piHiPri.o piThread.o

all:		$(TARGET)

$(TARGET):	$(OBJ)
	@echo [AR] $(OBJ)
	@ar rcs $(TARGET) $(OBJ)
	@ranlib $(TARGET)
	@size   $(TARGET)

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) *~ core tags Makefile.bak

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

depend:
	makedepend -Y $(SRC)

install:	$(TARGET)
	@echo [install]
	install -m 0755 -d /usr/local/lib
	install -m 0755 -d /usr/local/include
	install -m 0644 wiringPi.h	/usr/local/include
	install -m 0644 wiringSerial.h	/usr/local/include
	install -m 0644 wiringShift.h	/usr/local/include
	install -m 0644 lcd.h		/usr/local/include
	install -m 0644 libwiringPi.a	/usr/local/lib

uninstall:
	@echo [uninstall]
	rm -f /usr/local/include/lcd.h
	rm -f /usr/local/include/wiringShift.h
	rm -f /usr/local/include/wiringPi.h
	rm -f /usr/local/lib/libwiringPi.a



# DO NOT DELETE

wiringPi.o: wiringPi.h
wiringPiFace.o: wiringPi.h
wiringSerial.o: wiringSerial.h
wiringShift.o: wiringPi.h wiringShift.h
lcd.o: wiringPi.h lcd.h
piHiPri.o: wiringPi.h
piThread.o: wiringPi.h
