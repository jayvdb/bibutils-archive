#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newstr.h"
#include "fields.h"
#include "is_ws.h"

#define TRUE (1)
#define FALSE (0)

char progname[] = "end2xml";
char version[] = "1.3 4/26/04";

int verbose = 0;

/* name is in format Lastname,Firstname(or inits),Suffix */
void
outputname( char *name )
{
	char *p = name;
	if ( p == NULL ) return;
	printf("<LAST>");
	while ( *p && *p!=',') printf("%c",*p++);
	printf("</LAST>");
	if ( *p == 0 ) return;
	while ( *p==',' || *p==' ' ) p++;
	while ( *p && *p!=',') {
		printf("<PREF>");
		while ( *p && *p!=' ' && *p!=',') printf("%c.",*p++);
		while ( *p==' ' ) p++;
		printf("</PREF>");
	}
	while ( *p==',' || *p==' ' ) p++;
	if ( *p ) {
		printf("<SUFF>");
		while ( *p && *p!=' ' && *p!=',') printf("%c",*p++);
		while ( *p==' ' ) p++;
		printf("</SUFF>");
	}
}

/* potentially multiple names separated by semicolons */
void
outputnames( char *tag, char *value )
{
	newstring name;
	int i, len_value;

	newstr_init( &name );
	len_value = strlen( value );
	for ( i=0; i<len_value; ++i ) {
		if ( value[i]==';' ) {
			if (name.data) {
				printf("        <%s>",tag); 
				outputname( name.data );
				printf("</%s>\n",tag);
				name.data[0]='\0';
			}
		} else {
			if ( name.data==NULL || strlen(name.data)>0 || 
			     (value[i]!=' ' && value[i]!='\t'))
				newstr_addchar( &name, value[i] );
		}
	}
	printf("        <%s>",tag); 
	outputname( name.data );
	printf("</%s>\n",tag);
	newstr_free( &name );
}

int
extract_name2(char **buffer, newstring **s)
{
	newstring prename,*postname;
	char      *p;
	int       ok = 0;

	postname = (newstring *) malloc (sizeof(newstring));
	if ( postname==NULL ) {
		fprintf( stderr,"%s: cannot allocate in extract_name2()\n",
			progname);
		return 0;
	}
	newstr_init(postname);
	newstr_init(&prename);

	if (buffer!=NULL && strlen(*buffer)>0) {
		p = *buffer;
		ok = 1;

		/* skip past whitespace */
		while (*p && *p!='|' && (*p==' ' || *p=='\t')) p++;

		/* get last name */
		while (*p && *p!='|' && *p!=',') {
			newstr_addchar(postname,*p);
			p++;
		}

		/* skip past comma and whitespace */
		if ( *p==',' ) p++;
		while (*p && *p!='|' && (*p==' ' || *p=='\t')) p++;

		/* add each part of the given name */
		newstr_empty( &prename );
		while (*p && *p!='|') {
			if (*p!=' ' && *p!='\t')
				newstr_addchar( &prename, *p );
			else if (*(p+1)!='|' && *(p+1)!='\0')
				newstr_addchar( &prename, ' ' );
			p++;
		}
		if ( prename.len > 0 ) {
			newstr_addchar( postname, '|' );
			newstr_strcat ( postname, prename.data );
		}
		if (*p=='|') p++;
	}
	*s = postname;
	*buffer = p;
	return ok;
}


typedef struct {
	char *old;
	char *new;
} lookups;

