
SRC	=	pins.tex

ifneq ($V,1)
Q ?= @
endif


all:		${SRC}
	$Q echo	Generating DVI
	$Q latex  pins.tex

pins.dvi:	pins.tex
	$Q latex  pins.tex

pdf:	pins.dvi
	$Q dvipdf pins.dvi


.PHONY:	clean
clean:
	$Q rm -f *.dvi *.aux *.log *.ps *.toc *.bak *~
