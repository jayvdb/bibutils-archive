#
# bib2xml MAKEFILE
#

INCLUDEDIR = -I ../include -I ../lib
LIBDIR     = -L../lib
LIBS       = -lnewstr 

bib2xml : bib2xml.o bibstr.o
	$(CC) $(CFLAGS) $(LIBDIR) bib2xml.o bibstr.o -o $@  $(LIBS)

bib2xml.o : bib2xml.c
	$(CC) $(CFLAGS) $(INCLUDEDIR) bib2xml.c -c -o bib2xml.o

bibstr.o : bibstr.c
	$(CC) $(CFLAGS) $(INCLUDEDIR) bibstr.c -c -o bibstr.o

test: bib2xml
	csh -f test.com

clean:
	rm -f *.o core 

realclean:
	rm -f *.o core bib2xml