void
addtype( fields *info, char *data )
{
	lookups types[] = {
		{ "GENERIC", "ARTICLE" },
		{ "BOOK", "BOOK" }, 
		{ "MANUSCRIPT", "MANUSCRIPT" },
		{ "CONFERENCE PROCEEDINGS", "INPROCEEDINGS"},
		{ "REPORT", "REPORT" },
		{ "COMPUTER PROGRAM", "BOOK" },
		{ "AUDIOVISUAL MATERIAL", "AUDIOVISUAL" },
		{ "ARTWORK", "BOOK" },
		{ "PATENT", "BOOK" },
		{ "BILL", "BILL" },
		{ "CASE", "CASE" },
		{ "JOURNAL ARTICLE", "ARTICLE" }, 
		{ "BOOK SECTION", "INBOOK" },
		{ "EDITED BOOK", "BOOK" },
       		{ "NEWSPAPER ARTICLE",  "ARTICLE" },
		{ "THESIS", "PHDTHESIS" },
		{ "PERSONAL COMMUNICATION", "COMMUNICATION" },
		{ "ELECTRONIC SOURCE", "ARTICLE" },
		{ "FILM OR BROADCAST", "AUDIOVISUAL" },
		{ "MAP", "BOOK" },
		{ "HEARING", "HEARING" },
		{ "STATUTE", "STATUTE" }
	};
	int  ntypes = sizeof( types ) / sizeof( lookups );
	int  i, found=0;
	for ( i=0; i<ntypes; ++i ) {
		if ( !strcasecmp( types[i].old, data ) ) {
			found = 1;
			fields_add( info, "TYPE", types[i].new );
		}
	}
	if ( !found ) {
		fprintf( stderr, "Did not identify RIS reference type '%s'\n",
			data );
		fprintf( stderr, "Defaulting to journal article type\n");
		fields_add( info, "TYPE", types[0].new );
	}
}

void
addname( fields *info, char *tag, char *data )
{
	newstring *s;
	char *p = data;
	if ( extract_name2( &p, &s ) ) {
		fields_add( info, tag, s->data );
		newstr_free( s );
		free( s );
	}
}

void
addpage( fields *info, char *p )
{
	newstring page;
	newstr_init( &page );
	while ( *p && is_ws(*p) ) p++;
	while ( *p && !is_ws(*p) && *p!='-' && *p!='\r' && *p!='\n' ) 
		newstr_addchar( &page, *p++ );
	if ( page.len>0 ) fields_add( info, "PAGESTART", page.data );
	newstr_empty( &page );
	while ( *p && (is_ws(*p) || *p=='-' ) ) p++;
	while ( *p && !is_ws(*p) && *p!='-' && *p!='\r' && *p!='\n' ) 
		newstr_addchar( &page, *p++ );
	if ( page.len>0 ) fields_add( info, "PAGEEND", page.data );
	newstr_free( &page );
}

void
adddate( fields *info, char *tag, char *p )
{
	newstring date;
	newstr_init( &date );
	if ( !strcasecmp( tag, "%D" ) ) {
		while ( *p ) newstr_addchar( &date, *p++ );
		if ( date.len>0 ) fields_add( info, "YEAR", date.data );
	} else if ( !strcasecmp( tag, "%8" ) ) {
		while ( *p && *p!=' ' ) newstr_addchar( &date, *p++ );
		if ( date.len>0 ) fields_add( info, "MONTH", date.data );
		newstr_empty( &date );
		while ( is_ws( *p ) ) p++;
		while ( *p && *p!=' ' ) newstr_addchar( &date, *p++ );
		if ( date.len>0 ) fields_add( info,  "DAY", date.data );
	}
	newstr_free( &date );
}

void
addsn( fields *info, char *buf )
{
	int ndigits=0, issn=0, isbn=0;
	char *p = buf;

	if ( !strncasecmp( p, "ISSN", 4 ) ) issn=1;
	else if ( !strncasecmp( p, "ISBN", 4 ) ) isbn=1;

	if ( !issn && !isbn ) {
		while ( *p ) {
			if ( ( *p>='0' && *p<='9' ) || *p=='x' || *p=='X' ) 
				ndigits++;
			p++;
		}
		if ( ndigits==8 ) issn = 1;
		else if ( ndigits==10 || ndigits==13 ) isbn = 1;
	}
	
	if ( issn ) fields_add( info, "ISSN", buf );
	else if ( isbn ) fields_add( info, "ISBN", buf );
	else fields_add( info, "SERIALNUMBER", buf );
}

