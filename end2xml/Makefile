#
# end2xml MAKEFILE
#

CFLAGS     = -I ../include -I ../lib
LIBS       = -L../lib -lnewstr 

end2xml : end2xml.o myxmlout.o
	$(CC) $(CFLAGS) $(LIBDIR) end2xml.o myxmlout.o -o $@ $(LIBS)

test:

clean:
	rm -f *.o core 

realclean:
	rm -f *.o core end2xml
