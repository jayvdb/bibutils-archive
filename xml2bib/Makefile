#
# xml2bib MAKEFILE
#

#CC         = cc
INCLUDEDIR = -I ../include
LIBDIR     = -L../lib
LIBS       = -lnewstr

all : xml2bib

xml2bib : xml2bib.o 
	$(CC) $(LIBDIR) xml2bib.o -o $@  $(LIBS)

xml2bib.o : xml2bib.c
	$(CC) $(INCLUDEDIR) xml2bib.c -c -o xml2bib.o

clean:
	rm -f *.o core

test:

realclean:
	rm -f *.o core xml2bib
