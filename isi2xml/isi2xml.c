/*
 * isi2xml.c
 *
 * Copyright (c) Chris Putnam 2004
 *
 * Program and source code released under the GPL
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newstr.h"
#include "fields.h"
#include "is_ws.h"

#define TRUE (1)
#define FALSE (0)

char progname[] = "isi2xml";
char version[] = "1.2 07/20/04";

void
isi_xmltype( fields *info )
{
	int n = fields_find( info, "PT " );
	if ( n!=-1 ) {
		if (strncasecmp(info->data[n].data,"BOOK",4)==0)
			printf("      <TYPE>BOOK</TYPE>\n");
		else if (strncasecmp(info->data[n].data,"CHAP",4)==0)
			printf("      <TYPE>INBOOK</TYPE>\n");
		else if (strncasecmp(info->data[n].data,"THES",4)==0)
			printf("      <TYPE>PHDTHESIS</TYPE>\n");
		else    printf("      <TYPE>ARTICLE</TYPE>\n");
	}
}

void
isi_xmlpages( fields *info )
{
	int sp = fields_find( info, "BP " );
	int ep = fields_find( info, "EP " );
	if ( sp==-1 && ep==-1 ) return;
	printf("      <PAGES>\n");
	if (sp!=-1) {
		newstr_encodexml( &(info->data[sp]) );
		printf("        <START>%s</START>\n",info->data[sp].data);
	}
	if (ep!=-1) {
		newstr_encodexml( &(info->data[ep]) );
		printf("        <END>%s</END>\n",info->data[ep].data);
	}
	printf("      </PAGES>\n");
}

/* name is in format Lastname,Firstname(or inits),Suffix */
void
isi_xmlname( char *p )
{
	int suffix = 0;

	if ( !p ) return;
	while ( is_ws( *p ) ) p++;

	if ( *p ) {
		printf("<LAST>");
		while ( *p && *p!=',') printf("%c",*p++);
		printf("</LAST>");
	}

	while ( *p==',' || is_ws( *p ) ) p++;

	while ( *p ) {
		if ( !strncasecmp( p, "Jr", 2 ) || !strncasecmp( p, "III", 3 ) )
			suffix = 1;
		if ( !suffix ) {
			printf("<PREF>");
			while ( *p && *p!=' ' && *p!=',' && *p!='.' ) {
				printf("%c.",*p++);
				while ( *p && *p!=' ' && *p!=',' && *p!='.' )
					p++;
			}
			printf("</PREF>");
		} else {
			printf("<SUFF>");
			while ( *p && *p!=' ' && *p!=',' && *p!='.' ) {
				printf("%c",*p++);
			}
			printf("</SUFF>");
			suffix = 0;
		}
		while ( is_ws( *p ) || *p=='.' || *p=='-' || *p==',' ) p++;
	}
}

/* potentially multiple names separated by semicolons */
void
isi_xmlnames( char *tag, char *value )
{
	newstring name;
	char *p;

	newstr_init( &name );
	p = value;
	while ( p && (p==value || *(p-1)) ) {
		if ( *p==';' || *p=='\0' ) {
			if ( name.len ) {
				printf("        <%s>",tag); 
				newstr_encodexml( &name );
				isi_xmlname( name.data );
				printf("</%s>\n",tag);
				newstr_empty( &name );
			}
		} else {
			if ( name.len || ( *p!=' ' && *p!='\t' ) )
				newstr_addchar( &name, *p );
		}
		p++;
	}
	newstr_free( &name );
}

void
isi_xmlpeople( fields *info, char *oldtags[], char *xmltag, char *plural,
		char all )
{
	int i, j=0, n=0;
	while ( oldtags[j][0]!='\0' && ( all || n==-1 ) ) {
		for ( i=0; i<info->nfields; ++i ) {
			if ( !strcasecmp(info->tag[i].data,oldtags[j])) {
				if ( !n ) printf("      <%s>\n", plural);
				isi_xmlnames( xmltag, info->data[i].data );
				n++;
			}
		}
		j++;
	}
	if ( n ) printf( "      </%s>\n", plural );
}

void
isi_xmlkeyword( char *value )
{
	newstring keyword;
	int i, len_value;

	newstr_init( &keyword );
	len_value = strlen( value );
	for ( i=0; i<len_value; ++i ) {
		if ( value[i]==';' ) {
			if (keyword.data) {
			newstr_encodexml( &keyword );
			printf("        <KEYWORD>%s</KEYWORD>\n",keyword.data );
			newstr_empty( &keyword );
			}
		} else {
			if ( keyword.data==NULL || strlen(keyword.data)>0 || 
			     (value[i]!=' ' && value[i]!='\t'))
				newstr_addchar( &keyword, value[i] );
		}
	}
	newstr_encodexml( &keyword );
	printf("        <KEYWORD>%s</KEYWORD>\n",keyword.data );
	newstr_free( &keyword );
}

