/*
 * wordbib2xml.c
 * 
 * Copyright (c) Chris Putnam 2009-2012
 *
 * Source code and program released under the GPL
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "bibutils.h"
#include "wordin.h"
#include "modsout.h"
#include "tomods.h"
#include "bibprog.h"

char help1[] =  "Converts a Word2007 Bibliography XML file into MODS XML\n\n";
char help2[] = "word2007bib_file";

const char progname[] = "wordbib2xml";

int
main( int argc, char *argv[] )
{
	param p;
	wordin_initparams( &p, progname );
	modsout_initparams( &p, progname );
	tomods_processargs( &argc, argv, &p, help1, help2 );
	bibprog( argc, argv, &p );
	bibl_freeparams( &p );
	return EXIT_SUCCESS;
}