void
addtag( fields *info, char *buf )
{
	lookups tags[] = {
		{ "%0", "TYPE" },                       /* 0 more process   */
		{ "%A", "AUTHOR" },                     /* 1-2 more process   */
		{ "%Y", "AUTHOR" },
		{ "%E", "EDITOR" },                     /* 3 more process   */
		{ "%D", "YEAR" },                       /* 4 more process   */
		{ "%8", "MONTHDATE" },                  /* 5 more process   */
		{ "%P", "PAGE" },                       /* 6 more process   */
		{ "%@", "SERIALNUM" },                  /* 7 more process   */
		{ "%9", "GENRE" },
		{ "%T", "TITLE" },
		{ "%B", "SUBTITLE" },
		{ "%S", "BOOKTITLE" },
		{ "%C", "ADDRESS" },
		{ "%F", "REFNUM" },
		{ "%U", "URL" },
		{ "%J", "JOURNAL" },
		{ "%V", "VOLUME" },
		{ "%7", "EDITION" },
		{ "%X", "ABSTRACT" },
		{ "%Z", "ABSTRACT" }, /* notes */
		{ "%N", "NUMBER" },
		{ "%K", "KEYWORD" }, 
		{ "%I", "PUBLISHER" }, 
		{ "%!", "SHORTTITLE" },
		{ "%(", "ORIGINALPUB" },
		{ "%)", "REPRINTEDITION" },
		{ "%*", "REVIEWEDITEM" },
		{ "%+", "AUTHORADDRESS" },
		{ "%L", "CALLNUMBER" },
		{ "%O", "NOTES" },
		{ "%&", "SECTION" }
	};
	int  ntags = sizeof(tags)/sizeof(lookups);
	char *customs[] = { "%1", "%2", "%3", "%4", "%#", "%$" };
	int  ncustoms = sizeof( customs ) / sizeof(char*);
	char *p;
	int  i, len, found=0;

	newstring data;

	newstr_init( &data );

	p = buf;
	i = 0;
	while ( i<3 && *p ) { p++; i++; }; /* skip tag */
	if ( !*p  ) return; 

	while ( *p && ( *p==' ' || *p=='\t' ) ) p++;
	while ( *p && *p!='\r' && *p!='\n' ) {
		/* fix weird mac quotes */
		if ( *p==-45 || *p==-46 ) *p='\"';
		else if ( *p==-43 ) *p='\'';
		newstr_addchar( &data, *p );
		p++;
	}

	/* trim ending ws */
	while ( data.len > 0 && ( data.data[data.len-1]==' ' ||
				  data.data[data.len-1]=='\t' ) ) {
		data.data[data.len-1] = '\0';
		data.len--;
	}

	/* don't add empty strings */
	if ( data.len==0 ) {
		newstr_free( &data );
		return;
	}

	for ( i=0; i<ntags && !found; ++i ) {
		if ( !strncmp( buf, tags[i].old, 2 ) ) {
			found = 1;
			if ( i==0 ) addtype( info, data.data );
			else if ( i>0 && i<4 ) addname(info, tags[i].new, data.data);
			else if ( i==4 || i==5 ) adddate( info, tags[i].old, data.data );
			else if ( i==6 ) addpage( info, data.data );
			else if ( i==7 ) addsn( info, data.data );
			else  fields_add( info, tags[i].new, data.data );
		}
	}

	if ( !found ) {
		for ( i=0; i<ncustoms && !found; ++i ) {
			if ( !strncmp( buf, customs[i], 2 ) ) {
				len = strlen( buf );
				fprintf( stderr, "Did not process 'custom' tag '" );
				for ( i=0; i<2; ++i ) 
					fprintf(stderr,"%c",buf[i]);
				fprintf( stderr, "' with value '");
				for ( i=3; i<len; ++i ) 
					fprintf(stderr,"%c",buf[i]);
				fprintf( stderr, "'\n");
				found=1;
			}
		}
	}

	if ( !found ) {
		len = strlen( buf );
		fprintf( stderr, "Did not identify tag '" );
		for ( i=0; i<2; ++i ) fprintf(stderr,"%c",buf[i]);
		fprintf( stderr, "' with value '");
		for ( i=3; i<len; ++i ) fprintf(stderr,"%c",buf[i]);
		fprintf( stderr, "'\n");
	}

	newstr_free( &data );
}

void
verbose_fieldsin( fields *info )
{
	int i;
	fprintf( stderr,"======== processed\n");
	for ( i=0; i<info->nfields; ++i ) {
		fprintf( stderr, "'%s'='%s'\n", info->tag[i].data,
			info->data[i].data );
	}
}

