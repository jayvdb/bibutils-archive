/*
 * modsclean.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "bibutils.h"
#include "modsin.h"
#include "modsout.h"
#include "tomods.h"
#include "bibprog.h"

const char progname[] = "modsclean";

int
main( int argc, char *argv[] )
{
	param p;
	modsin_initparams( &p, progname );
	modsout_initparams( &p, progname );
	tomods_processargs( &argc, argv, &p, "", "" );
	bibprog( argc, argv, &p );
	bibl_freeparams( &p );
	return EXIT_SUCCESS;
}
