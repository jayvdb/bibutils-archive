#
# bib2ris MAKEFILE
#

#CC         = cc
INCLUDEDIR = -I ../include
LIBDIR     = -L../lib
LIBS       = -lnewstr

bib2ris : bib2ris.o 
	$(CC) $(LIBDIR) bib2ris.o -o $@  $(LIBS)

bib2ris.o : bib2ris.c
	$(CC) $(INCLUDEDIR) bib2ris.c -c -o bib2ris.o

clean:
	rm -f *.o core

realclean:
	rm -f *.o core bib2ris

