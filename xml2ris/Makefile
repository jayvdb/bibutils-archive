#
# xml2ris MAKEFILE
#

INCLUDEDIR = -I ../include -I ../lib
LIBDIR     = -L../lib
LIBS       = -lnewstr

all : xml2ris

xml2ris : xml2ris.o 
	$(CC) $(LIBDIR) xml2ris.o -o $@  $(LIBS)

xml2ris.o : xml2ris.c
	$(CC) $(INCLUDEDIR) xml2ris.c -c -o xml2ris.o

clean:
	rm -f *.o core

realclean:
	rm -f *.o core xml2ris

test:
