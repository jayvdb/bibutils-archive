CC = cc -Wall
INCLUDEDIR = -I../include
OBJS = newstr.o xml.o strsearch.o compiledate.o

all: strtest strtest_old

strtest: strtest.c ../lib/libnewstr.a
	$(CC) $(INCLUDEDIR) -c -o strtest.o strtest.c
	$(CC) -o strtest strtest.o -L../lib -lnewstr

strtest_old: strtest.c ../../bibutils_2.3/lib/libnewstr.a
	$(CC) $(INCLUDEDIR) -D OLD -c -o strtest_old.o strtest.c
	$(CC) -o strtest_old strtest_old.o -L../../bibutils_2.3/lib -lnewstr

clean: 
	/bin/rm -f *.o core

realclean:
	/bin/rm -f *.o *.a core strtest strtest_old

test:
