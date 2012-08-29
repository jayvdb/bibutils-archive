/*
 * risout.h
 *
 * Copyright (c) Chris Putnam 2005-2012
 *
 */
#ifndef RISOUT_H
#define RISOUT_H

#include <stdio.h>
#include "bibutils.h"

extern void risout_write( fields *info, FILE *fp, param *p,
		unsigned long refnum );
extern void risout_writeheader( FILE *outptr, param *p );

extern void risout_initparams( param *p, const char *progname );


#endif
