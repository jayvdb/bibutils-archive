/*
 * bibtexout.c
 *
 * Copyright (c) Chris Putnam 2003-5
 *
 * Program and source code released under the GPL
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "newstr.h"
#include "strsearch.h"
#include "xml.h"
#include "fields.h"
#include "bibl.h"
#include "bibtexout.h"

enum {
	TYPE_UNKNOWN = 0,
	TYPE_ARTICLE,
	TYPE_INBOOK,
	TYPE_INPROCEEDINGS,
	TYPE_PROCEEDINGS,
	TYPE_INCOLLECTION,
	TYPE_COLLECTION,
	TYPE_BOOK,
	TYPE_PHDTHESIS,
	TYPE_MASTERSTHESIS,
	TYPE_REPORT,
	TYPE_MANUAL,
	TYPE_UNPUBLISHED,
	TYPE_MISC
};

static void
output_citekeysafe( FILE *fp, char *q1, char *q2 )
{
	char *p;
	int i, n = 0;
	for ( i=0; i<2; ++i ) {
		if ( i==0 ) p = q1;
		else p = q2;
		while ( p && *p && *p!='|' ) {
			if ( *p!=' ' && *p!='\t' ) {
/*			if ( ( *p>='A' && *p<='Z' ) || 
			     ( *p>='a' && *p<='z' ) ||
			     ( *p>='0' && *p<='9' ) ) {*/
					if ( n==0 && (*p>='0' && *p<='9') ) 
						fprintf(fp,"ref");
					fprintf(fp,"%c",*p);
					n++;
			}
			p++;
		}
	}
}

static void
output_citekey( FILE *fp, fields *info, unsigned long refnum )
{
	int n = fields_find( info, "REFNUM", -1 ), n1, n2;
	if ( n!=-1 ) output_citekeysafe( fp, info->data[n].data, NULL );
	else {
		n1 = fields_find( info, "AUTHOR", -1 );
		n2 = fields_find( info, "YEAR", -1 );
		if ( n1!=-1 && n2!=-1 ) {
			output_citekeysafe( fp, info->data[n1].data,
				       info->data[n2].data );
		} else fprintf( fp, "ref%ld", refnum+1 );
	}
}

static int
bibtexout_type( fields *info, char *filename, int refnum )
{
	char *genre;
	int type = TYPE_UNKNOWN, i, maxlevel, n, level;

	/* determine bibliography type */
	for ( i=0; i<info->nfields; ++i ) {
		if ( strcasecmp( info->tag[i].data, "GENRE" ) &&
		     strcasecmp( info->tag[i].data, "NGENRE" ) ) continue;
		genre = info->data[i].data;
		level = info->level[i];
		if ( !strcasecmp( genre, "periodical" ) ||
		     !strcasecmp( genre, "academic journal" ) ||
		     !strcasecmp( genre, "magazine" ) )
			type = TYPE_ARTICLE;
		else if ( !strcasecmp( genre, "instruction" ) )
			type = TYPE_MANUAL;
		else if ( !strcasecmp( genre, "unpublished" ) )
			type = TYPE_UNPUBLISHED;
		else if ( !strcasecmp( genre, "conference publication" ) ) {
			if ( level==0 ) type=TYPE_PROCEEDINGS;
			else type = TYPE_INPROCEEDINGS;
		} else if ( !strcasecmp( genre, "collection" ) ) {
			if ( level==0 ) type=TYPE_COLLECTION;
			else type = TYPE_INCOLLECTION;
		} else if ( !strcasecmp( genre, "report" ) )
			type = TYPE_REPORT;
		else if ( !strcasecmp( genre, "book" ) ) {
			if ( level==0 ) type=TYPE_BOOK;
			else type=TYPE_INBOOK;
		} else if ( !strcasecmp( genre, "theses" ) ) {
			if ( type==TYPE_UNKNOWN ) type=TYPE_PHDTHESIS;
		} else if ( !strcasecmp( genre, "Ph.D. thesis" ) )
			type = TYPE_PHDTHESIS;
		else if ( !strcasecmp( genre, "Masters thesis" ) )
			type = TYPE_MASTERSTHESIS;
	}
	if ( type==TYPE_UNKNOWN ) {
		for ( i=0; i<info->nfields; ++i ) {
			if ( strcasecmp( info->tag[i].data, "ISSUANCE" ) ) continue;
			if ( !strcasecmp( info->data[i].data, "monographic" ) ) {
				if ( info->level[i]==0 ) type = TYPE_BOOK;
				else if ( info->level[i]==1 ) type=TYPE_INBOOK;
			}
		}
	}

	/* default to BOOK type */
	if ( type==TYPE_UNKNOWN ) {
		maxlevel = fields_maxlevel( info );
		if ( maxlevel > 0 ) type = TYPE_INBOOK;
		else {
			fprintf( stderr, "xml2bib: cannot identify TYPE" 
				" in reference %d",refnum+1 );
			n = fields_find( info, "REFNUM", -1 );
			if ( n!=-1 ) 
				fprintf( stderr, " %s", info->data[n].data);
			fprintf( stderr, "\n" );
			type = TYPE_MISC;
		}
	}
	return type;
}

static void
output_type( FILE *fp, int type )
{
	switch ( type ) {
		case TYPE_ARTICLE:
			fprintf( fp, "@ARTICLE{" );
			break;
		case TYPE_INBOOK:
			fprintf( fp, "@INBOOK{" );
			break;
		case TYPE_PROCEEDINGS:
			fprintf( fp, "@PROCEEDINGS{" );
			break;
		case TYPE_INPROCEEDINGS:
			fprintf( fp, "@INPROCEEDINGS{" );
			break;
		case TYPE_BOOK:
			fprintf( fp, "@BOOK{" );
			break;
		case TYPE_PHDTHESIS:
			fprintf( fp, "@PHDTHESIS{" );
			break;
		case TYPE_MASTERSTHESIS:
			fprintf( fp, "@MASTERSTHESIS{" );
			break;
		case TYPE_REPORT:
			fprintf( fp, "@TECHREPORT{" );
			break;
		case TYPE_MANUAL:
			fprintf( fp, "@MANUAL{" );
			break;
		case TYPE_COLLECTION:
			fprintf( fp, "@COLLECTION{" );
			break;
		case TYPE_INCOLLECTION:
			fprintf( fp, "@INCOLLECTION{" );
			break;
		case TYPE_UNPUBLISHED:
			fprintf( fp, "@UNPUBLISHED{" );
			break;
		case TYPE_MISC:
			fprintf( fp, "@MISC{" );
			break;
	}
}

static void
output_element( FILE *fp, char *tag, char *data, int format_opts )
{
	int i, len, nquotes = 0;
	char ch;
	if ( format_opts & BIBOUT_WHITESPACE ) 
		fprintf( fp, ",\n  %s = \t", tag );
	else
		fprintf( fp, ",\n%s=", tag );

	if ( format_opts & BIBOUT_BRACKETS ) fprintf( fp, "{" );
	else fprintf( fp, "\"" );

	len = strlen( data );
	for ( i=0; i<len; ++i ) {
		ch = data[i];
		if ( ch!='\"' ) fprintf( fp, "%c", ch );
		else {
			if ( format_opts & BIBOUT_BRACKETS || 
			    ( i>0 && data[i-1]=='\\' ) )
				fprintf( fp, "\"" );
			else {
				if ( nquotes % 2 == 0 )
					fprintf( fp, "``" );
				else    fprintf( fp, "\'\'" );
				nquotes++;
			}
		}
	}

	if ( format_opts & BIBOUT_BRACKETS ) fprintf( fp, "}" );
	else fprintf( fp, "\"" );
}

