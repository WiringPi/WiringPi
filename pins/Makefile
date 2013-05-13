
SRC	=	pins.tex


all:		${SRC}
	@echo	Generating DVI
	@latex  pins.tex

pins.dvi:	pins.tex
	@latex  pins.tex

pdf:	pins.dvi
	@dvipdf pins.dvi


.PHONEY:	clean
clean:
	@rm -f *.dvi *.aux *.log *.ps *.toc *.bak *~
