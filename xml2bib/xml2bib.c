/*---------------------------------------------------------------------------

xml2bib --   Bibliography XML to RIS format Pre-Reference Manager 

---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "newstr.h"
#include "search.h"
#include "xml.h"

#define TRUE (1==1)
#define FALSE (!TRUE)

void notify (char *message)
{
  fprintf(stderr,"%s",message);
}

void strip_spaces(char *str)
 {
   char *p,*q;
   p=str;
   q=str;
   while (*(p-1)) {
     if (*p!=' ') *q++=*p;
     p++;
   }
 }

int iswhitespace (char ch)
{
  if (ch==' ' || ch=='\t') return TRUE;
  else return FALSE;
}

void process_authors(newstring *authors, FILE *outptr)
{
	long numauthor=0;
	newstring *author = NULL, *part = NULL;
	char      *pos1, *pos2;

	if (authors==NULL || authors->data==NULL || authors->data[0]=='\0') return;

	fprintf(outptr,"\nAUTHOR=\"");
	/* extract each author */
	pos1 = authors->data;
	while (pos1!=NULL && *pos1!='\0') {
		pos1 = extract_xmldata(pos1,"AUTHOR",&author);
		if (author==NULL || author->data==NULL || author->data[0]=='\0')
			continue;
		if (numauthor>0) fprintf(outptr,"\nand ");
		pos2 = author->data;
		while (pos2!=NULL && *pos2!='\0') {
			pos2 = extract_xmldata(pos2,"PREF",&part);
			if (part!=NULL && part->data!=NULL && part->data[0]!='\0')
				fprintf(outptr,"%s ",part->data);
		}
		(void) extract_xmldata(author->data,"LAST",&part);
		if (part==NULL || part->data==NULL || part->data[0]=='\0') 
			continue;
		fprintf(outptr,"%s",part->data);
		numauthor++;
	}
	fprintf(outptr,"\"");

	if (author!=NULL) {
		newstr_clear(author);
		free(author);
	}
}

void process_pages( newstring *pages, FILE *outptr )
{
	newstring 	*sp=NULL;
	newstring 	*ep=NULL;

	fprintf(outptr,"\nPAGES=\"");
	(void) extract_xmldata(pages->data,"START",&sp);
	(void) extract_xmldata(pages->data,"END",&ep);
	if (sp!=NULL && sp->data!=NULL && sp->data[0]!='\0') {
		fprintf(outptr,"%s",sp->data);
		newstr_clear(sp);
	}
	if (ep!=NULL && ep->data!=NULL && ep->data[0]!='\0') {
		if (sp!=NULL) fprintf(outptr,"--");
		fprintf(outptr,"%s",ep->data);
		newstr_clear(ep);
	}
	fprintf(outptr,"\"");
	if (sp!=NULL) free(sp);
	if (ep!=NULL) free(ep);
}

/*#define NUMFIELDS (14) */