static void
add_person( newstr *s, char *p )
{
	int nseps = 0, nch;
	while ( *p ) {
		nch = 0;
		if ( nseps ) newstr_addchar( s, ' ' );
		while ( *p && *p!='|' ) {
			newstr_addchar( s, *p++ );
			nch++;
		}
		if ( *p=='|' ) p++;
		if ( nseps==0 ) newstr_addchar( s, ',' );
		else if ( nch==1 ) newstr_addchar( s, '.' );
		nseps++;

	}
}

static void
output_people( FILE *fp, fields *info, unsigned long refnum, char *tag, 
		char *ctag, char *bibtag, int level, int format_opts )
{
	newstr allpeople;
	int i, npeople, person, corp;

	/* primary citation authors */
	npeople = 0;
	for ( i=0; i<info->nfields; ++i ) {
		if ( level!=-1 && info->level[i]!=level ) continue;
		person = ( strcasecmp( info->tag[i].data, tag ) == 0 );
		corp   = ( strcasecmp( info->tag[i].data, ctag ) == 0 );
		if ( person || corp ) {
			if ( npeople==0 ) newstr_init( &allpeople );
			else {
				if ( format_opts & BIBOUT_WHITESPACE )
					newstr_strcat(&allpeople,"\n\t\tand ");
				else newstr_strcat( &allpeople, "\nand " );
			}
			if ( corp ) {
				newstr_addchar( &allpeople, '{' );
				newstr_strcat( &allpeople, info->data[i].data );
				newstr_addchar( &allpeople, '}' );
			} else add_person( &allpeople, info->data[i].data ); 
			npeople++;
		}
	}
	if ( npeople ) {
		output_element( fp, bibtag, allpeople.data, format_opts );
		newstr_free( &allpeople );
	}
}

static void
output_title( FILE *fp, fields *info, unsigned long refnum, char *bibtag, int level, int format_opts )
{
	newstr title;
	int n1 = fields_find( info, "TITLE", level );
	int n2 = fields_find( info, "SUBTITLE", level );
	if ( n1!=-1 ) {
		newstr_init( &title );
		newstr_strcpy( &title, info->data[n1].data );
		if ( n2!=-1 ) {
			if ( info->data[n1].data[info->data[n1].len]!='?' )
				newstr_strcat( &title, ": " );
			else newstr_addchar( &title, ' ' );
			newstr_strcat( &title, info->data[n2].data );
		}
		output_element( fp, bibtag, title.data, format_opts );
		newstr_free( &title );
	}
}

static void
output_date( FILE *fp, fields *info, unsigned long refnum, int format_opts )
{
	char *months[12] = { "January", "February", "March", "April",
		"May", "June", "July", "August", "September", "October",
		"November", "December" };
	int n, month;
	n = fields_find( info, "YEAR", -1 );
	if ( n==-1 ) n = fields_find( info, "PARTYEAR", -1 );
	if ( n!=-1 ) output_element( fp, "YEAR", info->data[n].data, format_opts );
	n = fields_find( info, "MONTH", -1 );
	if ( n==-1 ) n = fields_find( info, "PARTMONTH", -1 );
	if ( n!=-1 ) {
		month = atoi( info->data[n].data );
		if ( month>0 && month<13 )
			output_element( fp, "MONTH", months[month-1], format_opts );
		else
			output_element( fp, "MONTH", info->data[n].data, format_opts );
	}
	n = fields_find( info, "DAY", -1 );
	if ( n==-1 ) n = fields_find( info, "PARTDAY", -1 );
	if ( n!=-1 ) output_element( fp, "DAY", info->data[n].data, format_opts );
}

