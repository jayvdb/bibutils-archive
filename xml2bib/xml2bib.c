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
char   version[]  = "1.3 02/18/03";

void 
process_people(newstring *people, FILE *outptr, char *xmltag, char *bibtextag)
{
	newstring person, part, part2;
	char      *pos1, *pos2;
	long      numpeople=0;

	if (people==NULL || people->data==NULL || people->data[0]=='\0') return;
	newstr_init( &person );
	newstr_init( &part );
	newstr_init( &part2 );

	fprintf(outptr,"\n%s=\"",bibtextag);
	/* extract each person */
	pos1 = people->data;
	while (pos1!=NULL && *pos1!='\0') {
		pos1 = xml_extractdata(pos1,xmltag,&person);
		if (person.data==NULL || person.data[0]=='\0') continue;
		if (numpeople>0) fprintf(outptr,"\nand ");
		pos2 = person.data;
		while (pos2!=NULL && *pos2!='\0') {
			pos2 = xml_extractdata(pos2,"PREF",&part);
			if (part.data!=NULL && part.data[0]!='\0')
				fprintf(outptr,"%s ",part.data);
		}
		(void) xml_extractdata(person.data,"LAST",&part);
		if (part.data==NULL || part.data[0]=='\0') 
			continue;
		(void) xml_extractdata(person.data,"SUFF",&part2);
		if (part2.data==NULL || part2.data[0]=='\0')
			fprintf(outptr,"%s",part.data);
		else    fprintf(outptr,"{%s %s}",part.data,part2.data);
		numpeople++;
	}
	fprintf(outptr,"\"");

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
		fprintf(outptr,"\nYEAR=\"%s\"",part.data);
	}

	(void) xml_extractdata(date->data,"MONTH",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		if (year) fprintf(outptr,",");
		month = 1;
		whichmonth = atoi(part.data)-1;
		if (whichmonth<0) whichmonth=0;
		else if (whichmonth>11) whichmonth=11;
		fprintf(outptr,"\nMONTH=\"%s\"",months[whichmonth]);
	}

	(void) xml_extractdata(date->data,"DAY",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		if ((year && !month) || month ) fprintf(outptr,",");
		day = 1;
		fprintf(outptr,"\nDAY=\"%s\"",part.data);
	}

	(void) xml_extractdata(date->data,"OTHER",&part);
	if (part.data!=NULL && part.data[0]!='\0') {
		if ((year && !month && !day) || (month &&!day) || day )
			fprintf(outptr,",");
		fprintf(outptr,"\nDATEOTHER=\"%s\"",part.data);
	}

	newstr_free( &part );
}

void 
process_pages( newstring *pages, FILE *outptr )
{
	newstring sp, ep;
	int       npages=0;

	newstr_init( &sp );
	newstr_init( &ep );

	fprintf(outptr,"\nPAGES=\"");
	(void) xml_extractdata(pages->data,"START",&sp);
	if (sp.data!=NULL && sp.data[0]!='\0') {
		fprintf(outptr,"%s",sp.data);
		npages++;
	}
	(void) xml_extractdata(pages->data,"END",&ep);
	if (ep.data!=NULL && ep.data[0]!='\0') {
		if (npages) fprintf(outptr,"--");
		fprintf(outptr,"%s",ep.data);
	}
	fprintf(outptr,"\"");

	newstr_free( &sp );
	newstr_free( &ep );
}

void
process_keywords( newstring *keywords, FILE *outptr )
{
	newstring keyword;
	char      *p = keywords->data;
	int       nkeywords=0;

	newstr_init( &keyword );
	fprintf(outptr,"\nKEYWORDS=\"");
	while ( *p ) {
		p = xml_extractdata(p,"KEYWORD",&keyword);
		if (keyword.data!=NULL && keyword.data[0]!='\0') {
			if (nkeywords++) fprintf(outptr,", ");
			fprintf(outptr,"%s",keyword.data);
		}
	}
	fprintf(outptr,"\"");
	newstr_free( &keyword );
}

void 
process_article (FILE *outptr, newstring *ref, long refnum)
{
	char	*xmltags[]={ "REFNUM","AUTHORS","DATE",
			"TITLE","JOURNAL",
			"VOLUME", "PAGES", "EDITOR", "PUBLISHER",
			"ADDRESS", "CHAPTER", "BOOKTITLE",  
			"ABSTRACT", "URL", "KEYWORDS", "SERIALNUM", "ISSUE",
       			"NOTES", "SECONDARYTITLE", "SERIESTITLE",
			"REPRINTSTATUS", "NOTES2"	};
	char	*bibtextags[]={ "REFNUM","AUTHORS","", 
			"TITLE","JOURNAL",
			"VOLUME", "PAGES", "EDITOR", "PUBLISHER",
			"ADDRESS", "CHAPTER", "BOOKTITLE",  
			"ABSTRACT", "URL", "KEYWORDS", "ISBN", "NUMBER",
       			"NOTE", "SECONDARYTITLE", "SERIESTITLE",
			"REPRINTSTATUS", "NOTES2"	};
	int 	numfields = sizeof(xmltags) / sizeof(char*);
	int 	i,numelem=0;
	char 	*p,*buffer;
	newstring s;

	if ( ref == NULL ) return;
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

	for (i=0; i<numfields; ++i) {
		if (i==0) {  /* Refnum */
			(void) xml_extractdata(buffer,"REFNUM",&s);
			if (s.data!=NULL && s.data[0]!='\0') {
				fprintf(outptr,"%s",s.data);
			} else fprintf(outptr,"%ld",refnum);
			numelem++;
		} else if (i==1) {  /* Name */
			(void) xml_extractdata(buffer,"AUTHORS",&s);
			if (s.data!=NULL && s.data[0]!='\0') {
				if (numelem>0) fprintf(outptr,",");
				process_people(&s,outptr,"AUTHOR","AUTHOR");
				numelem++;
			}
		} else if (i==2) {  /* DATE */
			(void) xml_extractdata(buffer,"DATE",&s);
			if (s.data!=NULL && s.data[0]!='\0') {
				if (numelem>0) fprintf(outptr,",");
				process_date(&s,outptr);
				numelem++;
			}
		} else if (i==7) {  /* Editors */
			(void) xml_extractdata(buffer,"EDITORS",&s);
			if (s.data!=NULL && s.data[0]!='\0') {
				if (numelem>0) fprintf(outptr,",");
				process_people(&s,outptr,"EDITOR","EDITOR");
				numelem++;
			}
         	} else if (i==6) { /* Pages */
			(void) xml_extractdata(buffer,"PAGES",&s);
			if (s.data!=NULL && s.data[0]!='\0') {
				if (numelem>0) fprintf(outptr,",");
				process_pages(&s,outptr);
				numelem++;
			}
		} else if (i==14) { /*keywords */
			(void) xml_extractdata(buffer,"KEYWORDS",&s);
			if (s.data!=NULL && s.data[0]!='\0') {
				if (numelem>0) fprintf(outptr,",");
				process_keywords(&s,outptr);
				numelem++;
			}
         	} else {  /* default */
			(void) xml_extractdata(buffer,xmltags[i],&s);
			if (s.data!=NULL && s.data[0]!='\0') {
				if (numelem>0) fprintf(outptr,",");
				fprintf(outptr,"\n%s=\"%s\"",bibtextags[i],
					s.data); 
				numelem++;
			}
		}
	}

	fprintf(outptr,"}\n\n");
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
	fprintf(stderr,"  -v, --version  display version\n\n");

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

	fprintf(stderr,"xml2bib:  Processed %ld references.\n",numrefs);

	return EXIT_SUCCESS;
}


