#
# isi2xml MAKEFILE
#

INCLUDEDIR = -I ../include -I ../lib
LIBDIR     = -L../lib
LIBS       = -lnewstr

# LINUX 
# CC = gcc
# OBJS = newstr.o

isi2xml : isi2xml.o 
	$(CC) isi2xml.o $(LIBDIR) $(OBJS) -o $@ $(LIBS)

isi2xml.o : isi2xml.c
	$(CC) $(INCLUDEDIR) isi2xml.c -c -o isi2xml.o

test: isi2xml FORCE
	csh -f test.com

clean:  FORCE
	rm -f *.o core

realclean: FORCE
	rm -f *.o core isi2xml

test: FORCE isi2xml
#	(cd test; \
#	../isi2xml test.txt > test.xml ; \
#	../isi2xml spaces.txt > spaces.xml ; \
#	../isi2xml cntrlm.txt > cntrlm.xml ; \
#	diff test.xml test_ref.xml | wc ; \
#	diff spaces.xml spaces_ref.xml | wc; \
#	diff cntrlm.xml cntrlm_ref.xml | wc; \
#	cd .. )

FORCE:
