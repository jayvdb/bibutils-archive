/*
 * risout.h
 *
 * Copyright (c) Chris Putnam 2005-8
 *
 */
#ifndef RISOUT_H
#define RISOUT_H

#include <stdio.h>
#include "bibutils.h"

extern void risout_write( fields *info, FILE *fp, int format_opts, 
		unsigned long refnum );
extern void risout_writeheader( FILE *outptr, param *p );


#endif
