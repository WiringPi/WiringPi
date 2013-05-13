#
# Makefile:
#	Gertboard - Examples using wiringPi
#
#	Copyright (c) 2013 Gordon Henderson
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

SRC	=	gertboard.c				\
		buttons.c 7segments.c			\
		voltmeter.c temperature.c vumeter.c	\
		record.c

OBJ	=	$(SRC:.c=.o)

BINS	=	$(SRC:.c=)

all:	$(BINS)

gertboard:	gertboard.o
	@echo [link]
	@$(CC) -o $@ gertboard.o $(LDFLAGS) $(LDLIBS)

buttons:	buttons.o
	@echo [link]
	@$(CC) -o $@ buttons.o $(LDFLAGS) $(LDLIBS)

7segments:	7segments.o
	@echo [link]
	@$(CC) -o $@ 7segments.o $(LDFLAGS) $(LDLIBS)

voltmeter:	voltmeter.o
	@echo [link]
	@$(CC) -o $@ voltmeter.o $(LDFLAGS) $(LDLIBS)

temperature:	temperature.o
	@echo [link]
	@$(CC) -o $@ temperature.o $(LDFLAGS) $(LDLIBS)

vumeter:	vumeter.o
	@echo [link]
	@$(CC) -o $@ vumeter.o $(LDFLAGS) $(LDLIBS)

record:	record.o
	@echo [link]
	@$(CC) -o $@ record.o $(LDFLAGS) $(LDLIBS)

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo [Clean]
	@rm -f $(OBJ) *~ core tags $(BINS)

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
