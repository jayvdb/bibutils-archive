
## IRIX
#MTYPE=IRIX
#POSTFIX=_irix
#CC = CC="cc"
#RANLIB= RANLIB="echo Skipping ranlib"

## SunOS5
#MTYPE=SunOS
#POSTFIX=_sunos5
#CC =  CC="/tsri/SunOS5/SunWorkShop/SUNWspro/bin/cc "
#RANLIB= RANLIB="echo Skipping ranlib"

# Linux web
#MTYPE=Linux
#POSTFIX=_i386
#CC = CC="cc -Wall"
#RANLIB= RANLIB="ranlib"

# MacOSX web
#MTYPE=MacOSX
#POSTFIX=_osx
#CC = CC="cc -Wall"
#RANLIB= RANLIB="ranlib"

# Linux mine
MTYPE=Linux
POSTFIX=_i386
CC= CC="cc -Wall"
RANLIB= RANLIB="ranlib"

#INSTALLDIR = /home/cdputnam/bin/
INSTALLDIR = bibutils_2.18

VERSION=2.18

DIRS =libdir uniqbibdir ris2xmldir med2xmldir bib2xmldir end2xmldir \
	isi2xmldir xml2bibdir xml2endir xml2risdir xmlreplacedir


all : $(DIRS)

libdir:
	cd lib ; make -k $(CC) -k $(RANLIB) -k VERSION="$(VERSION)"; cd ..

med2xmldir:
	cd med2xml ; make -k $(CC) ; cd ..

bib2xmldir:
	cd bib2xml ; make -k $(CC) ; cd ..

ris2xmldir:
	cd ris2xml ; make -k $(CC) ; cd ..

end2xmldir:
	cd end2xml ; make -k $(CC) ; cd ..

isi2xmldir:
	cd isi2xml ; make -k $(CC) ; cd ..

xml2bibdir:
	cd xml2bib ; make -k $(CC) ; cd ..

xml2endir:
	cd xml2en ; make -k $(CC) ; cd ..

xml2risdir:
	cd xml2ris ; make -k $(CC) ; cd ..

xmlreplacedir:
	cd xmlreplace ; make -k $(CC) ; cd ..

uniqbibdir:
	cd uniqbib ; make -k $(CC); cd ..

clean: FORCE
	cd lib     ; make clean ; cd ..
	cd med2xml ; make clean ; cd ..
	cd bib2xml ; make clean ; cd ..
	cd end2xml ; make clean ; cd ..
	cd isi2xml ; make clean ; cd ..
	cd ris2xml ; make clean ; cd ..
	cd xml2bib ; make clean ; cd ..
	cd xml2en  ; make clean ; cd ..
	cd xml2ris ; make clean ; cd ..
	cd xmlreplace ; make clean; cd ..
	cd uniqbib ; make clean ; cd ..

realclean: FORCE
	cd lib     ; make realclean ; cd ..
	cd med2xml ; make realclean ; cd ..
	cd bib2xml ; make realclean ; cd ..
	cd end2xml ; make realclean ; cd ..
	cd isi2xml ; make realclean ; cd ..
	cd ris2xml ; make realclean ; cd ..
	cd xml2bib ; make realclean ; cd ..
	cd xml2en  ; make realclean ; cd ..
	cd xml2ris ; make realclean ; cd ..
	cd xmlreplace ; make realclean ; cd ..
	cd uniqbib ; make realclean ; cd ..
	/bin/rm -rf update

test: FORCE
	cd lib    ; make test; cd ..
	cd med2xml; make test; cd ..
	cd bib2xml; make test; cd ..
	cd end2xml; make test; cd ..
	cd isi2xml; make test; cd ..
	cd xml2bib; make test; cd ..
	cd xml2en ; make test; cd ..
	cd xml2ris; make test; cd ..

install: $(DIRS) FORCE
	cp med2xml/med2xml $(INSTALLDIR)/med2xml
	cp bib2xml/bib2xml $(INSTALLDIR)/bib2xml
	cp end2xml/end2xml $(INSTALLDIR)/end2xml
	cp isi2xml/isi2xml $(INSTALLDIR)/isi2xml
	cp ris2xml/ris2xml $(INSTALLDIR)/ris2xml
	cp xml2ris/xml2ris $(INSTALLDIR)/xml2ris
	cp xml2bib/xml2bib $(INSTALLDIR)/xml2bib
	cp xml2en/xml2en   $(INSTALLDIR)/xml2en
	cp xmlreplace/xmlreplace $(INSTALLDIR)/xmlreplace
	cp uniqbib/uniqbib $(INSTALLDIR)/uniqbib

package: $(DIRS) FORCE
	mkdir update
	mkdir update/bibutils_$(VERSION)
	cp med2xml/med2xml update/bibutils_$(VERSION)/med2xml
	cp bib2xml/bib2xml update/bibutils_$(VERSION)/bib2xml
	cp end2xml/end2xml update/bibutils_$(VERSION)/end2xml
	cp isi2xml/isi2xml update/bibutils_$(VERSION)/isi2xml
	cp ris2xml/ris2xml update/bibutils_$(VERSION)/ris2xml
	cp xml2ris/xml2ris update/bibutils_$(VERSION)/xml2ris
	cp xml2bib/xml2bib update/bibutils_$(VERSION)/xml2bib
	cp xml2en/xml2en   update/bibutils_$(VERSION)/xml2en
	cp xmlreplace/xmlreplace update/bibutils_$(VERSION)/xmlreplace
	cp uniqbib/uniqbib update/bibutils_$(VERSION)/uniqbib
	cd update; tar cvf - bibutils_$(VERSION) | gzip - > bibutils_$(VERSION)$(POSTFIX).tgz; cd ..
	rm -r update/bibutils_$(VERSION)

FORCE:
