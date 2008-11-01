/*
 * isiout.h
 *
 * Copyright (c) Chris Putnam 2007-8
 *
 */
#ifndef ISIOUT_H
#define ISIOUT_H

#include <stdio.h>
#include "bibutils.h"

extern void isiout_write( fields *info, FILE *fp, int format_opts, 
		unsigned long refnum );
extern void isiout_writeheader( FILE *outptr, param *p );

#endif
