/*---------------------------------------------------------------------------

xml2ris --   Bibliography XML to RIS format Pre-Reference Manager 

---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "newstr.h"
#include "strsearch.h"
#include "xml.h"

char progname[] = "xml2ris";
char version[]  = "1.3 11/01/03";

void
process_person( newstring *person, FILE *outptr )
{
	newstring part;
	int       last = 0, pref = 0, suff = 0;
	char      *p;

	newstr_init( &part );
	(void) xml_extractdata(person->data,"LAST",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		newstr_decodexml( &part );
		fprintf(outptr,"%s",part.data);
		last++;
	}
	p = person->data;
	while ( *p ) {
		p = xml_extractdata(p,"PREF",&part);
		if (part.data!=NULL && part.data[0]!='\0') {
			if ( last && pref==0 ) fprintf(outptr,", ");
			newstr_decodexml( &part );
			fprintf(outptr,"%s ",part.data);
			pref++;
		}
	}
	p = person->data;
	while ( *p ) {
		p = xml_extractdata(p,"SUFF",&part);
		if (part.data!=NULL && part.data[0]!='\0') {
			if ( last && pref==0 && suff==0 ) fprintf(outptr,",");
			if ( last && suff==0 ) fprintf(outptr,",");
			newstr_decodexml( &part );
			fprintf(outptr,"%s ",part.data);
			suff++;
		}
	}
	newstr_free( &part );
}

void 
process_people(newstring *people, FILE *outptr, char *xmltag, char *ristag)
{
	newstring person;
	char      *p;

	if (people==NULL || people->data==NULL || people->data[0]=='\0') 
		return;

	newstr_init(&person);

	p = people->data;
	while ( *p ) {
		p = xml_extractdata(p,xmltag,&person);
		if (person.data!=NULL && person.data[0]!='\0') {
			fprintf(outptr,"%s  - ",ristag);
			process_person( &person, outptr );	
			fprintf(outptr,"%c%c",13,10);
		}
	}

	newstr_free(&person);
}


void
process_date( newstring *date, FILE *outptr )
{
	newstring part;
	newstr_init( &part );
	fprintf(outptr,"PY  - ");
	(void) xml_extractdata(date->data,"YEAR",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		newstr_decodexml( &part );
		fprintf(outptr,"%s",part.data);
	}
	fprintf(outptr,"/");
	(void) xml_extractdata(date->data,"MONTH",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		newstr_decodexml( &part );
		fprintf(outptr,"%s",part.data);
	}
	fprintf(outptr,"/");
	(void) xml_extractdata(date->data,"DAY",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		newstr_decodexml( &part );
		fprintf(outptr,"%s",part.data);
	}
	fprintf(outptr,"/");
	(void) xml_extractdata(date->data,"OTHER",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		newstr_decodexml( &part );
		fprintf(outptr,"%s",part.data);
	}
	fprintf(outptr,"%c%c",13,10);
	newstr_free( &part );
}

void 
process_pages( newstring *pages, FILE *outptr )
{
	newstring sp,ep;
	newstr_init(&sp);
	newstr_init(&ep);
	(void) xml_extractdata(pages->data,"START",&sp);
	if (sp.data!=NULL && sp.data[0]!='\0') {
		newstr_decodexml( &sp );
		fprintf(outptr,"SP  - %s%c%c",sp.data,13,10);
	}
	(void) xml_extractdata(pages->data,"END",&ep);
	if (ep.data!=NULL && ep.data[0]!='\0') {
		newstr_decodexml( &ep );
		fprintf(outptr,"EP  - %s%c%c",ep.data,13,10);
	}
	newstr_free(&sp);
	newstr_free(&ep);
}

/* need to fix editor, seriesauthors, authors, year output */

