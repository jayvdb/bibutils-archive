#CC = gcc -Wall 
RANLIB = echo

SIMPLE_OBJS   = is_ws.o strsearch.o charsets.o

NEWSTR_OBJS   = newstr.o newstr_conv.o entities.o latex.o utf8.o gb18030.o

CONTAIN_OBJS  = fields.o list.o xml.o xml_encoding.o vplist.o

BIBL_OBJS     = name.o title.o doi.o bibl.o serialno.o marc.o reftypes.o bibl.o

INPUT_OBJS    = bibtexin.o bibtextypes.o \
		biblatexin.o bltypes.o \
		copacin.o copactypes.o \
		endin.o endtypes.o \
		endxmlin.o \
		isiin.o isitypes.o \
		medin.o \
		modsin.o modstypes.o marc.o \
		risin.o ristypes.o \
		ebiin.o wordin.o

OUTPUT_OBJS   = bibtexout.o endout.o isiout.o modsout.o risout.o wordout.o \
		adsout.o

BIBCORE_OBJS = $(SIMPLE_OBJS) $(NEWSTR_OBJS) $(CONTAIN_OBJS) $(BIBL_OBJS) \
               bibcore.o

BIBUTILS_OBJS = $(INPUT_OBJS) $(OUTPUT_OBJS) bibutils.o

all: libbibcore.a libbibutils.a

libbibcore.a: $(BIBCORE_OBJS)
	ar -cr $@ $^
	$(RANLIB) $@

libbibutils.a: $(BIBUTILS_OBJS)
	ar -cr $@ $^
	$(RANLIB) $@

clean: 
	/bin/rm -f *.o core

realclean:
	/bin/rm -f *.o *.a core

test:
