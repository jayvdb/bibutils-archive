/*---------------------------------------------------------------------------

xml2en --   Bibliography XML to EndNote

---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "newstr.h"
#include "strsearch.h"
#include "xml.h"

char progname[] = "xml2en";
char version[]  = "1.1 02/10/03";

void 
process_authors(newstring *authors, FILE *outptr, char *xmltag, char *tag)
{
	newstring author, part;
	char      *pos1, *pos2;

	if (authors==NULL || authors->data==NULL || authors->data[0]=='\0') return;

	newstr_init( &author );
	newstr_init( &part );

	/* extract each author */
	pos1 = authors->data;
	while (pos1!=NULL && *pos1!='\0') {
		pos1 = xml_extractdata(pos1,xmltag,&author);
		if (author.data==NULL || author.data[0]=='\0')
			continue;
		pos2 = author.data;
		fprintf(outptr,"%s ",tag);
		while (pos2!=NULL && *pos2!='\0') {
			pos2 = xml_extractdata(pos2,"PREF",&part);
			if (part.data!=NULL && part.data[0]!='\0')
				fprintf(outptr,"%s ",part.data);
		}
		(void) xml_extractdata(author.data,"LAST",&part);
		if (part.data==NULL || part.data[0]=='\0') 
			continue;
		fprintf(outptr,"%s\n",part.data);
	}

	newstr_free(&author);
	newstr_free(&part);
}

void
process_date( newstring *date, FILE *outptr )
{
	newstring year;
	newstr_init( &year );
	(void) xml_extractdata(date->data,"YEAR",&year);
	if ( year.data!=NULL && year.data[0]!='\0' ) {
		fprintf(outptr,"%%D %s\n",year.data);
	} 
	newstr_free( &year );
}

void 
process_pages( newstring *pages, FILE *outptr )
{
	newstring sp, ep;
	int       npages = 0;

	newstr_init( &sp );
	newstr_init( &ep );

	fprintf(outptr,"%%P ");
	(void) xml_extractdata(pages->data,"START",&sp);
	if (sp.data!=NULL && sp.data[0]!='\0') {
		fprintf(outptr,"%s",sp.data);
		npages++;
	}
	(void) xml_extractdata(pages->data,"END",&ep);
	if (ep.data!=NULL && ep.data[0]!='\0') {
		if (npages) fprintf(outptr,"-");
		fprintf(outptr,"%s",ep.data);
	}
	printf("\n");
	newstr_free(&sp);
	newstr_free(&ep);
}

void
process_keywords( newstring *keywords, FILE *outptr )
{
	newstring word;
	char *p;

	newstr_init( &word );
	p = keywords->data;
	while ( p && *p ) {
		p = xml_extractdata(p,"KEYWORD",&word);
		if (p && *p) fprintf(outptr,"%%K %s\n",word.data);
	}
	newstr_free(&word);
}

void 
process_article (FILE *outptr, newstring *ref, long nref )
{
	newstring s;
	char *xmltags[]={"TYPE","AUTHORS","DATE","TITLE","JOURNAL",
		"VOLUME", "PAGES", "URL", "EDITOR", "ADDRESS",
       		"PUBLISHER", "ISSUE", "ABSTRACT", "NOTES", 
		"KEYWORDS", "REFNUM" };
	char *enimport[]={  "", "", "", "%T", "%J", "%V", "", "%U",
       		"", "%C", "%I", "%N", "%X", "%Z", "", "%F" };
	int  numfields = sizeof( xmltags ) / sizeof( char* );
	char *buffer;
	int  i;

	if ( ref==NULL ) return;
	buffer=ref->data;

	newstr_init( &s );

	for (i=0; i<numfields; ++i) {
		if (i==0) {  /* Type */
			(void) xml_extractdata(buffer,"TYPE",&s);
			if (strcasecmp(s.data,"ARTICLE")==0)
				fprintf(outptr,"%%0 Journal Article\n");
			else if (strcasecmp(s.data,"BOOK")==0)
				fprintf(outptr,"%%0 Book\n");
			else if (strcasecmp(s.data,"INBOOK")==0)
				fprintf(outptr,"%%0 Book Section\n");
			else if (strcasecmp(s.data,"PHDTHESIS")==0)
				fprintf(outptr,"%%0 Thesis\n");
		} else if (i==1) {  /* Authors */
			(void) xml_extractdata(buffer,"AUTHORS",&s);
			process_authors(&s,outptr,"AUTHOR","%A");
		} else if (i==8) {  /* Editor */
			(void) xml_extractdata(buffer,"EDITORS",&s);
			process_authors(&s,outptr,"EDITOR","%E");
		} else if (i==2) { /* Date */
			(void) xml_extractdata(buffer,"DATE",&s);
			process_date(&s,outptr);
         	} else if (i==6) { /* Pages */
			(void) xml_extractdata(buffer,"PAGES",&s);
			process_pages(&s,outptr);
         	} else if (i==14) { /* Keywords */
			(void) xml_extractdata(buffer,"KEYWORDS",&s);
			process_keywords(&s,outptr);
         	} else {  /* Default */
			(void) xml_extractdata(buffer,xmltags[i],&s);
			if (s.data!=NULL && s.data[0]!='\0') {
				fprintf(outptr,"%s %s\n",enimport[i],s.data);
			}
		}
	}

	newstr_free( &s );

	fprintf(outptr,"\n");
	fflush(outptr);
}


void
help( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"\n%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	fprintf(stderr,"Converts an XML intermediate reference file into a pre-EndNote format\n\n");

	fprintf(stderr,"usage: %s xml_file > bibtex_file\n\n",progname);
        fprintf(stderr,"  xml_file can be replaced with file list or omitted to use as a filter\n\n");

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
	FILE 	*inptr=stdin, *outptr=stdout;
	long 	numrefs=0L;
	int 	i;

	process_args( &argc, argv );

	if (argc==1) {
		numrefs = xml_readrefs(inptr,outptr);
	} else {
		for (i=1; i<argc; ++i) {
			inptr = fopen(argv[i],"r");
			if (inptr!=NULL) {
				numrefs += xml_readrefs(inptr,outptr);
				fclose(inptr);
			} else {
				fprintf(stderr,"xml2en: cannot open %s\n",
					argv[i]);
			}
		}
	}

	fprintf(stderr,"xml2en:  Processed %ld references.\n",numrefs);

	return EXIT_SUCCESS;
}


