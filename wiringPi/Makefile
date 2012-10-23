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

DYN_VERS_MAJ=1
DYN_VERS_MIN=0

STATIC=libwiringPi.a
DYNAMIC=libwiringPi.so.$(DYN_VERS_MAJ).$(DYN_VERS_MIN)

#DEBUG	= -g -O0
DEBUG	= -O2
CC	= gcc
INCLUDE	= -I.
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -fPIC

LIBS    =

# Should not alter anything below this line
###############################################################################

SRC	=	wiringPi.c wiringPiFace.c wiringSerial.c wiringShift.c	\
		gertboard.c						\
		piNes.c							\
		lcd.c piHiPri.c piThread.c softPwm.c wiringPiSPI.c

OBJ	=	$(SRC:.c=.o)

#all:		$(STATIC) $(DYNAMIC)
all:		$(DYNAMIC)

$(STATIC):	$(OBJ)
	@echo [Link (Static)]
	@ar rcs $(STATIC) $(OBJ)
	@ranlib $(STATIC)
	@size   $(STATIC)

$(DYNAMIC):	$(OBJ)
	@echo [Link]
	@$(CC) -shared -Wl,-soname,libwiringPi.so.1 -o libwiringPi.so.1.0 -lpthread $(OBJ)

.c.o:
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONEY:	clean
clean:
	rm -f $(OBJ) *~ core tags Makefile.bak libwiringPi.*

.PHONEY:	tags
tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

.PHONEY:	install
install:	$(TARGET)
	@echo "[Install]"
	@install -m 0755 -d /usr/local/lib
	@install -m 0755 -d /usr/local/include
	@install -m 0644 wiringPi.h	/usr/local/include
	@install -m 0644 wiringSerial.h	/usr/local/include
	@install -m 0644 wiringShift.h	/usr/local/include
	@install -m 0644 gertboard.h	/usr/local/include
	@install -m 0644 piNes.h		/usr/local/include
	@install -m 0644 softPwm.h	/usr/local/include
	@install -m 0644 lcd.h		/usr/local/include
	@install -m 0644 wiringPiSPI.h	/usr/local/include
#	@install -m 0644 libwiringPi.a	/usr/local/lib
	@install	-m 0755 libwiringPi.so.1.0 /usr/local/lib
	@ln -sf /usr/local/lib/libwiringPi.so.1.0 /usr/local/lib/libwiringPi.so
	@ln -sf /usr/local/lib/libwiringPi.so.1.0 /usr/local/lib/libwiringPi.so.1
	@ldconfig

.PHONEY:	uninstall
uninstall:
	@echo "[UnInstall]"
	@rm -f /usr/local/include/wiringPi.h
	@rm -f /usr/local/include/wiringSerial.h
	@rm -f /usr/local/include/wiringShift.h
	@rm -f /usr/local/include/gertboard.h
	@rm -f /usr/local/include/piNes.h
	@rm -f /usr/local/include/softPwm.h
	@rm -f /usr/local/include/lcd.h
	@rm -f /usr/local/include/wiringPiSPI.h
	@rm -f /usr/local/lib/libwiringPi.*
	@ldconfig


.PHONEY:	depend
depend:
	makedepend -Y $(SRC)

# DO NOT DELETE

wiringPi.o: wiringPi.h
wiringPiFace.o: wiringPi.h
wiringSerial.o: wiringSerial.h
wiringShift.o: wiringPi.h wiringShift.h
gertboard.o: wiringPiSPI.h gertboard.h
piNes.o: wiringPi.h piNes.h
lcd.o: wiringPi.h lcd.h
piHiPri.o: wiringPi.h
piThread.o: wiringPi.h
softPwm.o: wiringPi.h softPwm.h
wiringPiSPI.o: wiringPiSPI.h
