/*
 * fields.h
 */
#ifndef FIELDS_H
#define FIELDS_H

#include "newstr.h"

typedef struct {
	newstring *tag;
	newstring *data;
	int       *used;
	int       nfields;
	int       maxfields;
	int       type;
} fields;

extern int  fields_add( fields *info, char *tag, char *data );
extern void fields_free( fields *info );
extern void fields_init( fields *info );
extern int  fields_find( fields *info, char *searchtag );

#endif
