#
# xmlreplace MAKEFILE
#

INCLUDEDIR = -I ../include
LIBDIR     = -L../lib
LIBS       = -lnewstr

all : xmlreplace

xmlreplace : tree.o findrepl.o xmlreplace.o
	$(CC) $(LIBDIR) xmlreplace.o findrepl.o tree.o -o $@  $(LIBS)

xmlreplace.o : xmlreplace.c
	$(CC) $(INCLUDEDIR) xmlreplace.c -c -o xmlreplace.o

findrepl.o : findrepl.c
	$(CC) $(INCLUDEDIR) findrepl.c -c -o findrepl.o

tree.o : tree.c
	$(CC) $(INCLUDEDIR) tree.c -c -o tree.o

clean:
	rm -f *.o core

test:

realclean:
	rm -f *.o core xmlreplace
