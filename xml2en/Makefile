#
# xml2en MAKEFILE
#

#CC         = cc
INCLUDEDIR = -I ../include
LIBDIR     = -L../lib
LIBS       = -lnewstr

all : xml2en

xml2en : xml2en.o 
	$(CC) $(LIBDIR) xml2en.o -o $@  $(LIBS)

xml2en.o : xml2en.c
	$(CC) $(INCLUDEDIR) xml2en.c -c -o xml2en.o

clean:
	rm -f *.o core

test:

realclean:
	rm -f *.o core xml2en
