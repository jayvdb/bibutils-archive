#
# bib2en MAKEFILE
#

# SGI CC
#CC         = cc
INCLUDEDIR = -I ../include
LIBDIR     = -L../lib
LIBS       = -lnewstr
#OBJS = newstr.o

# LINUX (not tested, but likely use gcc)
# CC = gcc
# OBJS = findrepl.o newstr.o

bib2en : bib2en.o 
	$(CC) $(LIBDIR) bib2en.o -o $@  $(LIBS)

bib2en.o : bib2en.c
	$(CC) $(INCLUDEDIR) bib2en.c -c -o bib2en.o

clean:
	rm -f *.o core

realclean:
	rm -f *.o core bib2en