void 
process_article (FILE *outptr, newstring *ref, long nref)
{
	newstring s;
	char 	*tags[]={"AUTHORS","YEAR","TITLE","JOURNAL",
			"VOLUME", "PAGES", "EDITORS", "PUBLISHER",
			"ADDRESS", "CHAPTER", "BOOKTITLE", "REFNUM",
			"ABSTRACT", "NOTES", "SERIALNUM", "ISSUE", 
			"URL", "SECONDARYTITLE", "SERIESTITLE", "REPRINTSTATUS",
			"SERIESAUTHORS", "NUMBER"	};
	char	*descriptors[]={"AU","","TI","JO", "VL","","",
			"PB", "CT","CP","BT", "ID", "AB", "N1", "SN", 
			"IS", "UR", "T2", "T3", "RP", "", "IS" };
	int 	numfields = sizeof(tags) / sizeof(char*);
	int	i;
	char 	*p, *buffer;

	if ( ref==NULL ) return;
	buffer = ref->data;

	newstr_init(&s);

	(void) xml_extractdata(buffer,"TYPE",&s);
	if ((p=s.data)!=NULL) {
		if (strsearch(p,"ARTICLE")==p)  
			fprintf(outptr,"\nTY  - JOUR\n");
		else if (strsearch(p,"INBOOK")==p) 
			fprintf(outptr,"\nTY  - CHAP\n");
		else if (strsearch(p,"INPROCEEDINGS")==p) 
			fprintf(outptr,"\n  - CHAP\n");
		else if (strsearch(p,"BOOK")==p) 
			fprintf(outptr,"\nTY  - BOOK\n");
		else if (strsearch(p,"PHDTHESIS")==p)  
			fprintf(outptr,"\nTY  - BOOK\n");
		else {
			fprintf(stderr,"xml2ris: cannot identify TYPE\n");
			if (p!=NULL) fprintf(stderr," %s\n",p);
			else fprintf(stderr,"\n");
			fprintf(outptr,"\nTY  - JOUR\n");  /* default */
		}
	} else {
		fprintf(stderr,"xml2ris: cannot identify type\n");
		fprintf(outptr,"\nTY  - JOUR\n");
	}

	for (i=0; i<numfields; ++i) {
		if (i==0) {  /* Name */
			(void) xml_extractdata(buffer,"AUTHORS",&s);
			if ( s.data && s.data[0]!='\0')
				process_people(&s,outptr,"AUTHOR","AU");
         	} else if (i==1) { /* YEAR */
			(void) xml_extractdata(buffer,"DATE",&s);
			if ( s.data && s.data[0]!='\0') 
				process_date(&s,outptr);
         	} else if (i==5) { /* Pages */
			(void) xml_extractdata(buffer,"PAGES",&s);
			if ( s.data && s.data[0]!='\0')
				process_pages(&s,outptr);
         	} else if (i==6) { /* Editors  */
			(void) xml_extractdata(buffer,"EDITORS",&s);
			if ( s.data && s.data[0]!='\0')
				process_people(&s,outptr,"EDITOR","ED");
         	} else if (i==20) { /* Series Authors */
			(void) xml_extractdata(buffer,"SERIESAUTHORS",&s);
			if ( s.data && s.data[0]!='\0')
				process_people(&s,outptr,"AUTHOR","A3");
         	} else {  /* default */
			(void) xml_extractdata(buffer,tags[i],&s);
			if (s.data!=NULL && s.data[0]!='\0') {
				newstr_decodexml( &s );
				fprintf(outptr,"%s  - %s%c%c",descriptors[i],
					s.data,13,10); 
			}
		}
	}
   
	newstr_free(&s);

	fprintf(outptr,"ER  - %c%c",13,10);
	fflush(outptr);
}


void
help( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"\n%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	fprintf(stderr,"Converts an XML intermediate reference file into RIS format\n\n");

	fprintf(stderr,"usage: %s xml_file > ris_file\n\n",progname);
        fprintf(stderr,"  xml_file can be replaced with file list or omitted to use as a filter\n\n");

	fprintf(stderr,"  -h, --help     display this help\n");
	fprintf(stderr,"  -v, --version  display version\n\n");

	fprintf(stderr,"Citation codes (ID  - ) generated from <REFNUM> tag.   See \n");
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
	FILE 	*inptr=stdin, *outptr=stdout;
	long 	numrefs = 0L;
	int 	i;

	process_args( &argc, argv );

	if (argc==1) {
		numrefs = xml_readrefs(inptr,outptr);
	} else {
		for (i=1; i<argc; ++i) {
			inptr = fopen(argv[i],"r");
			if (inptr!=NULL) {
				numrefs += xml_readrefs(inptr,outptr);
				fclose( inptr );
			} else {
				fprintf(stderr,"xml2ris: cannot open %s\n",
					argv[i]);
			}
		}
	}

	fprintf(stderr,"xml2ris:  Processed %ld references.\n",numrefs);

	return EXIT_SUCCESS;
}


