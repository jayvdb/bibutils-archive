#
# ris2xml MAKEFILE
#

# SGI CC
CC         = cc
INCLUDEDIR = -I ../include
LIBDIR     = -L../lib
LIBS       = -lnewstr

# LINUX 
# CC = gcc
# OBJS = newstr.o

ris2xml : ris2xml.o 
	$(CC) ris2xml.o $(LIBDIR) $(OBJS) -o $@ $(LIBS)

ris2xml.o : ris2xml.c
	$(CC) $(INCLUDEDIR) ris2xml.c -c -o ris2xml.o

clean:  FORCE
	rm -f *.o core

realclean: FORCE
	rm -f *.o core ris2xml

test: FORCE ris2xml
#	(cd test; \
#	../ris2xml test.txt > test.xml ; \
#	../ris2xml spaces.txt > spaces.xml ; \
#	../ris2xml cntrlm.txt > cntrlm.xml ; \
#	diff test.xml test_ref.xml | wc ; \
#	diff spaces.xml spaces_ref.xml | wc; \
#	diff cntrlm.xml cntrlm_ref.xml | wc; \
#	cd .. )

FORCE:
