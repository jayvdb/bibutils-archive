/*
 * ris2xml.c
 *
 * Copyright (c) Chris Putnam 2003-2004
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

char progname[] = "ris2xml";
char version[] = "1.12 07/19/04";

void
ris_xmltype( fields *info )
{
	int n = fields_find( info, "TY" );
	if ( n!=-1 ) {
		if (!strncasecmp(info->data[n].data,"BOOK",4))
			printf("      <TYPE>BOOK</TYPE>\n");
		else if (!strncasecmp(info->data[n].data,"CHAP",4))
			printf("      <TYPE>INBOOK</TYPE>\n");
		else if (!strncasecmp(info->data[n].data,"THES",4))
			printf("      <TYPE>PHDTHESIS</TYPE>\n");
		else    printf("      <TYPE>ARTICLE</TYPE>\n");
	}
}

void
ris_xmlpages( fields *info )
{
	char *p;
	int sp = fields_find( info, "SP" );
	int ep = fields_find( info, "EP" );
	if ( sp==-1 && ep==-1 ) return;
	printf("      <PAGES>\n");
	if ( sp!=-1 ) {
		newstr_encodexml( &(info->data[sp]) );
		p = info->data[sp].data;
		if ( p ) {
			while ( is_ws( *p ) ) p++;
			printf("        <START>%s</START>\n",p);
		}
	}
	if ( ep!=-1 ) {
		newstr_encodexml( &(info->data[ep]) );
		p = info->data[ep].data;
		if ( p ) {
			while ( is_ws( *p ) ) p++;
			printf("        <END>%s</END>\n",p);
		}
	}
	printf("      </PAGES>\n");
}

/* name is in format Lastname,Firstname(or inits),Suffix */
void
ris_xmlname( char *p )
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

	while ( *p  ) {
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
ris_xmlnames( char *tag, char *value )
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
				ris_xmlname( name.data );
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
ris_xmlkeyword( char *value )
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
			if ( keyword.data!=NULL || keyword.len>0 || 
			     (value[i]!=' ' && value[i]!='\t'))
				newstr_addchar( &keyword, value[i] );
		}
	}
	printf("        <KEYWORD>%s</KEYWORD>\n",keyword.data );
	newstr_free( &keyword );
}

/* potentially multiple keywords, separated by keywords */
void
ris_xmlkeywords( fields *info )
{
	int i,found=0;

	for (i=0; i<info->nfields; ++i ) {
		if ( !(strcasecmp(info->tag[i].data,"KW"))) {
			if ( !found ) {
				printf("      <KEYWORDS>\n");
			}
			newstr_encodexml( &(info->data[i]) );
			ris_xmlkeyword( info->data[i].data );
			found++;
		}
	}
	if ( found ) printf("      </KEYWORDS>\n");
}

void
ris_xmldate( fields *info )
{
	char *tags[]={ "PY", "Y1", "Y2" };
	int  ntags = sizeof(tags)/sizeof(char*);
	int  j,n=-1;
	char *p;
	for (j=0; j<ntags && n==-1; ++j) {

		n = fields_find( info, tags[j] );
		if ( n==-1 ) continue;

		printf("      <DATE>");
		newstr_encodexml( &(info->data[n]) );

		/* output year */
		printf("<YEAR>");
		p = info->data[n].data;
		while ( p && (*p==' ' || *p=='\t') ) p++;
		while ( p && *p && *p!='/') printf("%c",*p++);
		printf("</YEAR>");

		/* output month */
		if ( p && *p=='/' ) p++;
		while ( p && *p && (*p==' ' || *p=='\t' || *p=='\r' || *p=='\n')) p++;
		if ( p && *p && *p!='/' ) {
			printf("<MONTH>");
			while ( p && (*p==' ' || *p=='\t') ) p++;
			while ( p && *p && *p!='/' ) 
				printf("%c",*p++);
			printf("</MONTH>");
		}

		/* output day */
		if ( p && *p=='/' ) p++;
		while ( p && *p && (*p==' ' || *p=='\t' || *p=='\r' || *p=='\n')) p++;
		if ( p && *p && *p!='/' ) {
			printf("<DAY>");
			while ( p && (*p==' ' || *p=='\t') ) p++;
			while ( p && *p && *p!='/' ) 
				printf("%c",*p++);
			printf("</DAY>");
		}

		/* output other */
		if ( p && *p=='/' ) p++;
		while ( p && *p && (*p==' ' || *p=='\t' || *p=='\r' || *p=='\n')) p++;
		if ( p && *p && *p!='/' ) {
			printf("<OTHER>");
			while ( p && (*p==' ' || *p=='\t') ) p++;
			while ( p && *p && *p!='/' ) 
				printf("%c",*p++);
			printf("</OTHER>");
		}

		printf("</DATE>\n");
	}
}

void
ris_xmlmakerefnum( fields *info )
{
	int year, name;
	char *p;

	year = fields_find( info, "PY" );
	if ( year==-1 ) year = fields_find( info, "Y1" );

	name = fields_find( info, "AU" );
	if ( name==-1 ) name = fields_find( info, "A1" );

	if (year!=-1 && name!=-1) {
		printf("      <REFNUM>");
		p = info->data[name].data;
		while ( p && *p && *p!=',' && *p!=' ' && *p!=';' && *p!='\t' &&
		       	*p!='\r' && *p!='\n') printf("%c",*p++);
		p = info->data[year].data;
		while ( p && *p && ( *p==' ' || *p=='\t' ) ) p++;
		while ( p && *p && *p!=',' && *p!='/' && *p!='\t' && *p!='\r'
			&& *p!='\n') printf("%c",*p++);
		printf("</REFNUM>\n");
	}
}

void
ris_xmlrefnum( fields *info )
{
	int n=fields_find( info, "ID" );
	if ( n!=-1 ) {
		newstr_encodexml( &(info->data[n]) );
		printf("      <REFNUM>%s</REFNUM>\n", info->data[n].data );
	} else {
		ris_xmlmakerefnum( info );
	}
}

void
ris_xmlpeople( fields *info, char *oldtags[], char *xmltag, char *plural,
	    char all )
{
	int i,j=0,n=0;
	while ( oldtags[j][0]!='\0' && ( all || n==-1 ) ) {
		for (i=0; i<info->nfields; ++i ) {
			if (!strcasecmp(info->tag[i].data,oldtags[j])) {
				if ( !n ) printf("      <%s>\n", plural);
				ris_xmlnames( xmltag, info->data[i].data );
				n++;
			}
		}
		j++;
	}
	if ( n ) printf( "      </%s>\n", plural );
}

void
ris_xmleasy( fields *info, char *oldtags[], char *xmltag, char all )
{
	int i = 0, n = -1;
	char *p, *q;
	while ( oldtags[i][0]!='\0' && ( all || n==-1 ) ) {
		n = fields_find( info, oldtags[i] );
		if ( n!=-1 ) {
			newstr_encodexml( &(info->data[n]) );
			p = info->data[n].data;
			while ( is_ws( *p ) ) p++;
			q = &(info->data[n].data[info->data[n].len]);
			while ( q!=p && (q-1)!=p && is_ws(*(q-1)) ) q--;
			printf("      <%s>",xmltag);
			while ( p!=q )
				printf("%c",*p++);
			printf("</%s>\n",xmltag);
		}
		i++;
	}
}

void
ris_xmlout( fields *info )
{
	enum modes { SIMPLE, PEOPLE, PAGES, DATE, KEYWORD, REFNUM };
	typedef struct rismapping {
		char mode;
		char *oldtags[6];
		char *newtag;
		char *plural;
		char all;
	} rismapping;
	rismapping mapping[] = {
		{ PEOPLE, { "AU", "A1", "A3", "" }, "AUTHOR", "AUTHORS", 1 },
		{ SIMPLE, { "T1", "TI", "CT", "" }, "TITLE", "", 0 },
		{ SIMPLE, { "T2", "" }, "SECONDARYTITLE", "", 0 },
		{ SIMPLE, { "T3", "" }, "SERIESTITLE", "", 0 },
		{ SIMPLE, { "BT", "" }, "BOOKTITLE", "", 0 },
		{ SIMPLE, { "J1", "J2", "JO", "JA", "JF", "" }, "JOURNAL", "", 0 },
		{ DATE,   { "" }, "", "", 0 },
		{ SIMPLE, { "VL", "" }, "VOLUME", "", 0 },
		{ SIMPLE, { "IS", "CP", "" }, "ISSUE", "", 0 },
		{ PAGES,  { "" }, "", "", 0 },
		{ PEOPLE, { "ED", "A2", "" }, "EDITOR", "EDITORS", 1 },
		{ SIMPLE, { "PB", "" }, "PUBLISHER", "", 0 },
		{ PEOPLE, { "A3", "" }, "AUTHOR", "SERIESAUTHORS", 1 },
		{ SIMPLE, { "CT", "AD", "CY", "" }, "ADDRESS", "", 1 },
		{ SIMPLE, { "SN", "" }, "SERIALNUM", "", 0 },
		{ SIMPLE, { "RP", "" }, "REPRINTSTATUS", "", 0 },
		{ SIMPLE, { "UR", "" }, "URL", "", 0 },
		{ SIMPLE, { "N1", "" }, "NOTES", "", 0 },
		{ SIMPLE, { "AB", "N2", "" }, "ABSTRACT", "", 1 },
		{ KEYWORD,  { "" }, "", "", 0 },
		{ REFNUM,  { "" }, "", "", 0 }
	};
	int i, n = sizeof(mapping)/sizeof(rismapping);
	printf("  <REF>\n");
	ris_xmltype( info );
	for ( i=0; i<n; ++i ) {
		if ( mapping[i].mode == SIMPLE ) {
			ris_xmleasy( info, 
					mapping[i].oldtags, 
					mapping[i].newtag,
				   	mapping[i].all );
		} else if ( mapping[i].mode == PEOPLE ) {
			ris_xmlpeople( info, 
					mapping[i].oldtags, 
					mapping[i].newtag, 
					mapping[i].plural, 
					mapping[i].all );
		} else if ( mapping[i].mode == DATE ) {
			ris_xmldate( info );
		} else if ( mapping[i].mode == PAGES ) {
			ris_xmlpages( info );
		} else if ( mapping[i].mode == KEYWORD ) {
			ris_xmlkeywords( info );
		} else if ( mapping[i].mode == REFNUM ) {
			ris_xmlrefnum( info );
		}
	}
	printf("  </REF>\n");
}

