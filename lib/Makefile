#CC = cc -Wall
INCLUDEDIR = -I../include
OBJS = newstr.o xml.o strsearch.o

libnewstr.a: $(OBJS)
	ar -cr libnewstr.a $(OBJS)

newstr.o : newstr.h newstr.c
	$(CC) $(INCLUDEDIR) -c -o newstr.o newstr.c

xml.o : xml.h xml.c
	$(CC) $(INCLUDEDIR) -c -o xml.o xml.c

strsearch.o : strsearch.h strsearch.c
	$(CC) $(INCLUDEDIR) -c -o strsearch.o strsearch.c

clean: 
	/bin/rm -f *.o core

realclean:
	/bin/rm -f *.o *.a core

test:
