/*
 * bib2xml.c
 *
 * Copyright (c) Chris Putnam 2003-2004
 *
 * Program and source code released under the GPL
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "strsearch.h"
#include "newstr.h"
#include "is_ws.h"
#include "bibstr.h"
#include "fields.h"

#define TRUE (1==1)
#define FALSE (!TRUE)

char progname[] = "bib2xml";
char version[] = "1.8 7/22/04";

char *bibtex_type[] = {
		"@ARTICLE", "@BOOK", "@INBOOK", "@INPROCEEDINGS", 
		"@INCOLLECTION", "@PHDTHESIS", "@TECHREPORT", "@REPORT" };
char *xml_type[] = {
		"ARTICLE", "BOOK", "INBOOK", "INBOOK", 
		"INBOOK",  "PHDTHESIS", "REPORT", "REPORT" };
int  ntypes = sizeof(bibtex_type)/sizeof(char*);

int  verbose = 0;

void 
strip_spaces(char *str)
{
	char *p,*q;
	p = q = str;
	while (*(p-1)) {
		if (*p!=' ') *q++=*p;
		p++;
	}
}

void 
extract_pages( char *p, newstring *sp, newstring *ep )
{
	if ( p ) {
		strip_spaces( p );
		while ( *p && isdigit(*p) ) { 
			newstr_addchar(sp,*p); 
			p++; 
		}
		while ( *p && !isdigit(*p) )
			p++;
		while ( *p && isdigit(*p) ) {
			newstr_addchar(ep,*p); 
			p++; 
		}
	}
}

char *
add_word( newstring *s, char *tag, char *p, char *term )
{
	if ( tag[0] ) {
		newstr_addchar( s, '<' );
		newstr_strcat( s, tag );
		newstr_addchar( s, '>' );
	}
	while ( *p && !strchr(term,*p) ) newstr_addchar( s, *p++ );
	if ( tag[0] ) {
		newstr_addchar( s, '<' );
		newstr_addchar( s, '/' );
		newstr_strcat( s, tag );
		newstr_addchar( s, '>' );
	}
	return p;
}

#define MAXELEMENT (100)
/* extract_name1()
 *
 * extract names in the format "H. F. Author", w/o comma
 */ 
void
extract_name1( char *p, newstring *s )
{
	char *name[MAXELEMENT];
	int i, n = 0;
	while ( *p ) {
		while ( is_ws( *p ) ) p++;
		if ( *p ) name[n++] = p;
		if ( n==MAXELEMENT ) n=MAXELEMENT-1;
		while ( *p && !is_ws( *p ) ) p++;
	}
	if ( n-1>0 ) {
		add_word( s, "LAST", name[n-1], " \t" );
		for (i=0; i<n-1; ++i) add_word( s, "PREF", name[i], " \t" );
	}
}

/* extract_name2()
 *
 * extract names in the format "Author, H.F.", w/comma
 */ 
void
extract_name2( char *p, newstring *s )
{
	while ( is_ws( *p ) ) p++;
	if ( *p && *p!=',' ) p = add_word( s, "LAST", p, "," ); 
	while ( *p==',' || is_ws( *p ) ) p++;
	while ( *p ) {
		while ( is_ws( *p ) ) p++;
		if ( *p ) p = add_word( s, "PREF", p, " \t" );
	}
}

/*
 * send to appropriate algorithm depending on if the author name is
 * in the format:  "H. F. Author" or "Author, H. F."
 */
char * 
extract_name( char *p, newstring *s )
{
	int junior=FALSE, third=FALSE;
	newstring name;
	newstr_init( &name );
	while ( *p && *p!='|' ) {
		newstr_addchar( &name, *p++ );
	}
	if (strsearch(name.data,", Jr.")) {
		newstr_findreplace( &name, ", Jr.", " " );
		junior = TRUE;
	} else if (strsearch(name.data," Jr.")) {
		newstr_findreplace( &name, " Jr.", " " );
		junior=TRUE;
	}
	if (strsearch(name.data," III")) {
		newstr_findreplace( &name, " III", " " );
		third=TRUE;
	}
	if ( name.len ) {
		if ( strchr( name.data, ',' )==NULL ) 
			extract_name1( name.data, s );
		else
			extract_name2( name.data, s );
	}
	if ( *p=='|' ) p++;
	if ( junior ) newstr_strcat( s, "<SUFF>Jr.</SUFF>" );
	if ( third )  newstr_strcat( s, "<SUFF>III</SUFF>" );
	newstr_free( &name );
	return p;
}

