/*
 * newstring routines for dynamically allocated strings
 *
 * C. Putnam 3/29/02  Clean up newstr_findreplace() (x4 speed increase too)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newstr.h"

#define newstr_initlen (64)

void 
newstr_init(newstring *string)
{
	if (string==NULL) return;
	string->dim=0;
	string->len=0;
	string->data=NULL;
}

void 
newstr_initalloc(newstring *string, unsigned long minsize)
{
	unsigned long size = newstr_initlen;
	if (string==NULL) return;
	if (minsize > newstr_initlen) size=minsize;
	string->data = (char *) malloc (sizeof( *(string->data) ) * size);
	if (string->data==NULL) {
		fprintf(stderr,"Error.  Cannot allocate memory in newstr_initalloc.\n");
		exit(1);
	}
	string->data[0]='\0';
	string->dim=size;
	string->len=0;
}

newstring *
newstr_new( void )
{
	newstring *string;
	string = (newstring *) malloc( sizeof( *string ) );
	if (string!=NULL) 
		newstr_initalloc( string, newstr_initlen );
	return string;
}

/* newstr_empty()
 *
 * empty data in string
 */
void
newstr_empty( newstring *string )
{
	if (string && string->data) {
		string->data[0]='\0';
		string->len=0;
	}
}

#ifndef NEWSTR_PARANOIA

void 
newstr_free(newstring *string)
{
	if (string==NULL) return;
	string->dim=0;
	string->len=0;
	if (string->data!=NULL) free(string->data);
	string->data=NULL;
}

void 
newstr_realloc(newstring *string, unsigned long minsize)
{
	char *newptr;
	unsigned long size;
	size = 2 * string->dim;
	if (size < minsize) size = minsize;
	newptr = (char *) realloc (string->data, sizeof( *(string->data) )*size);
	if (newptr==NULL) {
		fprintf(stderr,"Error.  Cannot reallocate memory (%ld bytes) in newstr_realloc.\n", sizeof(*(string->data))*size);
		exit(1);
	}
	string->data = newptr;
	string->dim = size;
}

#else

void 
newstr_free(newstring *string)
{
	unsigned long i;
	if (string==NULL) return;
	for (i=0; i<string->dim; ++i) string->data[i]='\0';
	string->dim=0;
	if (string->data!=NULL) free(string->data);
	string->data=NULL;
}

void 
newstr_realloc(newstring *string, unsigned long minsize)
{
	char *newptr;
	unsigned long size;
	unsigned long i;
	size = 2 * string->dim;
	if (size < minsize) size = minsize;
	newptr = (char *) malloc( sizeof( *(string->data) ) * size );
	if ( newptr == NULL ) {
		fprintf(stderr,"Error.  Cannot reallocate memory (%d bytes) in newstr_realloc.\n", sizeof(*(string->data))*size);
		exit(1);
	}
	for (i=0; i<string->dim; ++i) string->data[i]='\0';
	free( string->data );
	string->data = newptr;
	string->dim = size;
}

#endif

void
newstr_addchar(newstring *string, char newchar)
{
	if ( !string ) return;
	if ( !string->data || string->dim==0 ) 
		newstr_initalloc(string,newstr_initlen);
	if ( string->len + 2 > string->dim ) 
		newstr_realloc(string,string->len+2);
	string->data[string->len++] = newchar;
	string->data[string->len] = '\0';
}

void 
newstr_fprintf( FILE *fp, newstring *string )
{
    if (string && string->data) fprintf(fp,"%s",string->data);
}

void 
newstr_strcat (newstring *string, char *addstr)
{
	unsigned long lenaddstr;
	if ( !string || !addstr ) return;
	lenaddstr = strlen( addstr );
	if (string->data==NULL || string->dim==0) 
		newstr_initalloc(string, lenaddstr+1 );
	else {
		if ( string->len + lenaddstr + 1 > string->dim) 
		   newstr_realloc(string, string->len + lenaddstr + 1 );
	}
	strncpy(&(string->data[string->len]),addstr,lenaddstr);
	string->len += lenaddstr;
	string->data[string->len]='\0';
}

void
newstr_segcat (newstring *string, char *startat, char *endat)
{
	unsigned int seglength;
	char *p, *q;

	if (string==NULL || startat==NULL || endat==NULL ) return;
	if ( (unsigned int) startat > (unsigned int) endat ) return;

	seglength=(unsigned int) endat - (unsigned int) startat;
	if (string->data==NULL || string->dim==0)
		newstr_initalloc(string, seglength+1);
	else {
		if ( string->len + seglength + 1 > string->dim )
			newstr_realloc(string, string->len + seglength+1);
	}
	q = &(string->data[string->len]);
	p = startat;
	while ( *p && p!=endat ) *q++ = *p++;
	*q = '\0';
	string->len += seglength;
}

void 
newstr_strcpy (newstring *string, char *addstr)
{
	unsigned long lenaddstr;
	if ( !string || !addstr ) return;
	lenaddstr = strlen( addstr );
	if (string->data==NULL || string->dim==0)
		newstr_initalloc( string, lenaddstr+1 );
	else if ( lenaddstr+1 > string->dim) 
		newstr_realloc(string,lenaddstr+1);
	strncpy(string->data,addstr,lenaddstr);
	string->data[lenaddstr]='\0';
	string->len=lenaddstr;
}

void
newstr_segcpy (newstring *string, char *startat, char *endat )
{
	unsigned long seglength;
	char *p, *q;

	if (string==NULL || startat==NULL || endat==NULL ) return;
	if ( (unsigned long) startat > (unsigned long) endat ) return;

	seglength=(unsigned long) endat - (unsigned long) startat;
	if (string->data==NULL || string->dim==0)
		newstr_initalloc(string, seglength+1);
	else {
		if ( seglength+1 > string->dim )
			newstr_realloc(string, seglength+1);
	}
	q = string->data;
	p = startat;
	while ( *p && p!=endat ) *q++ = *p++;
	*q = '\0';
	string->len = seglength;
}

/*
 * newstr_findreplace()
 *
 *   if replace is "" or NULL, then delete find
 */

void 
newstr_findreplace (newstring *string, char *find, char *replace)
{
	long diff;
	unsigned long findstart, searchstart;
	unsigned long p1, p2;
	unsigned long find_len, rep_len, curr_len;
	char empty[2] = "";
	unsigned long minsize;
	char *p;

	if (string==NULL || string->data==NULL || string->dim==0 ||
		find==NULL) return;

	if (replace==NULL) replace = empty;

	find_len = strlen( find );
	rep_len  = strlen( replace );
	diff     = rep_len - find_len;
	if ( diff < 0 ) diff = 0;

	searchstart=0;
	while ((p=strstr(string->data + searchstart,find))!=NULL) {
		curr_len = strlen(string->data);
		findstart=(unsigned long) p - (unsigned long) string->data;
		minsize = curr_len + diff + 1;
	 	if (string->dim <= minsize) newstr_realloc( string, minsize );
		if ( find_len > rep_len ) {
			p1 = findstart + rep_len;
			p2 = findstart + find_len;
			while( string->data[p2] )
				string->data[p1++]=string->data[p2++];
			string->data[p1]='\0';
		} else if ( find_len < rep_len ) {
			for (p1=curr_len; p1>=findstart+find_len; p1--)
				string->data[p1+diff] = string->data[p1];
		}
		for (p1=0; p1<rep_len; p1++)
			string->data[findstart+p1]=replace[p1];
		searchstart = findstart + rep_len; 
		string->len += rep_len - find_len;
	}
}

