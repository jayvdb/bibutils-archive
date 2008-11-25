/*
 * bibprogs.c
 *
 * Copyright (c) Chris Putnam 2004-8
 *
 * Program and source code released under the GPL
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newstr.h"
#include "newstr_conv.h"
#include "list.h"
#include "bibl.h"
#include "bibutils.h"
#include "modsout.h"
#include "bibprogs.h"

extern list corps;
extern list asis;

static void
args_tellversion( char *progname )
{
	extern char bibutils_version[];
	extern char bibutils_date[];
	fprintf( stderr, "%s, ", progname );
	fprintf( stderr, "bibutils suite version %s date %s\n", 
		bibutils_version, bibutils_date );
}

static void
args_help( char *progname, char *help1, char *help2 )
{
	args_tellversion( progname );
	fprintf(stderr,"%s", help1 );

	fprintf(stderr,"usage: %s %s > xml_file\n\n", progname, help2 );
        fprintf(stderr,"  %s can be replaced with file list or "
			"omitted to use as a filter\n\n", help2 );

	fprintf(stderr,"  -i, --input-encoding      map character encoding\n");
	fprintf(stderr,"  -u, --unicode-characters  DEFAULT: directly write unicode (and not xml entities)\n");
	fprintf(stderr,"  -x, --xml-characters      write xml entities and not direclty unicode\n");
	fprintf(stderr,"  -un,--unicode-no-bom      as -u, but don't write byte order mark\n");
	fprintf(stderr,"  -nl,--no-latex            do not convert latex-style character combinations\n");
	fprintf(stderr,"  -s, --single-refperfile   one reference per output file\n");
	fprintf(stderr,"  -d, --drop-key            don't put key in MODS ID field\n");
	fprintf(stderr,"  -c, --corporation-file    specify file of corporation names\n");
	fprintf(stderr,"  -a, --asis                specify file of names that shouldn't be mangled\n");
	fprintf(stderr,"  -h, --help                display this help\n");
	fprintf(stderr,"  -v, --version             display version\n");
	fprintf(stderr,"  --verbose                 report all warnings\n");
	fprintf(stderr,"  --debug                   very verbose output\n\n");

	fprintf(stderr,"http://www.scripps.edu/~cdputnam/software/bibutils "
			"for more details\n\n");
}

int
args_match( char *check, char *shortarg, char *longarg )
{
	if ( shortarg && !strcmp( check, shortarg ) ) return 1;
	if ( longarg  && !strcmp( check, longarg  ) ) return 1;
	return 0;
}

static void
args_namelist( int argc, char *argv[], int i, char *progname, list *names,
		char *shortarg, char *longarg )
{
	if ( i+1 >= argc ) {
		fprintf( stderr, "%s: error %s (%s) takes the argument of "
			"the file\n", progname, shortarg, longarg );
		exit( EXIT_FAILURE );
	} else {
		if ( !list_fill( names, argv[i+1] ) ) {
			fprintf( stderr, "%s: warning problems reading '%s', "
				"obtained %d elements\n", progname, 
				argv[i+1], names->n );
		}
	}
}

int
args_charset( char *charset_name, int *charset, unsigned char *utf8 )
{
	if ( !strcasecmp( charset_name, "unicode" ) || 
	     !strcasecmp( charset_name, "utf8" ) ) {
		*charset = BIBL_CHARSET_UNICODE;
		*utf8 = 1;
	} else if ( !strcasecmp( charset_name, "gb18030" ) ) {
		*charset = BIBL_CHARSET_GB18030;
		*utf8 = 0;
	} else {
		*charset = get_charset( charset_name );
		*utf8 = 0;
	}
	if ( *charset == BIBL_CHARSET_UNKNOWN ) return 0;
	else return 1;
}

void
args_encoding( int argc, char *argv[], int i, int *charset, unsigned char *utf8, char *progname )
{
	if ( i+1 >= argc ) {
		fprintf( stderr, "%s: error -i (--input-encoding) takes "
				"the argument of the character set type\n",
				progname );
		list_charsets( stderr );
		exit( EXIT_FAILURE );
	} else {
		if ( !args_charset( argv[i+1], charset, utf8 ) ) {
			fprintf( stderr, "%s: character encoding lookup "
					"failed.\n", progname );
			list_charsets( stderr );
		}
	}
}

void
tomods_processargs( int *argc, char *argv[], param *p, char *progname,
	char *help1, char *help2 )
{
	int i, j, subtract;
	i = 0;
	while ( i<*argc ) {
		subtract = 0;
		if ( args_match( argv[i], "-h", "--help" ) ) {
			subtract = 1;
			args_help( progname, help1, help2 );
			exit( EXIT_SUCCESS );
		} else if ( args_match( argv[i], "-v", "--version" ) ) {
			subtract = 1;
			args_tellversion( progname );
			exit( EXIT_SUCCESS );
		} else if ( args_match( argv[i], "-a", "--add-refcount" ) ) {
			p->addcount = 1;
			subtract = 1;
		} else if ( args_match(argv[i], NULL, "--verbose" ) ) {
			/* --debug + --verbose = --debug */
			if ( p->verbose<1 ) p->verbose = 1;
			p->format_opts |= BIBL_FORMAT_VERBOSE;
			subtract = 1;
		} else if ( args_match(argv[i], NULL, "--debug" ) ) {
			p->verbose = 3;
			p->format_opts |= BIBL_FORMAT_VERBOSE;
			subtract = 1;
		} else if ( args_match( argv[i], "-d", "--drop-key" ) ) {
			p->format_opts |= MODSOUT_DROPKEY;
			subtract = 1;
		} else if ( args_match( argv[i], "-s", "--single-refperfile" )){
			p->singlerefperfile = 1;
			subtract = 1;
		} else if ( args_match( argv[i], "-u", "--unicode-characters")){
			p->utf8out = 1;
			p->utf8bom = 1;
			p->charsetout = BIBL_CHARSET_UNICODE;
			p->charsetout_src = BIBL_SRC_USER;
			subtract = 1;
		} else if ( args_match( argv[i], "-un", "--unicode-no-bom")){
			p->utf8out = 1;
			p->utf8bom = 0;
			p->charsetout = BIBL_CHARSET_UNICODE;
			p->charsetout_src = BIBL_SRC_USER;
			subtract = 1;
		} else if ( args_match( argv[i], "-nl", "--no-latex" ) ) {
			p->latexin = 0;
			subtract = 1;
		} else if ( args_match( argv[i], "-x", "--xml-entities" ) ) {
			p->utf8out = 0;
			p->utf8bom = 0;
			p->xmlout = 1;
			subtract = 1;
		} else if ( args_match( argv[i], "-c", "--corporation-file")){
			args_namelist( *argc, argv, i, progname, &corps,
				"-c", "--corporation-file" );
			subtract = 2;
		} else if ( args_match( argv[i], "-as", "--asis")) {
			args_namelist( *argc, argv, i, progname, &asis,
				"-as", "--asis" );
			subtract = 2;
		} else if ( args_match( argv[i], "-i", "--input-encoding" ) ) {
			args_encoding( *argc, argv, i, &(p->charsetin),
				&(p->utf8in), progname );
			p->charsetin_src = BIBL_SRC_USER;
			subtract = 2;
		}
		if ( subtract ) {
			for ( j=i+subtract; j<*argc; j++ ) {
				argv[j-subtract] = argv[j];
			}
			*argc -= subtract;
		} else i++;
	}
}
