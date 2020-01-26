#
# Makefile:
#	Gertboard - Examples using wiringPi
#
#	Copyright (c) 2013 Gordon Henderson
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
	$Q echo [link]
	$Q $(CC) -o $@ gertboard.o $(LDFLAGS) $(LDLIBS)

buttons:	buttons.o
	$Q echo [link]
	$Q $(CC) -o $@ buttons.o $(LDFLAGS) $(LDLIBS)

7segments:	7segments.o
	$Q echo [link]
	$Q $(CC) -o $@ 7segments.o $(LDFLAGS) $(LDLIBS)

voltmeter:	voltmeter.o
	$Q echo [link]
	$Q $(CC) -o $@ voltmeter.o $(LDFLAGS) $(LDLIBS)

temperature:	temperature.o
	$Q echo [link]
	$Q $(CC) -o $@ temperature.o $(LDFLAGS) $(LDLIBS)

vumeter:	vumeter.o
	$Q echo [link]
	$Q $(CC) -o $@ vumeter.o $(LDFLAGS) $(LDLIBS)

record:	record.o
	$Q echo [link]
	$Q $(CC) -o $@ record.o $(LDFLAGS) $(LDLIBS)

.c.o:
	$Q echo [CC] $<
	$Q $(CC) -c $(CFLAGS) $< -o $@

clean:
	$Q echo [Clean]
	$Q rm -f $(OBJ) *~ core tags $(BINS)

tags:	$(SRC)
	$Q echo [ctags]
	$Q ctags $(SRC)

depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
