/*---------------------------------------------------------------------------

xml.c 

---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "newstr.h"
#include "strsearch.h"
#include "xml.h"

#define XML_BUFSIZE (512)

char *find_xmlstartdata(char *buffer, char *tag)
{
	int i=0,length;
	char *p;
	char starttag[XML_BUFSIZE], *startptr;
	length = strlen(tag);
	if (length<XML_BUFSIZE-3) {
		sprintf(starttag,"<%s>",tag);
		p = strsearch(buffer,starttag);
	} else {
		p = NULL;
		startptr = (char *) malloc( sizeof(char)*(length+4));
		if (startptr!=NULL) {
			sprintf(startptr,"<%s>",tag);
			p = strsearch(buffer,startptr);
			free(startptr);
		}
	}
	if (p!=NULL) while (*p && i<length+2){ p++; i++; }
	return p;
}

char *find_xmlenddata(char *buffer, char *tag)
{
	int length;
	char *p;
	char endtag[XML_BUFSIZE], *endptr;
	length = strlen(tag);
	if (length<XML_BUFSIZE-4) {
		sprintf(endtag,"</%s>",tag);
		p = strsearch(buffer,endtag);
	} else {
		p = NULL;
		endptr = (char *) malloc( sizeof(char)*(length+5));
		sprintf(endptr,"</%s>",tag);
		p = strsearch(buffer,endptr);
		free(endptr);
	}
	return p;
}
	

char *extract_xmldata(char *buffer, char *tag, newstring **s_ptr)
{
	newstring *s;
	char *start,*end,*p;
	unsigned int i;

	s = *s_ptr;
	if (s==NULL) {
		s = (newstring *) malloc(sizeof(newstring));
		if (s==NULL) return NULL;
		newstr_init(s);
		*s_ptr = s;
	} else newstr_strcpy(s,"");
	*s_ptr = s;
	if (buffer==NULL) return NULL;
	start = find_xmlstartdata(buffer,tag);
	if (start==NULL) return NULL;
	end   = find_xmlenddata(start,tag);
	if (end==NULL) {
		newstr_strcpy(s,start);
		p = buffer + strlen(buffer);  /* point to \0 */
	}
	p = start;
	while ( p != end ) {
		newstr_addchar(s,*p);
		p++;
	}
	i = 0;
	while ( (*p) && (i<strlen(tag)+3) ) { p++; i++; }
	return p;
}

#undef XML_BUFSIZE