void
strip_newlines( newstring *s )
{
	char *p = s->data;
	while ( *p ) {
		if ( *p=='\r' || *p=='\n' ) {
			*p=' ';
		}
		p++;
	}
}

/* extract_tag()
 *
 * assume result already points to an allocated, initialized newstring
 */
char *
extract_tag( char *buffer, newstring *result )
{
	char *p = buffer, *q, *end;
	newstr_empty( result );
	while ( is_ws( *p ) ) p++;
	q = p;
	while ( *q && *q != '=' ) q++;
	/* handle lines like "tag={DATA}," and "tag     ={DATA}," */
	if ( *q=='=' ) {
		end = q+1;
		q--;
		while ( q>=p && is_ws(*q) ) q--;
	} else end = q;
	newstr_segcpy( result, p, q+1 );
	return end;
}

/* extract_data()
 *
 * assume result already points to an allocated, initialized newstring
 */
char *
extract_data( char *buffer, newstring *result )
{
	char *p = buffer, *q, *end;
	int  inquotes = 0, bracketlevel = 0;
	newstr_empty( result );
	while ( is_ws( *p ) ) p++;
	q = p;
	while ( *q && !(*q==',' && !inquotes && bracketlevel<1) ) {
		if ( *q == '{' ) bracketlevel++;
		else if ( *q == '}' ) bracketlevel--;
		else if ( *q == '\"' ) {
			if ( q==p || *(q-1)!='\\')
				inquotes = !inquotes;
		}
		q++;
	}
	/* handle lines like "tag = {DATA}," and "tag = {DATA} ," */
	if ( *q==',' ) end = q+1;
	else end = q;
	q--;
	while ( q>=p && is_ws(*q) ) q--;
	if ( ((*p=='"' && *q=='"') || (*p=='{' && *q=='}') ) && p!=q ) {
		p++; q--;
	}
	newstr_segcpy( result, p, q+1 );
	bibtex_usestrings( result );
	newstr_findreplace( result, "\\it ", "" );
	newstr_findreplace( result, "\\em ", "" );
	newstr_findreplace( result, "\\%", "%" );
	newstr_findreplace( result, "\\$", "$" );
	newstr_findreplace( result, "\\&", "&" );
	newstr_findreplace( result, "{", "" );
	newstr_findreplace( result, "}", "" );
	newstr_findreplace( result, "  ", " " );
	return end;
}

/* process_citetype()
 *
 * output the xml citation type, remove final "}" or ")" from the
 * reference, and return pointer to the internal reference information
 */
char *
process_citetype( fields *info, char *buffer )
{
	char *p;
	int  found_type=-1, i;

	/* add string */
	if ( !strncasecmp( buffer, "@STRING", 7 ) ) {
		p = buffer;
		while ( p && *p && *p!='{' && *p!='(' ) p++;
		bibtex_addstring( p );
		info->type=-1;
		return NULL;
	}

	/* determine citation type */
	for ( i=0; i<ntypes; ++i ) {
		if (strsearch(buffer,bibtex_type[i])!=NULL) found_type=i;
	}

	if ( found_type==-1 ) {
		fprintf(stderr,"bib2xml: defaulting to type ARTICLE.  Cannot identify type for:\n");
		fprintf(stderr,"%s\n",buffer);
		found_type = 0;
	}

	info->type = found_type;

	/* remove terminal bracket */
	p = buffer + strlen(buffer);
	while ( p && p > buffer && *p!='}' && *p!=')' ) p--;
	if ( *p=='}' || *p==')' ) *p='\0';

	/* point to just after bracket */
	p = buffer;	
	while ( p && *p!='{' && *p!='(' ) p++;
	if ( p ) p++;

	return p;
}

void 
process_cite( fields *info, char *buffer )
{
	newstring tag, field;
	char *p;

	info->nfields = 0;

	newstr_init( &tag );
	newstr_init( &field );

	/* Process citation type */
	p = process_citetype( info, buffer );

	/* extract the id key */
	if ( p ) {
		newstr_strcpy( &tag, "REFNUM" );  /* stick on REFNUM tag */
		p = extract_data( p, &field ); /* this is the id */
		fields_add( info, tag.data, field.data );
		while ( p && *p!='\0' ) {
			p = extract_tag( p, &tag );
			p = extract_data( p, &field );
			if ( !strsearch( tag.data, "ABSTRACT" ) &&
			     !strsearch( tag.data, "SUMMARY" ) ) {
				strip_newlines( &field );
			}
			newstr_encodexml( &field );
			fields_add( info, tag.data, field.data );
			while ( is_ws(*p) ) p++;
		}
	}
}

