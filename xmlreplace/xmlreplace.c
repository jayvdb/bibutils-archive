/*-------------------------------------------------------

xmlreplace -- find/replace elemments in bibliography XML

-------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newstr.h"
#include "strsearch.h"
#include "findrepl.h"
#include "tree.h"
#include "xml.h"

char progname[] = "xmlreplace";
char version[]  = "1.1 12/03/03";

findreplace *findreplacelist = NULL;

/*
 * process_article()
 */
void
process_article( FILE *outptr, newstring *s, long refnum )
{
	node *topnode;
	topnode = node_build( "REF", s->data );
	if ( topnode ) {
		node_findreplace( findreplacelist, topnode );
		node_output( outptr, topnode );
		node_free( topnode );
		free( topnode );
	}
}

void
help( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"\n%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	fprintf(stderr,"Does substitutions in an XML intermediate file.\n\n");

	fprintf(stderr,"usage: %s subs_file xml_file > newxml_file\n\n",progname);
        fprintf(stderr,"  xml_file can be replaced with file list or omitted to use as a filter\n");
        fprintf(stderr,"  subs_file in the format sepFIELDsepTOREPLACEsepREPLACE\n");
	fprintf(stderr,"    sep=separating character, can be different on each line\n");
	fprintf(stderr,"    FIELD=XML field to do replacement in (case-insensitive)\n");
	fprintf(stderr,"    TOREPLACE=text to be modified (case-sensitive)\n");
	fprintf(stderr,"    REPLACE=text to be substitute (blank does deletion)\n\n");

	fprintf(stderr,"  -h, --help     display this help\n");
	fprintf(stderr,"  -v, --version  display version\n\n");

	fprintf(stderr,"Note replacements are done in the order they exist in subs_file.  See\n");
	fprintf(stderr,"http://www.scripps.edu/~cdputnam/bibutils.html for more details\n\n");
	exit( EXIT_SUCCESS );
}

void
tellversion( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	exit( EXIT_SUCCESS );
}

void
process_args( int *argc, char *argv[] )
{
	int i;
	for ( i=0; i<*argc; ++i ) {
		if ( strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0 ) {
			help();
			/* help terminates or we'd remove from argv */
		}
		if ( strcmp(argv[i],"-v")==0 || strcmp(argv[i],"--version")==0){
			tellversion();
			/* tellversion terminates or we'd remove from argv*/
		}
	}
}


int
main( int argc, char *argv[] )
{
	FILE 	*inptr=stdin, *outptr=stdout, *fp;
	long    nrefs;
	int  	i;

	if (argc<2) help();
	process_args( &argc, argv );

	findreplacelist = readlist( argv[1] );
	if ( findreplacelist==NULL ) {
		fprintf(stderr,"%s: could not open substitution file %s\n",
				progname, argv[1] );
		exit(EXIT_FAILURE);
	}

	if ( argc==2 ) nrefs = xml_readrefs( inptr, outptr );
	else {
		fprintf(outptr,"<XML>\n<REFERENCES>\n");
		for (i=2; i<argc; ++i) {
			fp = fopen( argv[i], "r" );
			if ( fp == NULL ) {
				fprintf(stderr,"%s: could not open xml file %s\n",progname,argv[i]);
			} else {
				nrefs += xml_readrefs( fp, outptr );
				fclose(fp);
			}
		}
		fprintf(outptr,"</REFERENCES>\n</XML>\n");
	}

	return EXIT_SUCCESS;
}
