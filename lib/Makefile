#CC = cc -Wall
CFLAGS = -I../include
OBJS = newstr.o xml.o strsearch.o compiledate.o is_ws.o fields.o lists.o

libnewstr.a: $(OBJS)
	ar -cr libnewstr.a $(OBJS)
	$(RANLIB) libnewstr.a

newstr.o : newstr.h newstr.c
	$(CC) $(CFLAGS) -c -o newstr.o newstr.c

xml.o : xml.h xml.c
	$(CC) $(CFLAGS) -c -o xml.o xml.c

strsearch.o : strsearch.h strsearch.c
	$(CC) $(CFLAGS) -c -o strsearch.o strsearch.c

is_ws.o : is_ws.c
	$(CC) $(CFLAGS) -c -o is_ws.o is_ws.c

fields.o: fields.c
	$(CC) $(CFLAGS) -c -o fields.o fields.c

lists.o: lists.c
	$(CC) $(CFLAGS) -c -o lists.o lists.c

compiledate.o : compiledate.c
	$(CC) $(CFLAGS) -DVERSION="\"$(VERSION)\"" -c -o compiledate.o compiledate.c

clean: 
	/bin/rm -f *.o core

realclean:
	/bin/rm -f *.o *.a core

test:
