## IRIX
#MTYPE=IRIX
#POSTFIX=_irix
#CC = CC="cc"

## SunOS5
#MTYPE=SunOS
#POSTFIX=_sunos5
#CC =  CC="/tsri/SunOS5/SunWorkShop/SUNWspro/bin/cc "

# Linux web
#MTYPE=Linux
#POSTFIX=_elf_rh7.1
#CC = CC="cc -Wall"

#INSTALLDIR=/home/cdputnam/public_html/software

# Linux mine
MTYPE=Linux
POSTFIX=
CC = CC="cc -Wall"

INSTALLDIR=/home/cdputnam/bin


all : libdir med2bibdir bib2endir bib2risdir uniqbibdir ris2xmldir \
	med2xmldir bib2xmldir xml2bibdir xml2endir xml2risdir

libdir:
	cd lib ; make -k $(CC) ; cd ..

med2xmldir:
	cd med2xml ; make -k $(CC) ; cd ..

bib2xmldir:
	cd bib2xml ; make -k $(CC) ; cd ..

ris2xmldir:
	cd ris2xml ; make -k $(CC) ; cd ..

xml2bibdir:
	cd xml2bib ; make -k $(CC) ; cd ..

xml2endir:
	cd xml2en ; make -k $(CC) ; cd ..

xml2risdir:
	cd xml2ris ; make -k $(CC) ; cd ..

med2bibdir:
	cd med2bib ; make -k $(CC) ; cd ..

bib2endir:
	cd bib2en ; make -k $(CC); cd ..

bib2risdir:
	cd bib2ris ; make -k $(CC); cd ..

uniqbibdir:
	cd uniqbib ; make -k $(CC); cd ..

clean: FORCE
	cd lib     ; make clean ; cd ..
	cd med2xml ; make clean ; cd ..
	cd med2bib ; make clean ; cd ..
	cd bib2xml ; make clean ; cd ..
	cd ris2xml ; make clean ; cd ..
	cd xml2bib ; make clean ; cd ..
	cd xml2en  ; make clean ; cd ..
	cd xml2ris ; make clean ; cd ..
	cd bib2en  ; make clean ; cd ..
	cd bib2ris ; make clean ; cd ..
	cd uniqbib ; make clean ; cd ..

realclean: FORCE
	cd lib     ; make realclean ; cd ..
	cd med2xml ; make realclean ; cd ..
	cd med2bib ; make realclean ; cd ..
	cd bib2xml ; make realclean ; cd ..
	cd ris2xml ; make realclean ; cd ..
	cd xml2bib ; make realclean ; cd ..
	cd xml2en  ; make realclean ; cd ..
	cd xml2ris ; make realclean ; cd ..
	cd bib2en  ; make realclean ; cd ..
	cd bib2ris ; make realclean ; cd ..

test: FORCE
	cd lib    ; make test; cd ..
	cd med2xml; make test; cd ..
	cd bib2xml; make test; cd ..
	cd xml2bib; make test; cd ..
	cd xml2en ; make test; cd ..
	cd xml2ris; make test; cd ..

install: FORCE
	# new ones
	cp med2xml/med2xml $(INSTALLDIR)/med2xml$(POSTFIX)
	cp bib2xml/bib2xml $(INSTALLDIR)/bib2xml$(POSTFIX)
	cp ris2xml/ris2xml $(INSTALLDIR)/ris2xml$(POSTFIX)
	cp xml2ris/xml2ris $(INSTALLDIR)/xml2ris$(POSTFIX)
	cp xml2bib/xml2bib $(INSTALLDIR)/xml2bib$(POSTFIX)
	cp xml2en/xml2en   $(INSTALLDIR)/xml2en$(POSTFIX)
	# old ones
	cp med2bib/med2bib $(INSTALLDIR)/med2bib$(POSTFIX)
	cp bib2ris/bib2ris $(INSTALLDIR)/bib2ris$(POSTFIX)
	cp bib2en/bib2en   $(INSTALLDIR)/bib2en$(POSTFIX)
	cp uniqbib/uniqbib $(INSTALLDIR)/uniqbib$(POSTFIX)

FORCE:
