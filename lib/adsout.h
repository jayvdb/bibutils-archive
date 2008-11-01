/*
 * adsout.h
 *
 * Copyright (c) Richard Mathar 2007-8
 * Copyright (c) Chris Putnam 2007-8
 *
 */
#ifndef ADSOUT_H
#define ADSOUT_H

#include <stdio.h>
#include "bibutils.h"

extern void adsout_write( fields *info, FILE *fp, int format_opts,
		unsigned long refnam );
extern void adsout_writeheader( FILE *outptr, param *p );

#endif
