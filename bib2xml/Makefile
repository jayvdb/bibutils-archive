#
# bib2xml MAKEFILE
#

#CC         = cc
#CFLAGS      = -Wall
INCLUDEDIR = -I ../include
LIBDIR     = -L../lib
LIBS       = -lnewstr 

bib2xml : bib2xml.o 
	$(CC) $(CFLAGS) $(LIBDIR) bib2xml.o -o $@  $(LIBS)

bib2xml.o : bib2xml.c
	$(CC) $(CFLAGS) $(INCLUDEDIR) bib2xml.c -c -o bib2xml.o

test:

clean:
	rm -f *.o core 

realclean:
	rm -f *.o core bib2xml
