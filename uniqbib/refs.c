/*
 * refs.c
 *
 * Copyright (c) 1996-8 C. Putnam
 */
#include <stdio.h>
#include <stdlib.h>
#include "refs.h"

REFS *
refs_new( void )
{
	REFS *curr;

	curr = (REFS *) calloc ( 1, sizeof(REFS) );
	if ( !curr ) {
		fprintf(stderr,"Error.  Cannot allocate memory.\n");
		exit(1);
	}
	curr->refname = newstr_new();
	curr->source = newstr_new();
	if ( !curr->refname || !curr->source ) {
		fprintf(stderr,"Error.  Cannot allocate memory.\n");
		exit(1);
	}
	return curr;
}

void 
refs_dispose( REFS *curr )
{
	REFS *next;
	while ( curr ) {
		next = curr->next;
		newstr_free( curr->refname );
		newstr_free( curr->source );
		free( curr );
		curr = next;
	}
}