void
output_names( fields *info, FILE *outptr, char *btag, char *xtag, char *plural )
{
	newstring name;
	char      *q;
	int       n = fields_find( info, btag );
	if ( n!=-1 ) {
		fprintf( outptr, "  <%s>\n", plural );
		newstr_findreplace( &(info->data[n]), " and ","|" );
		q = info->data[n].data;
		newstr_init( &name );
		while ( *q ) {
			q = extract_name( q, &name );
			if ( name.len ) {
				fprintf(outptr,"    <%s>%s</%s>\n",
					xtag, name.data, xtag);
				newstr_empty( &name );
			}
		}
		newstr_free( &name );
		fprintf( outptr, "  </%s>\n", plural );
		info->used[n] = 1;
	}
}

void
output_date( fields *info, FILE *outptr )
{
	int year = fields_find( info, "YEAR" );
	int month = fields_find( info, "MONTH" );
	int day = fields_find( info, "DAY" );
	if ( year!=-1 || month!=-1 || day!=-1 ) {
		fprintf(outptr,"  <DATE>");
		if ( year!=-1 ) {
			fprintf( outptr, "<YEAR>%s</YEAR>",
					info->data[year].data );
			info->used[year] = 1;
		}
		if ( month!=-1 ) {
			fprintf( outptr, "<MONTH>%s</MONTH>",
					info->data[month].data );
			info->used[month] = 1;
		}
		if ( day!=-1 ) {
			fprintf( outptr, "<DAY>%s</DAY>",
					info->data[day].data );
			info->used[day] = 1;
		}
		fprintf(outptr,"</DATE>\n");
	}
}


void
output_pages( fields *info, FILE *outptr )
{
	newstring sp, ep;
	int  pages = fields_find( info, "PAGES" );
	if ( pages!=-1 ) {
		newstr_init( &sp );
		newstr_init( &ep );
		info->used[pages] = 1;
		fprintf(outptr,"  <PAGES>");
		extract_pages(info->data[pages].data,&sp,&ep);
		if ( sp.len ) fprintf(outptr,"<START>%s</START>",sp.data);
		if ( ep.len ) fprintf(outptr,"<END>%s</END>",ep.data);
		fprintf(outptr,"</PAGES>\n");
		newstr_free( &sp );
		newstr_free( &ep );
	}
}

void
output_easy( fields *info, FILE *outptr, char *bibtex, char *xml )
{
	char *p;
	int j;
	for ( j=0; j<info->nfields; ++j ) {
		p = strsearch( info->tag[j].data, bibtex );
		if ( p==info->tag[j].data ) {
			fprintf( outptr, "  <%s>%s</%s>\n", xml, 
					info->data[j].data, xml );
			info->used[j] = 1;
		}
	}
}

void
output_unused( fields *info, FILE *outptr )
{
	int unused = 0, i;
	for ( i=0; i<info->nfields; ++i ) {
		if ( info->used[i] ) continue;
		if  ( unused==0 ) fprintf( outptr, "  <UNRECOGNIZED>\n");
		fprintf(outptr,"    <ITEM><TAG>%s</TAG>",info->tag[i].data);
		fprintf(outptr,"<VALUE>%s</VALUE></ITEM>\n",info->data[i].data);
		unused++;
	}
	if ( unused ) fprintf( outptr, "  </UNRECOGNIZED>\n");
}

void
output_cite( fields *info, FILE *outptr )
{
	enum { SIMPLE, PEOPLE, DATE, PAGES };
	typedef struct bibmapping {
		int mode;
		char *bibtex;
		char *xml;
		char *plural;
	} bibmapping;
	bibmapping map[] = {
		{ PEOPLE, "AUTHOR", "AUTHOR", "AUTHORS" },
		{ DATE, "", "", "" },
		{ SIMPLE, "TITLE", "TITLE", "" },
		{ SIMPLE, "JOURNAL", "JOURNAL", "" },
		{ SIMPLE, "VOLUME", "VOLUME", "" },
		{ SIMPLE, "ISSUE", "ISSUE", "" },
		{ PAGES, "", "", "" },
		{ PEOPLE, "EDITOR", "EDITOR", "EDITORS" },
		{ SIMPLE, "PUBLISHER", "PUBLISHER", "" },
		{ SIMPLE, "ADDRESS", "ADDRESS", "" },
		{ SIMPLE, "CHAPTER", "CHAPTER", "" },
		{ SIMPLE, "BOOKTITLE", "BOOKTITLE", "" },
		{ SIMPLE, "ABSTRACT", "ABSTRACT", "" },
		{ SIMPLE, "REFNUM", "REFNUM", "" }
	};
	int i, n = sizeof(map)/sizeof(bibmapping);

	if ( info->type==-1 ) return;
	fprintf( outptr, "<REF>\n" );
	fprintf( outptr, "  <TYPE>%s</TYPE>\n", xml_type[info->type] );
	for ( i=0; i<n; ++i ) {
		switch ( map[i].mode ) {
		case PEOPLE:
			output_names( info, outptr, map[i].bibtex, map[i].xml,
				   map[i].plural );
			break;
		case DATE:
			output_date( info, outptr );
			break;
		case PAGES:
			output_pages( info, outptr );
			break;
		case SIMPLE:
		default:
			output_easy( info, outptr, map[i].bibtex, map[i].xml );
		}
	}

	output_unused( info, outptr );

	fprintf( outptr, "</REF>\n" );
	fflush( outptr );
}


/* read_refs()
 *
 * Reads references one at a time into buffer for processing
 *
 */
long
read_refs( FILE *inptr, FILE *outptr )
{
	newstring buffer, line;
	fields reffields;
	char buf[256]="", /**errorptr,*/   *p;
	int haveref = FALSE, processref = FALSE;
	long numrefs = 0L;
	int bufpos = 0;

	newstr_init( &buffer );
	newstr_init( &line );
	fields_init( &reffields );

	while ( newstr_fget( inptr, buf, sizeof(buf), &bufpos, &line ) ) {

		if ( line.len == 0 ) continue; /* blank line */
		p = &(line.data[0]);
		while (is_ws(*p)) p++;
		if (*p == '@') {
			if (haveref) processref = TRUE;
			else haveref = TRUE;
		}

		if ( processref ) {
			process_cite( &reffields, buffer.data );
			output_cite( &reffields, outptr );
			newstr_empty( &buffer );
			processref = FALSE;
			if ( reffields.type!=-1 ) numrefs++;
		}

		if ( haveref ) {
			newstr_strcat( &buffer, p );
			newstr_addchar( &buffer, '\n' ); 
		}

	}

	if ( haveref ) {
			process_cite( &reffields, buffer.data );
			output_cite( &reffields, outptr );
			newstr_empty( &buffer );
			processref = FALSE;
			if ( reffields.type!=-1 ) numrefs++;
	}

	fields_free ( &reffields );
	newstr_free ( &buffer );
	newstr_free ( &line );
	return numrefs;
}

void
help( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"\n%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	fprintf(stderr,"Converts a Bibtex reference file into XML\n\n");

	fprintf(stderr,"usage: %s bibtex_file > xml_file\n\n",progname);
        fprintf(stderr,"  bibtex_file can be replaced with file list or omitted to use as a filter\n\n");

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
	int i=0,j,subtract;
	while ( i<*argc ) {
		subtract = 0;
		if ( strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0 ) {
			help();
			/* help terminates or we'd remove from argv */
		}
		else if ( strcmp(argv[i],"-v")==0 || strcmp(argv[i],"--version")==0){
			tellversion();
			/* tellversion terminates or we'd remove from argv*/
		}
		else if ( strcmp(argv[i],"--verbose")==0 ) {
			verbose = 1;
			subtract = 1;
		}
		if ( subtract ) {
			for ( j=i+subtract; j<*argc; j++ )
				argv[j-subtract] = argv[j];
			*argc -= subtract;
		} else i++;
	}
}

int 
main(int argc, char *argv[])
{
	FILE *outptr=stdout,*inptr=stdin;
	long numrefs;
	int i;

	process_args( &argc, argv );

	if (argc==1) {
		fprintf(outptr,"<XML>\n");
		fprintf(outptr,"<REFERENCES>\n");
		numrefs = read_refs(inptr,outptr);
		fprintf(outptr,"</REFERENCES>\n");
		fprintf(outptr,"</XML>\n");
	} else {
		fprintf(outptr,"<XML>\n");
		fprintf(outptr,"<REFERENCES>\n");
		numrefs = 0;
		for (i=1; i<argc; ++i) {
			inptr = fopen( argv[i], "r" );
			if (inptr!=NULL) {
				numrefs += read_refs(inptr,outptr);
				fclose(inptr);
			} else {
				fprintf(stderr,"bib2xml: cannot open %s\n",
					argv[i]);
			}
		}
		fprintf(outptr,"</REFERENCES>\n");
		fprintf(outptr,"</XML>\n");
	}

	fprintf(stderr,"bib2xml:  Processed %ld references.\n",numrefs);
	return EXIT_SUCCESS;
}