static void
output_pages( FILE *fp, fields *info, unsigned long refnum, int format_opts )
{
	newstr pages;
	int sn, en;
	sn = fields_find( info, "PAGESTART", -1 );
	en = fields_find( info, "PAGEEND", -1 );
	if ( sn==-1 && en==-1 ) return;
	newstr_init( &pages );
	if ( sn!=-1 ) newstr_strcat( &pages, info->data[sn].data );
	if ( sn!=-1 && en!=-1 ) {
		if ( format_opts & BIBOUT_SINGLEDASH ) 
			newstr_strcat( &pages, "-" );
		else
			newstr_strcat( &pages, "--" );
	}
	if ( en!=-1 ) newstr_strcat( &pages, info->data[en].data );
	output_element( fp, "PAGES", pages.data, format_opts );
	newstr_free( &pages );
}

static void
output_simple( FILE *fp, fields *info, char *intag, char *outtag, 
		int format_opts )
{
	int n = fields_find( info, intag, -1 );
	if ( n!=-1 )
		output_element( fp, outtag, info->data[n].data, format_opts );
}

static void
output_simpleall( FILE *fp, fields *info, char *intag, char *outtag,
		int format_opts )
{
	int i;
	for ( i=0; i<info->nfields; ++i ) {
		if ( strcasecmp( info->tag[i].data, intag ) ) continue;
		output_element( fp, outtag, info->data[i].data, format_opts );
	}
}

void
bibtexout_write( fields *info, FILE *fp, int format_opts, unsigned long refnum )
{
	int type;
	type = bibtexout_type( info, "", refnum );
	output_type( fp, type );
	output_citekey( fp, info, refnum );
	output_people( fp, info, refnum, "AUTHOR", "CORPAUTHOR", "AUTHOR", 0,
		format_opts );
	output_people( fp, info, refnum, "EDITOR", "CORPEDITOR", "EDITOR", -1,
		format_opts );

	/* item=main level title */
	if ( type==TYPE_INBOOK )
		output_title( fp, info, refnum, "CHAPTER", 0, format_opts );
	else
		output_title( fp, info, refnum, "TITLE", 0, format_opts );

	/* item=host level title */
	if ( type==TYPE_ARTICLE )
		output_title( fp, info, refnum, "JOURNAL", 1, format_opts );
	else if ( type==TYPE_INBOOK )
		output_title( fp, info, refnum, "TITLE", 1, format_opts );
	else if ( type==TYPE_INPROCEEDINGS || type==TYPE_INCOLLECTION )
		output_title( fp, info, refnum, "BOOKTITLE", 1, format_opts );
	else if ( type==TYPE_BOOK || type==TYPE_COLLECTION || type==TYPE_PROCEEDINGS )
		output_title( fp, info, refnum, "SERIES", 1, format_opts );

	output_date( fp, info, refnum, format_opts );
	output_simple( fp, info, "EDITION", "EDITION", format_opts );
	output_simple( fp, info, "PUBLISHER", "PUBLISHER", format_opts );
	output_simple( fp, info, "ADDRESS", "ADDRESS", format_opts );
	output_simple( fp, info, "VOLUME", "VOLUME", format_opts );
	output_simple( fp, info, "ISSUE", "ISSUE", format_opts );
	output_simple( fp, info, "NUMBER", "NUMBER", format_opts );
	output_pages( fp, info, refnum, format_opts );
	output_simpleall( fp, info, "KEYWORD", "KEYWORDS", format_opts );
	output_simple( fp, info, "CONTENTS", "CONTENTS", format_opts );
	output_simple( fp, info, "ABSTRACT", "ABSTRACT", format_opts );
	output_simple( fp, info, "LOCATION", "LOCATION", format_opts );
	output_simple( fp, info, "SCHOOL", "SCHOOL", format_opts );
	output_simpleall( fp, info, "NOTES", "NOTE", format_opts );
	output_simple( fp, info, "ISBN", "ISBN", format_opts );
	output_simple( fp, info, "ISSN", "ISSN", format_opts );
	output_simple( fp, info, "DOI", "DOI", format_opts );
	output_simpleall( fp, info, "URL", "URL", format_opts );
	if ( format_opts & BIBOUT_FINALCOMMA ) fprintf( fp, "," );
	fprintf( fp, "\n}\n\n" );
	fflush( fp );
}

