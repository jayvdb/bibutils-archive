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

char *
xml_findstartdata(char *buffer, char *tag)
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

char *
xml_findenddata(char *buffer, char *tag)
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

#undef XML_BUFSIZE
	
/* xml_extractdata()
 *
 * Put everything between an XML start and end tag into a
 * newstring.  If no end, tag, copy everything to the string
 * end.
 *
 * Return pointer that points _after_ the end tag.
 */
char *
xml_extractdata(char *buffer, char *tag, newstring *s)
{
	unsigned int i;
	char         *start,*end,*p;

	if ( buffer==NULL || tag==NULL || s==NULL ) return NULL;

	newstr_strcpy(s,"");

	start = xml_findstartdata(buffer,tag);
	if ( start==NULL ) return buffer+strlen(buffer); /* point to \0 */

	end   = xml_findenddata(start,tag);
	if (end==NULL) {
		newstr_strcpy(s,start);
		p = buffer + strlen(buffer);  /* point to \0 */
	} else newstr_segcpy(s,start,end);

	p = end;
	i = 0;
	while ( (*p) && (i<strlen(tag)+3) ) { p++; i++; }

	return p;
}


/*
 *   xml_readrefs()
 *
 *         Reads references one at a time into buffer and sends each to
 *         process_article( FILE *outptr, newstring *ref, long numref)
 */
long 
xml_readrefs(FILE *inptr, FILE *outptr)
{
	extern void process_article( FILE *, newstring *, long );
	newstring buffer, ref;
	char *p;
	char line[256],*errorptr,*startptr,*endptr;
	int haveref = FALSE, processref = FALSE;
	long numrefs =0L;

	newstr_init(&buffer);
	newstr_init(&ref);

	while (!feof(inptr)) {

		if (processref) {
			process_article(outptr,&ref,numrefs+1);
			newstr_empty( &buffer );
			processref = FALSE;
			numrefs++;
			if (endptr!=NULL) newstr_strcpy(&buffer,endptr+6);
		}

		errorptr = fgets (line, sizeof(line), inptr);
		if (errorptr == NULL) continue;

		startptr = strsearch(line,"<REF>");
		if (startptr != NULL || haveref ) {
			if ( haveref ) newstr_strcat(&buffer,line);
			else {
				newstr_strcat(&buffer,startptr);
				haveref = TRUE;
			}
			endptr = xml_findenddata(buffer.data,"REF"); 
			if (endptr!=NULL) {
				processref=TRUE;
				p = xml_extractdata(buffer.data,"REF",&ref);
				newstr_empty( &buffer );
				if ( p!=NULL ) newstr_strcpy( &buffer, p );
				startptr = strsearch(buffer.data,"<REF>"); 
				if (startptr!=NULL) haveref=TRUE;
				else haveref=FALSE;
			}
		}
  	}
	newstr_free (&buffer);
	newstr_free (&ref);
	return numrefs;
}



