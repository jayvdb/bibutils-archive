#
# bibutils converters MAKEFILE
#

CFLAGS     = -I ../lib
LIBDIR     = -L../lib
PROGS      = bib2xml biblatex2xml copac2xml end2xml endx2xml isi2xml med2xml \
             ris2xml ebi2xml wordbib2xml \
             xml2ads xml2bib xml2end xml2isi xml2ris xml2wordbib modsclean

all: $(PROGS)

args.o : args.c
	$(CC) $^ -DCURR_VERSION="\"$(VERSION)\"" -DCURR_DATE="\"$(DATE)\"" $(CFLAGS) -c -o $@

bib2xml : bib2xml.o bibprog.o tomods.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

biblatex2xml : biblatex2xml.o bibprog.o tomods.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

copac2xml : copac2xml.o bibprog.o tomods.o args.o  ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

ebi2xml : ebi2xml.o bibprog.o tomods.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

end2xml : end2xml.o bibprog.o tomods.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

endx2xml : endx2xml.o bibprog.o tomods.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

isi2xml : isi2xml.o bibprog.o tomods.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

med2xml : med2xml.o bibprog.o tomods.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

wordbib2xml : wordbib2xml.o bibprog.o tomods.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

ris2xml : ris2xml.o bibprog.o tomods.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

xml2ads : xml2ads.o bibprog.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

xml2bib : xml2bib.o bibprog.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

xml2end : xml2end.o bibprog.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

xml2isi : xml2isi.o bibprog.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

xml2ris : xml2ris.o bibprog.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

xml2wordbib : xml2wordbib.o bibprog.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

modsclean : modsclean.o bibprog.o tomods.o args.o ../lib/libbibutils.a
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

test:

clean:
	rm -f *.o core 

realclean:
	rm -f *.o core $(PROGS)