void process_article (FILE *outptr, char *buffer, long refnum)
{
	char *tags[]={"REFNUM","AUTHORS","YEAR","TITLE","JOURNAL",
		"VOLUME", "PAGES", "EDITOR", "PUBLISHER",
		"ADDRESS", "CHAPTER", "BOOKTITLE", "EDITOR", "ABSTRACT"};
	int  NUMFIELDS;
	int 	i,numelem=0;
	char 	*p;
	newstring *s = NULL;

	NUMFIELDS = sizeof(tags)/sizeof(char*);

	(void) extract_xmldata(buffer,"TYPE",&s);
	if (s!=NULL) {
		p = s->data;
		if (p!=NULL && strncasecmp(p,"ARTICLE",7)==0)  
			fprintf(outptr,"@ARTICLE(");
		else if (p!=NULL && strncasecmp(p,"INBOOK",6)==0) 
			fprintf(outptr,"@INBOOK(");
		else if (p!=NULL && strncasecmp(p,"INPROCEEDINGS",13)==0) 
			fprintf(outptr,"@INPROCEEDINGS(");
		else if (p!=NULL && strncasecmp(p,"BOOK",4)==0) 
			fprintf(outptr,"@BOOK(");
		else if (p!=NULL && strncasecmp(p,"PHDTHESIS",9)==0)  
			fprintf(outptr,"@PHDTHESIS");
		else {
			fprintf(stderr,"xml2bib: cannot identify TYPE");
			if (p!=NULL) fprintf(stderr," %s\n",p);
			else fprintf(stderr," in reference %ld\n",refnum+1);
			fprintf(outptr,"@ARTICLE(");  /* default */
		}
	} else {
		fprintf(stderr,"xml2bib: cannot find type in reference %ld\n",refnum+1);
		fprintf(outptr,"@ARTICLE(");
	}

	for (i=0; s!=NULL && i<NUMFIELDS; ++i) {
		if (i==0) {  /* Refnum */
			(void) extract_xmldata(buffer,"REFNUM",&s);
			if (s!=NULL && s->data!=NULL && s->data[0]!='\0') {
				fprintf(outptr,"%s",s->data);
			} else fprintf(outptr,"%ld",refnum);
			numelem++;
		} else if (i==1) {  /* Name */
			(void) extract_xmldata(buffer,"AUTHORS",&s);
			if (s!=NULL && s->data!=NULL && s->data[0]!='\0') {
				if (numelem>0) fprintf(outptr,",");
				process_authors(s,outptr);
				numelem++;
			}
         	} else if (i==6) { /* Pages */
			(void) extract_xmldata(buffer,"PAGES",&s);
			if (s!=NULL && s->data!=NULL && s->data[0]!='\0') {
				if (numelem>0) fprintf(outptr,",");
				process_pages(s,outptr);
				numelem++;
			}
         	} else {  /* Not Name or Pages*/
			(void) extract_xmldata(buffer,tags[i],&s);
			if (s!=NULL && s->data!=NULL && s->data[0]!='\0') {
				if (numelem>0) fprintf(outptr,",");
				fprintf(outptr,"\n%s=\"%s\"",tags[i],
					s->data); 
				numelem++;
			}
		}
	}

	fprintf(outptr,")\n\n");
	fflush(outptr);
   
	if (s!=NULL) {
		newstr_clear(s);
		free(s);
	}
}

/*
 *   read_refs()
 *
 *         Reads references one at a time into buffer
 *         and send to process.
 */
long 
read_refs(FILE *inptr, FILE *outptr)
{
	newstring buffer,*ref;
	char line[256],*errorptr,*startptr,*endptr;
	int haveref = FALSE, processref = FALSE;
	long numrefs =0L;

	newstr_init(&buffer);
	ref = (newstring *) malloc( sizeof(newstring) );
	if (ref==NULL) {
		fprintf(stderr,"xml2bib: cannot allocate memory\n");
		exit(EXIT_FAILURE);
	}
	newstr_init(ref);

	while (!feof(inptr)) {

		errorptr = fgets (line, sizeof(line), inptr);
		if (errorptr != NULL) {
			startptr = search(line,"<REF>");
			if (startptr != NULL || haveref ) {
				haveref = TRUE;
				if (startptr!=NULL) newstr_strcat(&buffer,startptr);
				else newstr_strcat(&buffer,line);
				endptr = find_xmlenddata(buffer.data,"REF"); 
				if (endptr!=NULL) {
					char *p;
					processref=TRUE;
					p = extract_xmldata(buffer.data,"REF",&ref);
					buffer.data[0]='\0';
					while (p!=NULL && *p!='\0') {
						newstr_addchar(&buffer,*p);
						p++;
					}
					startptr = search(buffer.data,"<REF>"); 
					if (startptr!=NULL) haveref=TRUE;
					else haveref=FALSE;
				}
			}
			if (processref) {
				process_article(outptr,ref->data,numrefs+1);
				buffer.data[0]='\0';
				processref = FALSE;
				numrefs++;
				if (endptr!=NULL) newstr_strcpy(&buffer,endptr+6);

			}
		}

  	}

   newstr_clear (&buffer);
   return numrefs;
}


int 
main(int argc, char *argv[])
{
	FILE 	*inptr=stdin, *outptr=stdout;
	long 	numrefs=0L;
	int	i;

	if (argc==1) {
		numrefs = read_refs(inptr,outptr);
	}
	else {
		for (i=1; i<argc; ++i) {
			inptr = fopen(argv[i],"r");
			if (inptr!=NULL) {
				numrefs += read_refs(inptr,outptr);
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


