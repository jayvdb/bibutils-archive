#
# bibutils unit tests MAKEFILE
#

CFLAGS     = -Wall -I ../lib
LIBDIR     = -L../lib
LIBS       = -lbibutils 
PROGS      = newstr_test entities_test utf8_test

all: $(PROGS)

entities_test : entities_test.o ../lib/libbibcore.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBS) $(LDLIBS) -o $@

utf8_test : utf8_test.o ../lib/libbibcore.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBS) $(LDLIBS) -o $@

newstr_test : newstr_test.o ../lib/libbibcore.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBS) $(LDLIBS) -o $@

test: entities_test newstr_test FORCE
	./newstr_test
	./entities_test
	./utf8_test

clean:
	rm -f *.o core 

realclean:
	rm -f *.o core $(PROGS)

FORCE:
