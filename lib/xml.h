/*---------------------------------------------------------------------------

xml.h 

---------------------------------------------------------------------------*/
#ifndef XML_H
#define XML_H
#include "newstr.h"

char *find_xmlstartdata(char *buffer, char *tag);
char *find_xmlenddata(char *buffer, char *tag);
char *extract_xmldata(char *buffer, char *tag, newstring **s_ptr);

#endif

