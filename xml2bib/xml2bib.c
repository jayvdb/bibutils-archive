/*---------------------------------------------------------------------------

xml2bib --   Bibliography XML to RIS format Pre-Reference Manager 

---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "newstr.h"
#include "strsearch.h"
#include "xml.h"

char   progname[] = "xml2bib";
char   version[]  = "1.9 7/23/04";

int    finalcomma = 0;
int    singledash = 0;
int    whitespace = 0;
int    brackets   = 0;

void
output_element( FILE *outptr, char *tag, char *data )
{
	if ( whitespace ) fprintf( outptr, "\n  %s = \t", tag );
	else fprintf( outptr, "\n%s=", tag );

	if ( brackets ) fprintf( outptr, "{" );
	else fprintf( outptr, "\"" );

	fprintf( outptr, "%s", data );

	if ( brackets ) fprintf( outptr, "}" );
	else fprintf( outptr, "\"" ); 
}

void 
process_people(newstring *people, FILE *outptr, char *tag )
{
	newstring person, part, part2, allpeople;
	char      *pos1, *pos2, singletag[50];
	long      numpeople=0;

	if (people==NULL || people->data==NULL || people->data[0]=='\0') return;
	newstr_init( &allpeople );
	newstr_init( &person );
	newstr_init( &part );
	newstr_init( &part2 );

	strncpy( singletag, tag, sizeof( singletag ) );
	singletag[49] = '\0';
	singletag[ strlen(singletag)-1 ] = '\0'; /* AUTHORS->AUTHOR */

	/* extract each person */
	pos1 = people->data;
	while (pos1!=NULL && *pos1!='\0') {
		pos1 = xml_extractdata(pos1,singletag,&person);
		if (person.data==NULL || person.data[0]=='\0') continue;
		if ( numpeople>0 ) {
			if ( whitespace )
				newstr_strcat( &allpeople, "\n\t\tand " );
			else newstr_strcat( &allpeople, "\nand " );
		}
		pos2 = person.data;
		while (pos2!=NULL && *pos2!='\0') {
			pos2 = xml_extractdata(pos2,"PREF",&part);
			if (part.data!=NULL && part.data[0]!='\0') {
				newstr_decodexml( &part );
				newstr_strcat( &allpeople, part.data );
				newstr_addchar( &allpeople, ' ' );
			}
		}
		(void) xml_extractdata(person.data,"LAST",&part);
		if (part.data==NULL || part.data[0]=='\0') 
			continue;
		newstr_decodexml( &part );
		(void) xml_extractdata(person.data,"SUFF",&part2);
		if (part2.data==NULL || part2.data[0]=='\0') {
			newstr_strcat( &allpeople, part.data );
		} else {
			newstr_decodexml( &part2 );
			newstr_addchar( &allpeople, '{' );
			newstr_strcat( &allpeople, part.data );
			newstr_addchar( &allpeople, ' ' );
			newstr_strcat( &allpeople, part2.data );
			newstr_addchar( &allpeople, '}' );
		}
		numpeople++;
	}
	output_element( outptr, singletag, allpeople.data );

	newstr_free(&allpeople);
	newstr_free(&person);
	newstr_free(&part);
	newstr_free(&part2);
}

void
process_date( newstring *date, FILE *outptr )
{
	char *months[12]={ "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	int year=0, month=0, day=0, whichmonth;
	newstring part;
	newstr_init( &part );

	(void) xml_extractdata(date->data,"YEAR",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		year = 1;
		newstr_decodexml( &part );
		output_element( outptr, "YEAR", part.data );
	}

	(void) xml_extractdata(date->data,"MONTH",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		newstr_decodexml( &part );
		if (year) fprintf(outptr,",");
		month = 1;
		whichmonth = atoi(part.data)-1;
		if (whichmonth<0) {
			output_element( outptr, "MONTH", part.data );
		} else {
			if (whichmonth>11) whichmonth=11;
			output_element( outptr, "MONTH", months[whichmonth] );
		}
	}

	(void) xml_extractdata(date->data,"DAY",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		newstr_decodexml( &part );
		if ((year && !month) || month ) fprintf(outptr,",");
		day = 1;
		output_element( outptr, "DAY", part.data );
	}

	(void) xml_extractdata(date->data,"OTHER",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		newstr_decodexml( &part );
		if ((year && !month && !day) || (month &&!day) || day )
			fprintf(outptr,",");
		output_element( outptr, "DATEOTHER", part.data );
	}

	newstr_free( &part );
}

void 
process_pages( newstring *pages, FILE *outptr )
{
	newstring part, outpages;
	int       npages=0;

	newstr_init( &part );
	newstr_init( &outpages );

	(void) xml_extractdata(pages->data,"START",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		newstr_decodexml( &part );
		newstr_strcat( &outpages, part.data );
		npages++;
	}
	newstr_empty( &part );
	(void) xml_extractdata(pages->data,"END",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		newstr_decodexml( &part );
		if ( npages ) {
		       	if ( !singledash ) newstr_strcat( &outpages, "--" );
			else newstr_addchar( &outpages, '-' );
		}
		newstr_strcat( &outpages, part.data );
	}
	output_element( outptr, "PAGES", outpages.data );

	newstr_free( &outpages );
	newstr_free( &part );
}

void
process_keywords( newstring *keywords, FILE *outptr )
{
	newstring keyword, allwords;
	char      *p = keywords->data;
	int       nkeywords=0;

	newstr_init( &keyword );
	newstr_init( &allwords );
	while ( *p ) {
		p = xml_extractdata(p,"KEYWORD",&keyword);
		if (keyword.data!=NULL && keyword.data[0]!='\0') {
			if ( nkeywords++ ) newstr_strcat( &allwords, ", " );
			newstr_decodexml( &keyword );
			newstr_strcat( &allwords, keyword.data );
		}
	}
	output_element( outptr, "KEYWORDS", allwords.data );
	newstr_free( &allwords );
	newstr_free( &keyword );
}

void
process_serialnum( newstring *serialnum, FILE *outptr )
{
	char *tags[] = { "SERIALNUMBER", "ISSN", "ISBN" };
	int  ndigits=0, ntag = 0;
	char *p;
	newstr_decodexml( serialnum );
	p = serialnum->data;
	while ( *p ) {
		if ( (*p>='0' && *p<='9') || *p=='x' || *p=='X' ) 
			ndigits++;
		p++;
	}
	if ( ndigits==8 ) ntag = 1;
	else if ( ndigits==10 || ndigits==13 ) ntag = 2;
	output_element( outptr, tags[ntag], serialnum->data );
}

#define PROCESS_SIMPLE  (0)
#define PROCESS_PERSON  (1)
#define PROCESS_DATE    (2)
#define PROCESS_PAGES   (3)
#define PROCESS_KEYWORD (4)
#define PROCESS_SERIAL  (5)

typedef struct lookup {
	char *xml;
	char *bibtex;
	int  type;
} lookup;

void 
process_article( FILE *outptr, newstring *ref, long refnum )
{
	lookup tags[] = {
		{ "AUTHORS",        "AUTHORS",       PROCESS_PERSON  },
		{ "DATE",           "",              PROCESS_DATE    },
		{ "TITLE",          "TITLE",         PROCESS_SIMPLE  },
		{ "JOURNAL",        "JOURNAL",       PROCESS_SIMPLE  },
		{ "VOLUME",         "VOLUME",        PROCESS_SIMPLE  },
		{ "PAGES",          "PAGES",         PROCESS_PAGES   },
		{ "EDITORS",        "EDITOR",        PROCESS_PERSON  },
		{ "PUBLISHER",      "PUBLISHER",     PROCESS_SIMPLE  },
		{ "ADDRESS",        "ADDRESS",       PROCESS_SIMPLE  },
		{ "CHAPTER",        "CHAPTER",       PROCESS_SIMPLE  },
		{ "BOOKTITLE",      "BOOKTITLE",     PROCESS_SIMPLE  },
		{ "ABSTRACT",       "ABSTRACT",      PROCESS_SIMPLE  },
		{ "URL",            "URL",           PROCESS_SIMPLE  },
		{ "KEYWORDS",       "KEYWORDS",      PROCESS_KEYWORD },
		{ "SERIALNUM",      "",              PROCESS_SERIAL  },
		{ "ISSUE",          "NUMBER",        PROCESS_SIMPLE  },
		{ "NOTES",          "NOTE",          PROCESS_SIMPLE  },
		{ "SECONDARYTITLE", "SECONDARYTITLE",PROCESS_SIMPLE  },
		{ "SERIESTITLE",    "SERIESTITLE",   PROCESS_SIMPLE  },
		{ "REPRINTSTATUS",  "REPRINTSTATUS", PROCESS_SIMPLE  },
		{ "NOTES2",         "NOTES2",        PROCESS_SIMPLE  },
		{ "NUMBER",         "NUMBER",        PROCESS_SIMPLE  }
	};
	int 	numfields = sizeof(tags) / sizeof(lookup);
	int 	i,numelem=0;
	char 	*p,*buffer;
	newstring s;

	if ( !ref ) return;
	buffer = ref->data;
	newstr_init( &s );

	(void) xml_extractdata(buffer,"TYPE",&s);
	if ((p=s.data)!=NULL) {
		if (strsearch(p,"ARTICLE")==p)  
			fprintf(outptr,"@ARTICLE{");
		else if (strsearch(p,"INBOOK")==p) 
			fprintf(outptr,"@INBOOK{");
		else if (strsearch(p,"INPROCEEDINGS")==p) 
			fprintf(outptr,"@INPROCEEDINGS{");
		else if (strsearch(p,"BOOK")==p) 
			fprintf(outptr,"@BOOK{");
		else if (strsearch(p,"PHDTHESIS")==p)  
			fprintf(outptr,"@PHDTHESIS{");
		else {
			fprintf(stderr,"xml2bib: cannot identify TYPE");
			if (p!=NULL) fprintf(stderr," %s\n",p);
			else fprintf(stderr," in reference %ld\n",refnum+1);
			fprintf(outptr,"@ARTICLE{");  /* default */
		}
	} else {
		fprintf(stderr,"xml2bib: cannot find type in reference %ld\n",
				refnum+1);
		fprintf(outptr,"@ARTICLE(");
	}

	/* Refnum */
	(void) xml_extractdata(buffer,"REFNUM",&s);
	if (s.data!=NULL && s.data[0]!='\0') {
		newstr_decodexml( &s );
		newstr_findreplace( &s, " ", "" );
		newstr_findreplace( &s, "\t", "" );
		fprintf(outptr,"%s",s.data);
	} else fprintf(outptr,"%ld",refnum);
	numelem++;

	for ( i=0; i<numfields; ++i ) {
		(void) xml_extractdata( buffer, tags[i].xml, &s );
		if ( s.data==NULL || s.data[0]=='\0' ) continue;
		if ( numelem ) fprintf( outptr, "," );
		if ( tags[i].type==PROCESS_PERSON ) {
			process_people(&s,outptr,tags[i].xml );
		} else if ( tags[i].type==PROCESS_DATE ) {
			newstr_decodexml( &s );
			newstr_findreplace( &s, "\"", "\'" );
			process_date(&s,outptr);
		} else if ( tags[i].type==PROCESS_PAGES ) {
			newstr_decodexml( &s );
			newstr_findreplace( &s, "\"", "\'" );
			process_pages(&s,outptr);
		} else if ( tags[i].type==PROCESS_KEYWORD ) {
			newstr_decodexml( &s );
			newstr_findreplace( &s, "\"", "\'" );
			process_keywords(&s,outptr);
		} else if ( tags[i].type==PROCESS_SERIAL ) {
			newstr_decodexml( &s );
			newstr_findreplace( &s, "\"", "\'" );
			process_serialnum(&s,outptr);
		} else {  /*  tags[i].type==PROCESS_SIMPLE  */
			newstr_decodexml( &s );
			newstr_findreplace( &s, "\"", "\'" );
			output_element( outptr, tags[i].bibtex, s.data );
		}
		numelem++;
	}
	if ( finalcomma && numelem>0 ) fprintf(outptr,",");
	fprintf(outptr,"\n}\n\n");
	fflush(outptr);
   
	newstr_free(&s);
}

void
help( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"\n%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	fprintf(stderr,"Converts an XML intermediate reference file into Bibtex\n\n");

	fprintf(stderr,"usage: %s xml_file > bibtex_file\n\n",progname);
        fprintf(stderr,"  xml_file can be replaced with file list or omitted to use as a filter\n\n");

	fprintf(stderr,"  -h, --help     display this help\n");
	fprintf(stderr,"  -v, --version  display version\n");
	fprintf(stderr,"  -fc, --finalcomma add final comma to bibtex output\n");
	fprintf(stderr,"  -sd, --singledash use only one dash '-' instead of two '--' for page range\n");
	fprintf(stderr,"  -b, --brackets    use brackets, not quotations\n");
	fprintf(stderr,"  -w, --whitespace  add beautifying whitespace to output\n\n");

	fprintf(stderr,"Citation codes generated from <REFNUM> tag.   See \n");
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
	int i, j, subtract;
	i = 0;
	while ( i < *argc ) {
		subtract = 0;
		if ( !strcmp(argv[i],"-h") || !strcmp(argv[i],"--help") ) {
			i++;
			subtract = 1;
			help();
		}
		else if (!strcmp(argv[i],"-v") || !strcmp(argv[i],"--version")){
			i++;
			subtract = 1;
			tellversion();
		}
		else if (!strcmp(argv[i],"-fc") || !strcmp(argv[i],"--finalcomma")) {
			finalcomma = 1;
			subtract = 1;
		}
		else if (!strcmp(argv[i],"-sd") || !strcmp(argv[i],"--singledash")) {
			singledash = 1;
			subtract = 1;
		}
		else if (!strcmp(argv[i],"-b") || !strcmp(argv[i],"--brackets")){
			brackets = 1;
			subtract = 1;
		}
		else if (!strcmp(argv[i],"-w") || !strcmp(argv[i],"--whitespace")) {
			whitespace = 1;
			subtract = 1;
		}
		if ( subtract ) {
			for ( j=i+subtract; j<*argc; ++j )
				argv[j-subtract] = argv[j];
			*argc -= subtract;
		} else i++;
	}
}

int 
main(int argc, char *argv[])
{
	FILE 	*inptr=stdin, *outptr=stdout;
	long 	numrefs=0L;
	int	i;

	process_args( &argc, argv );

	if (argc==1) {
		numrefs = xml_readrefs(inptr,outptr);
	}
	else {
		for (i=1; i<argc; ++i) {
			inptr = fopen(argv[i],"r");
			if (inptr!=NULL) {
				numrefs += xml_readrefs(inptr,outptr);
				fclose( inptr );
			} else {
				fprintf(stderr,"xml2bib: cannot open %s\n",
					argv[i]);
			}
		}
	}

	fprintf(stderr,"%s %s:  Processed %ld references.\n",
			progname, version, numrefs);

	return EXIT_SUCCESS;
}


