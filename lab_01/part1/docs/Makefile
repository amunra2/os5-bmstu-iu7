.PHONY: all clean

FILE = report

all:
	pdflatex $(FILE).tex

	#bibtex $(FILE).aux

	pdflatex $(FILE).tex
	pdflatex $(FILE).tex
	
	xdg-open $(FILE).pdf

clean:
	rm *.aux *.bbl *.blg *.log *.out