/* potentially multiple keywords, separated by keywords */
void
isi_xmlkeywords( fields *info )
{
	int i,found=0;
	for (i=0; i<info->nfields; ++i ) {
		if ((!strncmp(info->tag[i].data,"ID ",3) ||
		     !strncmp(info->tag[i].data,"DE ",3) ) ) {
			if ( !found ) printf("      <KEYWORDS>\n");
			newstr_encodexml( &(info->data[i]) );
			isi_xmlkeyword( info->data[i].data );
			found++;
		}
	}
	if ( found ) printf("      </KEYWORDS>\n");
}

void
isi_xmldate( fields *info )
{
	char *checktags[2]={ "PY ", "PD " };
	int  found[2] = { -1, -1 }, any = -1;
	int  i;
	char *p;
	for ( i=0; i<2; ++i ) {
		found[i] = fields_find( info, checktags[i] );
		if ( found[i]!=-1 ) any=found[i];
	}
	if ( any==-1 ) return;
	printf("      <DATE>");
	/* output year */
	if ( found[0]!=-1 ) {
		newstr_encodexml( &(info->data[found[0]]) );
		printf( "<YEAR>%s</YEAR>", info->data[found[0]].data );
	}
	/* output month and day */
	if ( found[1]!=-1 ) {
		newstr_encodexml( &(info->data[found[1]]) );
		printf("<MONTH>");
		p = info->data[found[1]].data;
		while ( p && *p && *p!=' ' && *p!='\t' ) printf("%c",*p++); 
		printf("</MONTH>");
		while ( p && ( *p==' ' || *p=='\t' ) ) p++;
		if ( *p ) {
			printf("<DAY>");
			while ( p && *p && *p!=' ' && *p!='\t' ) 
				printf("%c",*p++);
			printf("</DAY>");
		}
	}
	printf("</DATE>\n");
}

void
isi_xmlrefnum( fields *info )
{
	int year = fields_find( info, "PY " );
	int name = fields_find( info, "AU " );
	char *p;
	if (year!=-1 && name!=-1) {
		printf("      <REFNUM>");
		p = info->data[name].data;
		while ( p && *p && *p!=',' && *p!=' ' && *p!=';' && *p!='\t' &&
		       	*p!='\r' && *p!='\n') printf("%c",*p++);
		p = info->data[year].data;
		while ( p && *p && *p!=',' && *p!='/' && *p!='\t' && *p!='\r'
			&& *p!='\n') printf("%c",*p++);
		printf("</REFNUM>\n");
	}
}

void
isi_xmleasy( fields *info, char *oldtags[], char *xmltag, char all )
{
	int i = 0, n = -1;
	char *p, *q;
	while ( oldtags[i][0]!='\0' && ( all || n==-1 ) ) {
		n = fields_find( info, oldtags[i] );
		if ( n!=-1 ) {
			newstr_encodexml( &(info->data[n] ) );
			p = info->data[n].data;
			while ( is_ws( *p ) ) p++;
			q = &(info->data[n].data[info->data[n].len]);
			while ( q!=p && (q-1)!=p && is_ws(*(q-1)) ) q--;
			printf("      <%s>", xmltag );
			while ( p!=q )
				printf( "%c", *p++ );
			printf("</%s>\n", xmltag );
		}
		i++;
	}
}

void
outputref( fields *info )
{
	enum modes { SIMPLE, PEOPLE, PAGES, DATE, KEYWORD, REFNUM };
	typedef struct isimapping {
		char mode;
		char *oldtags[5];
		char *newtag;
		char *plural;
		char all;
	} isimapping;
	isimapping mapping[] = {
		{ PEOPLE, { "AU ", "" }, "AUTHOR", "AUTHORS", 1 },
		{ SIMPLE, { "TI ", "" }, "TITLE",  "", 0 },
		{ SIMPLE, { "SE ", "" }, "SERIESTITLE", "", 0 },
		{ SIMPLE, { "JI ", "SO " }, "JOURNAL", "", 0 },
		{ DATE, { "" }, "", "", 0 },
		{ SIMPLE, { "VL ", "" }, "VOLUME", "", 0 },
		{ SIMPLE, { "IS ", "" }, "ISSUE", "", 0 },
		{ PAGES, { "" }, "", "", 0 },
		{ SIMPLE, { "PU ", "" }, "PUBLISHER", "", 0 },
		{ SIMPLE, { "PI ", "" }, "PUBCITY", "", 0 },
		{ SIMPLE, { "PA ", "" }, "ADDRESS", "", 0 },
		{ SIMPLE, { "SN ", "" }, "SERIALNUM", "", 0 },
		{ SIMPLE, { "NF ", "" }, "NOTES", "", 0 },
		{ SIMPLE, { "AB ", "" }, "ABSTRACT", "", 0 },
		{ KEYWORD, { "" }, "", "", 0 },
		{ REFNUM, { "" }, "", "", 0 }
	};
	int i, n = sizeof( mapping ) / sizeof( isimapping );
	printf("  <REF>\n");
	isi_xmltype( info );
	for ( i=0; i<n; ++i ) {
		if ( mapping[i].mode == SIMPLE ) {
			isi_xmleasy( info,
					mapping[i].oldtags,
					mapping[i].newtag,
					mapping[i].all );
		} else if ( mapping[i].mode == DATE ) {
			isi_xmldate( info );
		} else if ( mapping[i].mode == PAGES ) {
			isi_xmlpages( info );
		} else if ( mapping[i].mode == KEYWORD ) {
			isi_xmlkeywords( info );
		} else if ( mapping[i].mode == REFNUM ) {
			isi_xmlrefnum( info );
		} else if ( mapping[i].mode == PEOPLE ) {
			isi_xmlpeople( info,
					mapping[i].oldtags,
					mapping[i].newtag,
					mapping[i].plural,
					mapping[i].all );
		}
	}
	printf("  </REF>\n");
}

/* ISI definition of a tag is strict:
    character 1 = uppercase alphabetic character
    character 2 = uppercase alphabetic character or digit
*/
int
is_tag( char *buf )
{
	if (! (buf[0]>='A' && buf[0]<='Z') ) return FALSE;
	if (! (((buf[1]>='A' && buf[1]<='Z'))||(buf[1]>='0'&&buf[1]<='9')) ) return FALSE;
	return TRUE;
}

long 
isi_read( FILE *fp )
{
	char buf[1024]="", *p;
	char tag[4];
	fields reffields;
	newstring line;
	int i;
	long numrefs = 0;
	int bufpos = 0;

	newstr_init( &line );
	fields_init( &reffields );

	while( newstr_fget( fp, buf, sizeof(buf), &bufpos, &line ) ) {
		if ( line.len == 0 ) continue;  /* blank line */
		p = &(line.data[0]);

		/* Each reference ends with 'ER ' */
		if (is_tag( p ) ) {
			for ( i=0; i<3; ++i ) tag[i]=p[i];
			tag[3] = '\0';
			if ( !strncmp( p, "FN ", 3 ) ) {
				if ( strncasecmp( p, "FN ISI Export Format",20)) {
					fprintf(stderr,"%s: warning file FN type not '%s' not recognized.\n", progname, p );
				}
			} else if ( !strncmp( p, "VR ", 3 ) ) {
				if ( strncasecmp( p, "VR 1.0", 6 ) ) {
					fprintf(stderr,"%s: warning file version number '%s' not recognized, expected 'VR 1.0'\n", progname, p );
				}
			} else if ( !strncmp( p, "ER",  2 ) ) {
				outputref( &reffields );
				reffields.nfields = 0;
				numrefs++;
			} else
				fields_add( &reffields, tag, p+3 );
		}
		/* not a tag, but we'll append to last values ...*/
		else if ( reffields.nfields>0 ) {
			int i;
			/* subsequent authors aren't marked with AU tags */
			if ( !strncasecmp((reffields.tag[reffields.nfields-1]).data,"AU ",3 ) ) {
				fields_add( &reffields, "AU ", p+3 );
			}
			else {
				for ( i=0; i<3; i++ ) if ( p && *p ) p++;
				newstr_addchar( &(reffields.data[reffields.nfields-1]), ' ' );
				while ( *p ) {
					newstr_addchar(&(reffields.data[reffields.nfields-1]), *p );
					p++;
				}
			}
		} else {
			fprintf( stderr, "%s warning: '%s' outside of tag.\n",
					progname, p );
		}
	}

	if ( reffields.nfields ) {
		outputref( &reffields );
		numrefs++;
	}

	newstr_free( &line );
	fields_free( &reffields );

	return numrefs;
}

void
help( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"\n%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	fprintf(stderr,"Converts a ISI reference file into XML\n\n");

	fprintf(stderr,"usage: %s isi_file > xml_file\n\n",progname);
        fprintf(stderr,"  isi_file can be replaced with file list or "
			"omitted to use as a filter\n\n");

	fprintf(stderr,"  -h, --help     display this help\n");
	fprintf(stderr,"  -v, --version  display version\n\n");

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
main(int argc, char *argv[])
{
	int 	i;
	FILE 	*fp;
	long 	numref = 0L;

	process_args( &argc, argv );

	if ( argc==1 ) {
		printf("<XML>\n");
		printf("<REFERENCES>\n");
		numref = isi_read( stdin );
		printf("</REFERENCES>\n");
		printf("</XML>\n");
	} else {
		printf("<XML>\n");
		printf("<REFERENCES>\n");
		for (i=1; i<argc; i++) {
			fp = fopen( argv[i], "r" );
			if ( !fp ) {
				fprintf(stderr,"isi2xml %s: cannot open %s\n",
					version, argv[i]);
			} else { 
				numref += isi_read( fp );
				fclose(fp);
			}
		}
		printf("</REFERENCES>\n");
		printf("</XML>\n");
	}
	fprintf(stderr,"isi2xml %s:  Processed %ld references.\n",
		version, numref);
	return EXIT_SUCCESS;
}
 

