/*
 * myxmlout.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newstr.h"
#include "fields.h"


void
output_listheader( FILE *outptr )
{
	fprintf( outptr, "<XML>\n");
	fprintf( outptr, "<REFERENCES>\n");
}

void
output_listfooter( FILE *outptr )
{
	fprintf( outptr, "</REFERENCES>\n");
	fprintf( outptr, "</XML>\n");
}

void
output_type( fields *info, FILE *outptr )
{
	int found = fields_find( info, "TYPE" );
	if ( found!=-1 ) {
		newstr_encodexml( &(info->data[found] ) );
		fprintf( outptr, "    <TYPE>%s</TYPE>\n", 
					info->data[found].data );
		info->used[found] = 1;
	}
}

void
output_person( fields *info, FILE *outptr, int i )
{
	char *p = info->data[i].data;
	fprintf( outptr, "<LAST>" );
	while ( *p && *p!='|' ) fprintf( outptr, "%c", *p++ );
	fprintf( outptr, "</LAST>" );
	while ( *p ) {
		if ( *p=='|' ) p++;
		if ( !(*p) ) continue;
		fprintf( outptr, "<PREF>" );
		while ( *p && *p!='|' ) fprintf( outptr, "%c", *p++ );
		fprintf( outptr, "</PREF>" );
	}
	info->used[i] = 1;
}

void
output_people( fields *info, FILE *outptr, char *multi, char *single )
{
	int i, npeople=0;
	for ( i=0; i<info->nfields; ++i ) {
		if ( !strcasecmp( info->tag[i].data, single ) ) {
			if ( npeople==0 ) fprintf( outptr,"    <%s>\n",
					multi );
			fprintf( outptr, "       <%s>", single );
			output_person( info, outptr, i );
			fprintf( outptr, "</%s>\n", single );
			npeople++;
		}
	}
	if ( npeople>0 ) fprintf( outptr,"    </%s>\n", multi );
}

void
output_title( fields *info, FILE *outptr )
{
	int  found_title, found_subtitle;
	found_title = fields_find( info, "TITLE" );
	found_subtitle = fields_find( info, "SUBTITLE" );
	if ( found_title!=-1 || found_subtitle!=-1) {
		fprintf( outptr, "    <TITLE>");
		if ( found_title!=-1 ) {
			newstr_encodexml( &(info->data[found_title] ) );
			fprintf( outptr, "%s", info->data[found_title].data );
			info->used[found_title] = 1;
		}
		if ( found_subtitle!=-1 ) {
			newstr_encodexml( &(info->data[found_subtitle] ) );
			if ( found_title!=-1 ) fprintf( outptr, ": " );
			fprintf( outptr, "%s", info->data[found_subtitle].data );
			info->used[found_subtitle] = 1;
		}
		fprintf( outptr, "</TITLE>\n");
	}
}

void
output_source( fields *info, FILE *outptr )
{
	char   *internal_names[] = { "JOURNAL", "BOOKTITLE", "SERIESTITLE",
		"VOLUME", "ISSUE", "SECTION", "NUMBER", 
		"EDITION", "PUBLISHER", "ADDRESS", "REPRINTSTATUS" };
	char   *xml_type[] = { "JOURNAL", "BOOKTITLE", "SERIESTITLE",
		"VOLUME", "ISSUE", "SECTION", "NUMBER", 
		"EDITION", "PUBLISHER", "ADDRESS", "REPRINTSTATUS" };
	int    n, ntypes = sizeof( internal_names ) /sizeof( char * );
	int    found;
	for ( n=0; n<ntypes; ++n ) {
		found = fields_find( info, internal_names[n] );
		if ( found==-1 ) continue;
		newstr_encodexml( &(info->data[found] ) );
		fprintf( outptr, "    <%s>%s</%s>\n",
			xml_type[n], info->data[found].data, xml_type[n] );
		info->used[found] = 1;
	}
}

void
output_date( fields *info, FILE *outptr )
{
	int found_year = fields_find( info, "YEAR" );
	int found_month = fields_find( info, "MONTH" );
	int found_day = fields_find ( info, "DAY" );
	if ( found_year!=-1 || found_month!=-1 || found_day!=-1 ) {
		fprintf( outptr, "    <DATE>" );
		if ( found_year!=-1 ) {
			newstr_encodexml( &(info->data[found_year] ) );
			fprintf( outptr, "<YEAR>%s</YEAR>",
					info->data[found_year].data );
			info->used[found_year] = 1;
		}
		if ( found_month!=-1 ) {
			newstr_encodexml( &(info->data[found_month] ) );
			fprintf( outptr, "<MONTH>%s</MONTH>",
					info->data[found_month].data );
			info->used[found_month] = 1;
		}
		if ( found_day!=-1 ) {
			newstr_encodexml( &(info->data[found_day] ) );
			fprintf( outptr, "<DAY>%s</DAY>",
					info->data[found_day].data );
			info->used[found_day] = 1;
		}
		fprintf( outptr, "</DATE>\n" );
	}
}

void
output_sn( fields *info, FILE *outptr )
{
	char    *internal_names[] = { "ISBN", "ISSN", "REFNUM" };
	char    *xml_type[] = { "SERIALNUM", "SERIALNUM", "REFNUM" };
	int     n, ntypes = sizeof( internal_names ) / sizeof( char* );
	int     found;

	for ( n=0; n<ntypes; ++n ) {
		found = fields_find( info, internal_names[n] );
		if ( found==-1 ) continue;
		newstr_encodexml( &(info->data[found] ) );
		fprintf( outptr, "    <%s>%s</%s>\n",
			xml_type[n], info->data[found].data, xml_type[n] );
		info->used[found] = 1;
	}
}

void
output_abs( fields *info, FILE *outptr )
{
	int found;
	found = fields_find( info, "ABSTRACT" );
	if ( found!=-1 ) {
		newstr_encodexml( &(info->data[found] ) );
		fprintf( outptr, "    <ABSTRACT>%s</ABSTRACT>\n",
				info->data[found].data );
		info->used[found] = 1;
	}
	found = fields_find( info, "NOTES" );
	if ( found!=-1 ) {
		newstr_encodexml( &(info->data[found] ) );
		fprintf( outptr, "    <NOTES>%s</NOTES>\n",
				info->data[found].data );
		info->used[found] = 1;
	}
}

void
output_pages( fields *info, FILE *outptr )
{
	int found_sp, found_ep;
	found_sp = fields_find( info, "PAGESTART" );
	found_ep = fields_find( info, "PAGEEND" );
	if ( found_sp!=-1 || found_ep!=-1 ) {
		fprintf( outptr, "    <PAGES>" );
		if ( found_sp!=-1 ) {
			newstr_encodexml( &(info->data[found_sp] ) );
			fprintf( outptr, "<START>%s</START>",
					info->data[found_sp].data );
			info->used[found_sp] = 1;
		}
		if ( found_ep!=-1 ) {
			newstr_encodexml( &(info->data[found_ep] ) );
			fprintf( outptr, "<END>%s</END>", 
					info->data[found_ep].data );
			info->used[found_ep] = 1;
		}
		fprintf( outptr, "</PAGES>\n" );
	}
}

void
output_key( fields *info, FILE *outptr )
{
	int i, nkeys = 0;
	for ( i=0; i<info->nfields; ++i ) {
		if ( !strcasecmp( info->tag[i].data, "KEYWORD" ) ) {
			if ( nkeys==0 ) fprintf( outptr,"    <KEYWORDS>\n");
			newstr_encodexml( &(info->data[i] ) );
			fprintf( outptr, "      <KEYWORD>%s</KEYWORD>\n",
			       info->data[i].data );
			info->used[i] = 1;
			nkeys++;
		}
	}
	if ( nkeys!=0 ) fprintf( outptr,"    </KEYWORDS>\n");
}

void
output_url( fields *info, FILE *outptr )
{
	int found = fields_find( info, "URL" );
	if ( found!=-1 ) {
		newstr_encodexml( &(info->data[found] ) );
		fprintf(outptr, "    <URL>%s</URL>\n",info->data[found].data);
		info->used[found] = 1;
	}
}



void
output_cite( fields *info, FILE *outptr )
{
	int i;
	fprintf( outptr, "  <REF>\n");
	output_type( info, outptr );
	output_people( info, outptr, "AUTHORS", "AUTHOR" );
	output_people( info, outptr, "SERIESAUTHOR", "AUTHOR" );
	output_title( info, outptr );
	output_source( info, outptr );
	output_date( info, outptr );
	output_pages( info, outptr );
	output_people( info, outptr, "EDITORS", "EDITOR" );
	output_key( info, outptr );
	output_abs( info, outptr );
	output_url( info, outptr );
	output_sn( info, outptr );
	for ( i=0; i<info->nfields; ++i ) {
		if ( !info->used[i] ) {
			fprintf(stderr, "Warning: unused tag: '%s' value: '%s'\n", info->tag[i].data, info->data[i].data );
		}
	}
	fprintf( outptr, "  </REF>\n");
	fflush( outptr );
}