/* RIS definition of a tag is strict:
    character 1 = uppercase alphabetic character
    character 2 = uppercase alphabetic character or digit
    character 3 = space (ansi 32)
    character 4 = space (ansi 32)
    character 5 = dash (ansi 45)
    character 6 = space (ansi 32)
*/
int
ris_istag( char *buf )
{
	if (! (buf[0]>='A' && buf[0]<='Z') ) return FALSE;
	if (! (((buf[1]>='A' && buf[1]<='Z'))||(buf[1]>='0'&&buf[1]<='9')) ) return FALSE;
	if (buf[2]!=' ') return FALSE;
	if (buf[3]!=' ') return FALSE;
	if (buf[4]!='-') return FALSE;
	if (buf[5]!=' ') return FALSE;
	return TRUE;
}

long 
ris_read( FILE *fp )
{
	newstring line;
	fields reffields;
	char buf[1024]="", *p;
	char tag[3];
	long numrefs = 0;
	int i;
	int inref = FALSE;
	int bufpos = 0;

	newstr_init( &line );
	fields_init( &reffields );

	while( newstr_fget( fp, buf, sizeof(buf), &bufpos, &line ) ) {
		if ( line.len == 0 ) continue;  /* blank line */
		p = &(line.data[0]);

		if ( inref && !strncmp( p, "TY  - ", 6 ) ) {
			fprintf(stderr,"Warning.  Reference %ld "
				"not properly terminated.\n", numrefs+1 );
			inref = FALSE;
		} else if ( inref && !strncmp( p, "ER  - ", 6 ) ) {
			inref = FALSE;
		}

		if ( reffields.nfields && !inref ) {
			ris_xmlout( &reffields );
			numrefs++;
			reffields.nfields = 0;
		}

		/* Each reference starts with 'TY  - ' && ends with 'ER  - ' */
		if ( ris_istag( p ) && strncmp( p, "ER", 2 ) ) {
			for ( i=0; i<2; ++i ) tag[i] = p[i];
			tag[2]='\0';
			if ( !strncmp( p, "TY", 2 ) ) inref = TRUE;
			if ( inref ) fields_add( &reffields, tag, p+6 );
			else {
				fprintf(stderr,"Warning.  Tagged line not in properly started reference.\n");
				fprintf( stderr, "Ignored: '%s'\n", p );
			}
		}
		/* not a tag, but we'll append to last values ...*/
		else if ( inref ) {
			newstr_addchar( &(reffields.data[reffields.nfields-1]),' ' );
			newstr_strcat( &(reffields.data[reffields.nfields-1]),
					p );
		}
	}

	if ( reffields.nfields ) {
		ris_xmlout( &reffields );
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
	fprintf(stderr,"Converts a RIS reference file into XML\n\n");

	fprintf(stderr,"usage: %s ris_file > xml_file\n\n",progname);
        fprintf(stderr,"  ris_file can be replaced with file list or omitted to use as a filter\n\n");

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

	if (argc==1)  {
		printf("<XML>\n");
		printf("<REFERENCES>\n");
		numref = ris_read( stdin );
		printf("</REFERENCES>\n");
		printf("</XML>\n");
	} else {
		printf("<XML>\n");
		printf("<REFERENCES>\n");
		for (i=1; i<argc; i++) {
			fp=fopen(argv[i],"r");
			if (fp==NULL) {
				fprintf(stderr,"ris2xml %s: cannot open %s\n",
					version, argv[i]);
			} else { 
				numref += ris_read( fp );
				fclose(fp);
			}
		}
		printf("</REFERENCES>\n");
		printf("</XML>\n");
	}
	fprintf(stderr,"ris2xml %s:  Processed %ld references.\n",
		version, numref);
	return EXIT_SUCCESS;
}
 

