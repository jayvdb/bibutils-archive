/*
 * name.c
 *
 * mangle names w/ and w/o commas
 *
 * Copyright (c) Chris Putnam 2004-5
 *
 * Source code released under the GPL
 *
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "is_ws.h"
#include "newstr.h"
#include "fields.h"
#include "lists.h"
#include "name.h"

static void
check_case( char *start, char *end, int *upper, int *lower )
{
	int u = 0, l = 0;
	char *p = start;
	while ( p < end ) { 
		if ( islower( *p ) ) l = 1;
		else if ( isupper( *p ) ) u = 1;
		p++;
	}
	*upper = u;
	*lower = l;
}

static int
should_split( int upperlast, int lowerlast, int upperfirst, int lowerfirst )
{
	if ( ( upperlast && lowerlast ) && ( upperfirst && !lowerfirst ) )
		return 1;
	else return 0;
}


/* name_nocomma()
 *
 * names in the format "H. F. Author"
 */
void
name_nocomma( char *start, newstr *outname )
{
	char *p, *last, *end;
	int uplast, lowlast, upfirst, lowfirst, splitfirst;

	/* move to end */
	p = start;
	while ( *p && *(p+1) ) p++;

	/* point to last name */
	end = p;
	while ( p>start && !is_ws( *p ) ) p--;
	if ( !strcasecmp( p, "Jr." ) || !strcasecmp( p, "III" ) ) {
		while ( p>start && is_ws( *p ) ) p--;
		while ( p>start && !is_ws( *p ) ) p--;
	}
	last = p;

	while ( is_ws( *p ) ) p++;

	/* look for upper and lower case in last name */
	check_case( p, end+1, &uplast, &lowlast );
	/* copy last name */
	while ( p<=end ) newstr_addchar( outname, *p++ );

	/* now deal with first name */
	if ( start!=last ) {
		newstr_addchar( outname, '|' );
		/* look for upper and lower case in given name(s) */
		check_case( start, last, &upfirst, &lowfirst );
		splitfirst = should_split( uplast, lowlast, upfirst, lowfirst );
		/* copy given name(s) */
		p = start;
		while ( p!=last ) {
			if ( *p!=' ' && *p!='\t' ) {
				if ( !(splitfirst && ( *p=='.' || *p=='-' ) ) )
					newstr_addchar( outname, *p );
				if ( splitfirst ) newstr_addchar( outname, '|');
				p++;
			} else {
				while ( p!=last && ( *p==' ' || *p=='\t' ) )
					p++;
				if ( p!=last ) newstr_addchar( outname, '|' );
			}
		}
	}
}


/*
 * name_comma()
 *
 * names in the format "Author, H.F.", w/comma
 */
void
name_comma( char *p, newstr *outname )
{
	char *q;
	int uplast, lowlast, upfirst, lowfirst, splitfirst;

	q = p;
	while ( *q && ( *q!=',' ) ) q++;
	check_case( p, q, &uplast, &lowlast );

	while ( *p && ( *p!=',' ) ) 
		newstr_addchar( outname, *p++ );

	if ( *p==',' ) p++;
	while ( *p && is_ws( *p ) ) p++;

	q = p;
	while ( *q ) q++;
	check_case( p, q, &upfirst, &lowfirst );
	splitfirst = should_split( uplast, lowlast, upfirst, lowfirst );

	/* add each part of the given name */
	if ( *p ) newstr_addchar( outname, '|' );
	/* identify cases of Author, HF */
	while ( *p ) {
		if ( !is_ws( *p ) ) {
			if ( ! (splitfirst && ( *p=='.' || *p=='-' ) ) )
				newstr_addchar( outname, *p );
			if ( splitfirst ) newstr_addchar( outname, '|' );
		}
		else if ( *(p+1)!='\0' )
			newstr_addchar( outname, '|' );
		p++;
	}
}

/*
 * name_add( info, newtag, data, level )
 *
 * take name(s) in data, multiple names should be separated by
 * '|' characters and divide into individual name, e.g.
 * "H. F. Author|W. G. Author|Q. X. Author"
 *
 * for each name, compare to names in the "as is" or "corporation"
 * lists...these are not personal names and should be added to the
 * bibliography fields directly and should not be mangled
 * 
 * for each personal name, send to appropriate algorithm depending
 * on if the author name is in the format "H. F. Author" or
 * "Author, H. F."
 */

extern lists asis;
extern lists corps;

void
name_add( fields *info, char *tag, char *q, int level )
{
	newstr inname, outname;
	char *p, *start, *end;

	if ( !q ) return;

	newstr_init( &inname );
	newstr_init( &outname );

	while ( *q ) {
		/* remove leading ws */
		while ( is_ws( *q ) ) q++;
		start = q;
		while ( *q && *q!='|' ) q++;
		end = q;
		while ( is_ws( *end ) || *end=='|' ) end--;
		/* remove trailing ws */
		for ( p=start; p<=end; p++ )
			newstr_addchar( &inname, *p );
		if ( *q=='|' ) q++;

		if ( lists_find( &asis, inname.data ) != -1 ) {
			fields_add_tagsuffix( info, tag, ":ASIS", inname.data, 
				level );
		} else if ( lists_find( &corps, inname.data )!=-1 ) {
			fields_add_tagsuffix( info, tag, ":CORP", inname.data,
				level );
		} else {
			newstr_findreplace( &inname, ".", ". " );
			if ( strchr( inname.data, ',' ) ) 
				name_comma( inname.data, &outname );
			else
				name_nocomma( inname.data, &outname );
			if ( outname.len!=0 ) {
				fields_add( info, tag, outname.data, level );
			}
		}
		newstr_empty( &inname );
		newstr_empty( &outname );
	}
	newstr_free( &inname );
	newstr_free( &outname );
}
