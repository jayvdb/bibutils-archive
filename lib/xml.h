/*---------------------------------------------------------------------------

xml.h 

---------------------------------------------------------------------------*/
#ifndef XML_H
#define XML_H
#include "newstr.h"

#define TRUE (1)
#define FALSE (0)

extern char *xml_findstartdata(char *buffer, char *tag);
extern char *xml_findenddata(char *buffer, char *tag);
extern char *xml_extractdata(char *buffer, char *tag, newstring *s);
extern long  xml_readrefs(FILE *inptr, FILE *outptr);

#endif

