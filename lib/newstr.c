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
newstr_init( newstring *string )
{
	if ( !string ) return;
	string->dim=0;
	string->len=0;
	string->data=NULL;
}

void 
newstr_initalloc(newstring *string, unsigned long minsize)
{
	unsigned long size = newstr_initlen;
	if ( !string ) return;
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
	if ( !string ) return;
	if ( !addstr || addstr[0]=='\0' ) {
		if ( string->data ) string->data[0]='\0';
		string->len = 0;
		return;
	}
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


/* newstr_fget()
 *   returns 0 if we're done, 1 if we're not done
 *   extracts line by line (regardless of end characters)
 *   and feeds from buf....
 */
int
newstr_fget( FILE *fp, char *buf, int bufsize, int *pbufpos, newstring *outs )
{
	int  bufpos = *pbufpos, done = 0;
	char *ok;
	newstr_empty( outs );
	while ( !done ) {
		while ( buf[bufpos] && buf[bufpos]!='\r' && buf[bufpos]!='\n' )
			newstr_addchar( outs, buf[bufpos++] );
		if ( buf[bufpos]=='\0' ) {
			ok = fgets( buf, bufsize, fp );
			bufpos=*pbufpos=0;
			if ( !ok && feof(fp) ) return 0; /* end-of-file */
		} else if ( buf[bufpos]=='\r' || buf[bufpos]=='\n' ) done=1;
	}
	if ( ( buf[bufpos]=='\n' && buf[bufpos+1]=='\r') ||
	     ( buf[bufpos]=='\r' && buf[bufpos+1]=='\n') ) bufpos+=2;
	else if ( buf[bufpos]=='\n' || buf[bufpos]=='\r' ) bufpos+=1; 
	*pbufpos = bufpos;
	return 1;
}


/* newstr_swapstrings( s1, s2 )
 * be sneaky and swap internal newstring data from one
 * string to another
 */
void
newstr_swapstrings( newstring *s1, newstring *s2 )
{
	char *tmpp;
	int tmp;

	/* swap dimensioning info */
	tmp = s1->dim;
	s1->dim = s2->dim;
	s2->dim = tmp;

	/* swap length info */
	tmp = s1->len;
	s1->len = s2->len;
	s2->len = tmp;

	/* swap data */
	tmpp = s1->data;
	s1->data = s2->data;
	s2->data = tmpp;
}

#include "htmlentities.c"

void
newstr_encodexml( newstring *s )
{
	newstring ns;
	int i,j;
	newstr_init( &ns );
	for ( i=0; i<s->len; ++i ) {
		/* Basic Latin conversions */
		if ( s->data[i]==34 ) newstr_strcat( &ns, "&quot;" );
		else if ( s->data[i]=='\'' ) newstr_strcat( &ns, "&apos;" );
		else if ( s->data[i]==60 ) newstr_strcat( &ns, "&lt;" ); 
		else if ( s->data[i]==62 ) newstr_strcat( &ns, "&gt;" );
		else if ( s->data[i]==38 ) {  /* ampersand */
			int found = -1;
			if ( s->data[i+1]=='#' ) newstr_addchar( &ns, '&' );
			else {
				/* check for HTML entity */
				for ( j=0; j<nhtml_entities && found==-1; ++j ) {
					if ( !strncasecmp( &(s->data[i]), 
						  &(html_entities[j].html[0]),
						  strlen(&(html_entities[j].html[0])))) 
						found = j;
				}
				if ( found==-1 )  /* isolated '&' */
					newstr_strcat( &ns, "&amp;" );
				else { /* replace HTML entity */
					newstr_strcat( &ns,
				       	   &(html_entities[found].decimal[0]) );
					i += strlen( &(html_entities[found].decimal[0]) )-1;
				}
			}
		} else if ( ((unsigned char)(s->data[i]))>127 ) { 
			char buf[10];
			unsigned char c = (unsigned char)(s->data[i]);
			sprintf( buf, "&#%u;",c);
			newstr_strcat( &ns, buf );
		}
		else newstr_addchar( &ns, s->data[i] );
	}
	newstr_swapstrings( s, &ns );
	newstr_free( &ns );
}

void
newstr_decodexml( newstring *s )
{
	newstring ns, code;
	int i;
	char ch;
	newstr_init( &ns );
	newstr_init( &code );
	i = 0;
	while ( i<s->len ) {
		ch = s->data[i];
		if ( ch!='&' ) {
			newstr_addchar( &ns, ch );
			i++;
		} else {
			i++;
			while ( i<s->len && s->data[i]!=';' ) {
				newstr_addchar( &code, s->data[i] );
				i++;
			}
			if ( s->data[i]==';' ) i++;
			if ( code.data[0]=='#' ) {
				unsigned int c;
				if ( code.data[1]!='x' ) 
					c = atoi( &(code.data[1]) );
				else
					sscanf( &(code.data[2]), "%x", &c );
				if ( c<256 ) 
					newstr_addchar( &ns, (char) c );
				else         
					newstr_addchar( &ns, '?' );

			} else {
				if ( !strncmp( code.data, "quot", 4 ) ) {
					newstr_addchar( &ns, '\"' );
				} else if ( !strncmp( code.data, "apos", 4 ) ){
					newstr_addchar( &ns, '\'' );
				} else if ( !strncmp( code.data, "amp", 3 ) ){
					newstr_addchar( &ns, '&' );
				} else if ( !strncmp( code.data, "lt", 2 ) ) {
					newstr_addchar( &ns, '<' );
				} else if ( !strncmp( code.data, "gt", 2 ) ) {
					newstr_addchar( &ns, '>' );
				}
			}
			newstr_empty( &code );
		}
	}
	newstr_swapstrings( s, &ns );
	newstr_free( &code );
	newstr_free( &ns );
}
