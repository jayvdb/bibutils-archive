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
	string->data=NULL;
}

void 
newstr_initalloc(newstring *string, int minsize)
{
	int size = newstr_initlen;
	if (string==NULL) return;
	if (minsize > newstr_initlen) size=minsize;
	string->data = (char *) malloc (sizeof( *(string->data) ) * size);
	if (string->data==NULL) {
		fprintf(stderr,"Error.  Cannot allocate memory in newstr_initalloc.\n");
		exit(1);
	}
	string->data[0]='\0';
	string->dim=size;
}

newstring *
newstr_new( void )
{
	newstring *string;
	string = (newstring *) malloc( sizeof( *string ) );
	if (string!=NULL) 
		newstr_initalloc(string,newstr_initlen);
	return string;
}

#ifndef NEWSTR_PARANOIA

void 
newstr_clear(newstring *string)
{
	if (string==NULL) return;
	string->dim=0;
	if (string->data!=NULL) free(string->data);
	string->data=NULL;
}

void 
newstr_realloc(newstring *string, int minsize)
{
	char *newptr;
	int size;
	size = 2 * string->dim;
	if (size < minsize) size = minsize;
	newptr = (char *) realloc (string->data, sizeof( *(string->data) )*size);
	if (newptr==NULL) {
		fprintf(stderr,"Error.  Cannot reallocate memory (%d bytes) in newstr_realloc.\n", sizeof(*(string->data))*size);
		exit(1);
	}
	string->data = newptr;
	string->dim = size;
}

#else

void 
newstr_clear(newstring *string)
{
	unsigned long i;
	if (string==NULL) return;
	for (i=0; i<string->dim; ++i) string->data[i]='\0';
	string->dim=0;
	if (string->data!=NULL) free(string->data);
	string->data=NULL;
}

void 
newstr_realloc(newstring *string, int minsize)
{
	char *newptr;
	int size;
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
newstr_addchar(newstring *string,char newchar)
{
	int length;
	if ( !string ) return;
	if ( !string->data || string->dim==0 ) 
		newstr_initalloc(string,newstr_initlen);
	length = strlen( string->data );
	if ( length + 2 > string->dim ) 
		newstr_realloc(string,length+2);
	string->data[length+1]='\0';
	string->data[length]=newchar;
}

void 
newstr_fprintf( FILE *fp, newstring *string )
{
    if (string && string->data) fprintf(fp,"%s",string->data);
}

void 
newstr_strcat (newstring *string, char *addstr)
{
	int lenstr = 0, lenaddstr;
	if ( !string || !addstr ) return;
	lenaddstr = strlen( addstr );
	if (string->data==NULL || string->dim==0) 
		newstr_initalloc(string, lenaddstr+1 );
	else {
		lenstr = strlen( string->data );
		if ( lenstr + lenaddstr + 1 > string->dim) 
		   newstr_realloc(string, lenstr + lenaddstr + 1 );
	}
	strncat(string->data,addstr,lenaddstr+1);
}

void 
newstr_strcpy (newstring *string, char *addstr)
{
	int lenaddstr;
	if ( !string || !addstr ) return;
	lenaddstr = strlen( addstr );
	if (string->data==NULL || string->dim==0) 
		newstr_initalloc( string, lenaddstr+1 );
	else if ( lenaddstr+1 > string->dim) 
		newstr_realloc(string,lenaddstr+1);
	strncpy(string->data,addstr,string->dim);
	string->data[string->dim-1]='\0';
}

/*
 * newstr_findreplace()
 *
 *   if replace is "" or NULL, then delete find
 */

void 
newstr_findreplace (newstring *string, char *find, char *replace)
{
	int diff;
	unsigned int findstart, searchstart;
	unsigned int p1, p2;
	unsigned int find_len, rep_len, curr_len;
	char empty[1] = "";
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
		findstart=(unsigned int) p - (unsigned int) string->data;
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
	}
}

