/*
 * bibstr.c
 *
 * Handle Bibtex string substitutions like
 * @STRING{FCCMW={FCCM}}
 * @STRING{ISFCCM={FCCM}}
 * @STRING{ISFPCCM={FCCM}}
 *
 * Copyright (c) 2004 Chris Putnam
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "newstr.h"
#include "lists.h"
#include "is_ws.h"

extern int verbose; 

lists find    = { 0, 0, NULL };
lists replace = { 0, 0, NULL };

void
bibtex_addstring( char *p )
{
	void bibtex_getstring( newstring *s, char *p );
	char * bibtex_getfind( newstring *s, char *p );
	void bibtex_getreplace( newstring *s, char *p );

	int n = find.nstr;
	newstring s;
	newstr_init( &s );
	lists_add( &find, "" );
	lists_add( &replace, "" );
	bibtex_getstring( &s, p );
	p = bibtex_getfind( &(find.str[n]), s.data );
	bibtex_getreplace( &(replace.str[n]), p );
	if ( verbose ) {
		fprintf( stderr, "String replacement: '%s' = '%s'\n",
				(find.str[n]).data, (replace.str[n]).data );
	}
	newstr_free( &s );
}

void
bibtex_usestrings( newstring *s )
{
	int i, n = find.nstr;
	for ( i=0; i<n; ++i ) {
		if ( strstr( s->data, (find.str[i]).data ) ) {
			newstr_findreplace( s, (find.str[i]).data, 
					(replace.str[i]).data );
		}
	}
}

void
bibtex_strfree()
{
	lists_free( &find );
	lists_free( &replace );
}

void
bibtex_getstring( newstring *s, char *p )
{
	int level=0;
	char end;
	if ( *p=='{' ) end='}';
	if ( *p=='(' ) end=')';
	p++;
	while ( *p && (level!=0 || (level==0 && *p!=end ) ) ) {
		if ( *p=='{' || *p=='(' ) level++;
		else if ( *p=='}' || *p==')' ) level--;
		newstr_addchar( s, *p++ );
	}
}

char *
bibtex_getfind( newstring *s, char *p )
{
	while ( *p && *p!='=')
		newstr_addchar( s, *p++ );
	if ( *p=='=' ) p++;
	while ( is_ws( *p ) ) p++;
	return p;
}

void
bibtex_getreplace( newstring *s, char *p )
{
	while ( *p )
		newstr_addchar( s, *p++ );
}

/*
void
bibtex_strdump( FILE *outptr )
{
	int i;
	fprintf( outptr, "bibtex strings\n" );
	for ( i=0; i<find.nstr; ++i )
		fprintf( outptr, " '%s' = '%s'\n", find.str[i].data, replace.str[i].data );
}
*/

