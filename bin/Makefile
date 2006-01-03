#
# bibutils converters MAKEFILE
#

CFLAGS     = -I ../lib
LIBDIR     = -L../lib
LIBS       = -lbibprogs -lbibutils 
PROGS      = bib2xml copac2xml end2xml isi2xml med2xml ris2xml \
             xml2bib xml2end xml2ris modsclean

all: $(PROGS)

bib2xml : bib2xml.o ../lib/libbibprogs.a ../lib/libbibutils.a
	$(CC) bib2xml.o $(LIBDIR) $(LIBS) -o $@

copac2xml : copac2xml.o ../lib/libbibprogs.a ../lib/libbibutils.a
	$(CC) copac2xml.o $(LIBDIR) $(LIBS) -o $@

end2xml : end2xml.o ../lib/libbibprogs.a ../lib/libbibutils.a
	$(CC) end2xml.o $(LIBDIR) $(LIBS) -o $@

isi2xml : isi2xml.o ../lib/libbibprogs.a ../lib/libbibutils.a
	$(CC) isi2xml.o $(LIBDIR) $(LIBS) -o $@

med2xml : med2xml.o ../lib/libbibprogs.a ../lib/libbibutils.a
	$(CC) med2xml.o $(LIBDIR) $(LIBS) -o $@

ris2xml : ris2xml.o ../lib/libbibprogs.a ../lib/libbibutils.a
	$(CC) ris2xml.o $(LIBDIR) $(LIBS) -o $@

xml2bib : xml2bib.o ../lib/libbibprogs.a ../lib/libbibutils.a
	$(CC) xml2bib.o $(LIBDIR) $(LIBS) -o $@

xml2end : xml2end.o ../lib/libbibprogs.a ../lib/libbibutils.a
	$(CC) xml2end.o $(LIBDIR) $(LIBS) -o $@

xml2ris : xml2ris.o ../lib/libbibprogs.a ../lib/libbibutils.a
	$(CC) xml2ris.o $(LIBDIR) $(LIBS) -o $@

modsclean : modsclean.o ../lib/libbibprogs.a ../lib/libbibutils.a
	$(CC) modsclean.o $(LIBDIR) $(LIBS) -o $@
test:

clean:
	rm -f *.o core 

realclean:
	rm -f *.o core $(PROGS)
