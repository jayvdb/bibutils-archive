/*
 * fields.c
 *
 * Copyright (c) Chris Putnam 2003-4
 *
 * Source code released under the GPL
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strsearch.h"
#include "fields.h"

int
fields_add( fields *info, char *tag, char *data )
{
	newstring *newtags, *newdata;
	int *newused;
	int min_alloc = 20, i, found;
	if ( info->maxfields==0 ){
		info->tag = (newstring*)malloc( sizeof(newstring) * min_alloc );
		info->data= (newstring*)malloc( sizeof(newstring) * min_alloc );
		info->used= (int*)      malloc( sizeof(int) * min_alloc );
		if ( !info->tag || !info->data || !info->used ) {
			if ( info->tag ) free( info->tag );
			if ( info->data ) free( info->data );
			if ( info->used ) free( info->used );
			return 0;
		}
		info->maxfields = min_alloc;
		info->nfields = 0;
		for ( i=0; i<min_alloc; ++i ) {
			newstr_init(&(info->tag[i]));
			newstr_init(&(info->data[i]));
		}
	} else if ( info->nfields >= info->maxfields ){
		min_alloc = info->maxfields * 2;
		newtags = (newstring*) realloc( info->tag,
			       	sizeof(newstring) * min_alloc );
		newdata = (newstring*) realloc( info->data,
				sizeof(newstring) * min_alloc );
		newused = (int*)    realloc( info->used,
				sizeof(int) * min_alloc );
		if ( !newtags || !newdata || !newused ) {
			if ( newtags ) info->tag=newtags;
			if ( newdata ) info->data=newdata;
			if ( newused ) info->used=newused;
			return 0;
		}
		info->tag = newtags;
		info->data = newdata;
		info->used = newused;
		info->maxfields = min_alloc;
		for ( i=info->nfields; i<min_alloc; ++i ) {
			newstr_init(&(info->tag[i]));
			newstr_init(&(info->data[i]));
		}
	}
	found = 0;
	for ( i=0; i<info->nfields && !found; ++i ) {
		if ( ( tag && !strcasecmp( info->tag[i].data, tag )) &&
		     ( data && !strcasecmp( info->data[i].data, data ))) found=1;
	}
	if ( !found ) {
		newstr_strcpy( &(info->tag[info->nfields]), tag );
		newstr_strcpy( &(info->data[info->nfields]), data );
		info->used[ info->nfields ] = 0;
		info->nfields++;
	}
	return 1;
}

void
fields_init( fields *info )
{
	info->used = NULL;
	info->tag  = NULL;
	info->data = NULL;
	info->maxfields = info->nfields = 0;
}

void
fields_free( fields *info )
{
	int i;
	for (i=0; i<info->maxfields; ++i) {
		newstr_free( &(info->tag[i]) );
		newstr_free( &(info->data[i]) );
	}
	if ( info->tag )  free( info->tag );
	if ( info->data ) free( info->data );
	if ( info->used ) free( info->used );
}

int
fields_find( fields *info, char *searchtag )
{
	int i, found = -1;
	for ( i=0; i<info->nfields && found==-1; ++i ) {
		if ( strsearch( info->tag[i].data, searchtag ) ) {
			found = i;
			/* if there is no data for the tag, mark as unfound */
			/* but set "used" so noise is suppressed */
			if ( info->data[i].len==0 ) {
				found=-1;
				info->used[i] = 1;
			}
		}
	}
	return found;
}
