#
# uniqbib MAKEFILE
#

# SGI CC
#CC         = cc
INCLUDEDIR = -I ../include
LIBDIR     = -L../lib
LIBS       = -lnewstr
OBJS       = refs.o 

# LINUX (not tested, but likely use gcc)
# CC = gcc
# OBJS = refs.o newstr.o

uniqbib : uniqbib.o $(OBJS)
	$(CC) $(LIBDIR) uniqbib.o $(OBJS) -o $@ $(LIBS)

uniqbib.o : uniqbib.c 
	$(CC) $(INCLUDEDIR) uniqbib.c -c -o uniqbib.o

refs.o : refs.c
	$(CC) $(INCLUDEDIR) refs.c -c -o refs.o

clean :
	rm -f core *.o

realclean:
	rm -f core *.o uniqbib

