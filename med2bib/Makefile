#
# med2bib MAKEFILE
#

# SGI CC
#CC         = cc
INCLUDEDIR = -I ../include
LIBDIR     = -L../lib
LIBS       = -lnewstr
OBJS       = findrepl.o

# LINUX (not tested, but likely use gcc)
# CC = gcc
# OBJS = findrepl.o newstr.o

med2bib : med2bib.o $(OBJS)
	$(CC) med2bib.o $(LIBDIR) $(OBJS) -o $@ $(LIBS)

med2bib.o : med2bib.c
	$(CC) $(INCLUDEDIR) med2bib.c -c -o med2bib.o

findrepl.o : findrepl.c
	$(CC) $(INCLUDEDIR) findrepl.c -c -o findrepl.o

clean:
	rm -f *.o core 

realclean:
	rm -f *.o core med2bib