/* Endnote tag definition:
    character 1 = '%'
    character 2 = alphabetic character or digit
    character 3 = space (ansi 32)
*/
int
is_tag( char *buf )
{
	if ( !buf || buf[0]!='%' ) return  FALSE;
	if (! (((buf[1]>='A' && buf[1]<='Z'))||(buf[1]>='0'&&buf[1]<='9')||
	        (buf[1]>='a' && buf[1]<='z')|| (buf[1]=='?'||buf[1]=='@'||buf[1]=='!' || buf[1]=='#' || buf[1]=='$' || buf[1]=='&' || buf[1]=='(' || buf[1]==')' || buf[1]=='*' || buf[1]=='+')) ) 
		return FALSE;
	if (buf[2]!=' ') return FALSE;
	return TRUE;
}

long 
get_refs( FILE *fp, char *filename )
{
	extern void output_cite( fields *info, FILE *outptr );
	newstring line;
	fields reffields;
	char buf[1024]="";
	long numrefs = 0L;
	int inref = FALSE, bufpos = 0;
	char *p;

	newstr_init( &line );
	fields_init( &reffields );

	while ( !feof( fp ) ) {

		if ( !newstr_fget( fp, buf, sizeof(buf), &bufpos, &line ) )
			inref = FALSE;  /* end of file separates */

		p = &( line.data[0] );

		if ( !p || *p=='\0' ) inref = FALSE; /* blank line separates */

		if ( inref && strncmp(p,"%0 ",3)==0 ) {
			fprintf( stderr, "Warning.  Reference %s %ld not "
					"properly terminated.\n",
					filename, numrefs+1);
			inref = FALSE;
		}
	
		if ( !inref && reffields.nfields ) {
			if ( verbose ) verbose_fieldsin( &reffields );
			output_cite( &reffields, stdout );
			reffields.nfields = 0;
			numrefs++;
		}

		/* Each reference starts with a tag && ends with blank line */
		if ( p && is_tag( p ) ) {
			addtag( &reffields, p );
			inref = TRUE;
		} else if ( p && inref ) {
			/*bug in endnote puts %K only on 1st line of keywords*/
			if ( !strncasecmp( reffields.tag[reffields.nfields-1].data, "KEYWORD", 7 ) ) {
				char buf2[1024];
				strcpy( buf2, "%K ");
				strncat( buf2, p, sizeof(buf2)-4 );
				buf2[1023]='\0';
				addtag( &reffields, buf2 );
			} else {
			int added;
			added = 0;
			while ( *p ) {
			   if (added==0) 
				newstr_addchar(&(reffields.data[reffields.nfields-1]),' ');
			   newstr_addchar(&(reffields.data[reffields.nfields-1]),*p);
			   added++;
			   p++;
			}
			}
		}
	}
	newstr_free( &line );
	return numrefs;
}

void
help( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"\n%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	fprintf(stderr,"Converts an Endnote reference file into XML\n\n");

	fprintf(stderr,"usage: %s endnote_file > xml_file\n\n",progname);
        fprintf(stderr,"  endnote_file can be replaced with file list or omitted to use as a filter\n\n");

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
	int i, j;
	for ( i=0; i<*argc; ++i ) {
		if ( strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0 ) {
			help();
			/* help terminates or we'd remove from argv */
		}
		if ( strcmp(argv[i],"-v")==0 || strcmp(argv[i],"--version")==0){
			tellversion();
			/* tellversion terminates or we'd remove from argv*/
		}
		if ( strcmp(argv[i],"--verbose")==0 ) {
			verbose = 1;
			for ( j=i+1; j<*argc; ++j )
				argv[j-1] = argv[j];
			*argc = *argc-1;
		}
	}
}


int 
main(int argc, char *argv[])
{
	extern void output_listheader( FILE* );
	extern void output_listfooter( FILE* );
	int 	i;
	FILE 	*fp;
	long 	numref = 0L;

	process_args( &argc, argv );

	output_listheader( stdout );
	if (argc==1) 
		numref=get_refs( stdin, "stdin" );
	else {
		for (i=1; i<argc; i++) {
			fp=fopen(argv[i],"r");
			if (fp==NULL) {
				fprintf(stderr,"%s %s: cannot open %s\n",
					progname, version, argv[i]);
			} else { 
				numref += get_refs( fp, argv[i] );
				fclose( fp );
			}
		}
	}
	output_listfooter( stdout );

	fprintf(stderr,"%s %s:  Processed %ld references.\n",
		progname, version, numref);
	return EXIT_SUCCESS;
}
 

