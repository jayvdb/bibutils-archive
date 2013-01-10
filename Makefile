POSTFIX="_amd64"
CC = CC="cc -Wall"
RANLIB=RANLIB="ranlib"
INSTALLDIR=/usr/local/bin
LIBINSTALLDIR=/usr/local/lib

VERSION=4.16
DATE=01/09/13

PROGRAMS=bib2xml ris2xml end2xml endx2xml med2xml isi2xml copac2xml \
	biblatex2xml ebi2xml wordbib2xml \
	xml2ads xml2bib xml2end xml2isi xml2ris xml2wordbib modsclean

all : FORCE
	cd lib; $(MAKE) -k $(CC) -k $(RANLIB); cd ..
	cd bin; $(MAKE) -k $(CC) -k VERSION="$(VERSION)" -k DATE="$(DATE)"; cd ..

clean: FORCE
	cd lib     ; $(MAKE) clean ; cd ..
	cd bin     ; $(MAKE) clean ; cd ..
	cd test    ; $(MAKE) clean ; cd ..

realclean: FORCE
	cd lib     ; $(MAKE) realclean ; cd ..
	cd bin     ; $(MAKE) realclean ; cd ..
	cd test    ; $(MAKE) realclean ; cd ..
	rm -rf update lib/bibutils.pc

test: all FORCE
	cd lib    ; $(MAKE) test; cd ..
	cd bin    ; $(MAKE) test; cd ..

install: all FORCE
	cd lib ; $(MAKE) -k LIBINSTALLDIR=$(LIBINSTALLDIR) install; cd ..
	sed 's/VERSION/${VERSION}/g' packageconfig_start > lib/bibutils.pc
	@for p in ${PROGRAMS}; \
		do ( cp bin/$$p ${INSTALLDIR}/$$p ); \
	done

package: all FORCE
	sh -f maketgz.sh ${VERSION} ${POSTFIX}

deb: all FORCE
	sh -f makedeb.sh ${VERSION} ${POSTFIX}

FORCE:
