/*
 * strtest.c
 *
 * test newstr functions
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef OLD
#include "newstr.h"
#else
#include "../../bibutils_2.3/include/newstr.h"
#endif

int
test_consistency( newstring *s, int numchars, char *fn )
{
#ifndef OLD
	if ( strlen( s->data ) != s->len ) {
		fprintf(stdout,"%s: failed consistancy check found %d, s->len=%ld\n",fn,strlen(s->data),s->len);
		return 0;
	}
	if ( strlen( s->data ) != numchars ) {
		fprintf(stdout,"%s: failed consistancy check found %d, expected %d\n",fn,strlen(s->data),numchars);
		return 0;
	}
#endif
	return 1;
}

void
test_addchar( newstring *s )
{
	int numchars = 1000, i;
	s->data[0] = '\0';
#ifndef OLD
	s->len=0;
#endif
	for (i=0; i<numchars; ++i) {
		newstr_addchar( s, ( i % 64 ) + 64);
	}
	test_consistency( s, numchars, "test_addchar" );
}

void
test_strcat( newstring *s )
{
	int numstrings = 1000, i;
	s->data[0] = '\0';
#ifndef OLD
	s->len=0;
#endif
	for ( i=0; i<numstrings; ++i ) {
		newstr_strcat( s, "" );
	}
	test_consistency( s, 0, "test_strcat" );
	s->data[0] = '\0';
#ifndef OLD
	s->len=0;
#endif
	for ( i=0; i<numstrings; ++i ) {
		newstr_strcat( s, "1" );
	}
	test_consistency( s, numstrings, "test_strcat" );
	s->data[0] = '\0';
#ifndef OLD
	s->len = 0;
#endif
	for ( i=0; i<numstrings; ++i ) {
		newstr_strcat( s, "XXOO" );
	}
	test_consistency( s, numstrings*4, "test_strcat" );
}
	
void
test_strcpy( newstring *s )
{
	int numstrings = 1000, i;
	s->data[0] = '\0';
#ifndef OLD
	s->len=0;
#endif
	for ( i=0; i<numstrings; ++i ) {
		newstr_strcpy( s, "" );
	}
	test_consistency( s, 0, "test_strcpy" );
	s->data[0] = '\0';
#ifndef OLD
	s->len=0;
#endif
	for ( i=0; i<numstrings; ++i ) {
		newstr_strcpy( s, "1" );
	}
	test_consistency( s, 1, "test_strcpy" );
	s->data[0] = '\0';
#ifndef OLD
	s->len = 0;
#endif
	for ( i=0; i<numstrings; ++i ) {
		newstr_strcpy( s, "XXOO" );
	}
	test_consistency( s, 4, "test_strcpy" );
}

void
test_segcpy( newstring *s )
{
	int numstrings = 1000, i;
	char segment[]="0123456789";
	char *start=&(segment[2]), *end=&(segment[5]);
	s->data[0] = '\0';
#ifndef OLD
	s->len=0;
#endif
	for ( i=0; i<numstrings; ++i ) {
		newstr_segcpy( s, start, end );
	}
	test_consistency( s, 3, "test_segcpy" );
}

void
test_segcat( newstring *s )
{
	int numstrings = 1000, i;
	char segment[]="0123456789";
	char *start=&(segment[2]), *end=&(segment[5]);
	s->data[0] = '\0';
#ifndef OLD
	s->len=0;
#endif
	for ( i=0; i<numstrings; ++i ) {
		newstr_segcat( s, start, end );
	}
	test_consistency( s, 3*numstrings, "test_segcat" );
}

void
test_findreplace( newstring *s )
{
	int numstrings = 1000, i;
	char segment[]="0123456789";
	for ( i=0; i<numstrings; ++i ) {
		newstr_strcpy( s, segment );
		newstr_findreplace( s, "234", "" );
	}
	test_consistency( s, 7, "test_findreplace" );
	for ( i=0; i<numstrings; ++i ) {
		newstr_strcpy( s, segment );
		newstr_findreplace( s, "234", "223344" );
	}
	test_consistency( s, 13, "test_findreplace" );
}

int
main ( int argc, char *argv[] )
{
	int ntest = 1000;
	int i;
	newstring s;
	newstr_init( &s );
	newstr_strcpy( &s, "" );   /* allocate memory and clear */
	for ( i=0; i<ntest; ++i)
		test_addchar( &s );
	for ( i=0; i<ntest; ++i)
		test_strcat( &s );
	for ( i=0; i<ntest; ++i)
		test_strcpy( &s );
	for ( i=0; i<ntest; ++i)
		test_segcpy( &s );
	for ( i=0; i<ntest; ++i)
		test_segcat( &s );
	for ( i=0; i<ntest; ++i)
		test_findreplace( &s );
	newstr_free( &s );
	return EXIT_SUCCESS;
}
