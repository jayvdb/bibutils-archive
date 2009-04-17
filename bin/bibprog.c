/*
 * bibprog.c
 *
 * Copyright (c) 2004-8
 *
 * Program and source code released under the GPL
 *
 */
#include <stdio.h>
#include "bibutils.h"
#include "bibprog.h"

void
bibprog( int argc, char *argv[], param *p )
{
	FILE *fp;
	bibl b;
	int err, i;

	bibl_init( &b );
	if ( argc<2 ) {
		err = bibl_read( &b, stdin, "stdin", p );
		if ( err ) bibl_reporterr( err ); 
	} else {
		for ( i=1; i<argc; ++i ) {
			fp = fopen( argv[i], "r" );
			if ( fp ) {
				err = bibl_read( &b, fp, argv[i], p );
				if ( err ) bibl_reporterr( err );
				fclose( fp );
			}
		} 
	}
	bibl_write( &b, stdout, p );
	fflush( stdout );
	if( p->progname ) fprintf( stderr, "%s: ", p->progname );
	fprintf( stderr, "Processed %ld references.\n", b.nrefs );
	bibl_free( &b );
}

