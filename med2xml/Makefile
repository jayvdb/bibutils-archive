#
# med2xml MAKEFILE
#

INCLUDEDIR = -I ../include -I ../lib
LIBDIR     = -L../lib
LIBS       = -lnewstr

# LINUX 
# CC = gcc
# OBJS = newstr.o

med2xml : med2xml.o 
	$(CC) med2xml.o $(LIBDIR) $(OBJS) -o $@ $(LIBS)

med2xml.o : med2xml.c
	$(CC) $(INCLUDEDIR) med2xml.c -c -o med2xml.o

clean:  FORCE
	rm -f *.o core

realclean: FORCE
	rm -f *.o core med2xml

test: FORCE med2xml
	(cd test; \
	../med2xml test.txt > test.xml ; \
	../med2xml spaces.txt > spaces.xml ; \
	../med2xml cntrlm.txt > cntrlm.xml ; \
	diff test.xml test_ref.xml | wc ; \
	diff spaces.xml spaces_ref.xml | wc; \
	diff cntrlm.xml cntrlm_ref.xml | wc; \
	cd .. )

FORCE:
