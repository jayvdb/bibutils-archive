/*---------------------------------------------------------------------------

xml2en --   Bibliography XML to EndNote

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
	newstring *author = NULL, *part = NULL;
	char      *pos1, *pos2;
	int	  numauthors=0;

	if (authors==NULL || authors->data==NULL || authors->data[0]=='\0') return;

	/* extract each author */
	pos1 = authors->data;
	while (pos1!=NULL && *pos1!='\0') {
		pos1 = extract_xmldata(pos1,"AUTHOR",&author);
		if (author==NULL || author->data==NULL || author->data[0]=='\0')
			continue;
		if (numauthors>0) fprintf(outptr,"//");
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
		numauthors++;
	}
	fprintf(outptr,"\t");

	if (author!=NULL) {
		newstr_clear(author);
		free(author);
	}
}

void process_pages( newstring *pages, FILE *outptr )
{
	newstring 	*sp=NULL;
	newstring 	*ep=NULL;

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
	printf("\t");
	if (sp!=NULL) free(sp);
	if (ep!=NULL) free(ep);
}

#define NUMFIELDS (7)

void process_article (FILE *outptr, char *buffer)
{
	char *tags[NUMFIELDS]={"AUTHORS","YEAR","TITLE","JOURNAL",
		"VOLUME", "PAGES", "REFNUM"};
	int 	i;
	char 	*p;
	newstring *s = NULL;

	/*****
	(void) extract_xmldata(buffer,"TYPE",&s);
	if (s!=NULL) {
		p = s->data;
		if (strncasecmp(p,"ARTICLE",7)==0)  
			fprintf(outptr,"\nTY  - JOUR\n");
		else if (strncasecmp(p,"INBOOK",6)==0) 
			fprintf(outptr,"\nTY  - CHAP\n");
		else if (strncasecmp(p,"INPROCEEDINGS",13)==0) 
			fprintf(outptr,"\n  - CHAP\n");
		else if (strncasecmp(p,"BOOK",4)==0) 
			fprintf(outptr,"\nTY  - BOOK\n");
		else if (strncasecmp(p,"PHDTHESIS",9)==0)  
			fprintf(outptr,"\nTY  - BOOK\n");
		else {
			fprintf(stderr,"xml2en: cannot identify TYPE %s\n",p);
			fprintf(outptr,"\nTY  - JOUR\n");  
		}
	}
	*****/

	for (i=0; i<NUMFIELDS; ++i) {
		if (i==0) {  /* Name */
			(void) extract_xmldata(buffer,"AUTHORS",&s);
			process_authors(s,outptr);
         	} else if (i==5) { /* Pages */
			(void) extract_xmldata(buffer,"PAGES",&s);
			process_pages(s,outptr);
         	} else {  /* Not Name or Pages*/
			(void) extract_xmldata(buffer,tags[i],&s);
			if (s!=NULL && s->data!=NULL && s->data[0]!='\0') {
				fprintf(outptr,"%s\t",s->data);
			} else fprintf(outptr,"\t");
		}
	}

	fprintf(outptr,"\n");
   
	if (s!=NULL) {
		newstr_clear(s);
		free(s);
	}

	fflush(outptr);
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
		fprintf(stderr,"xml2en: cannot allocate memory\n");
		exit(EXIT_FAILURE);
	}
	newstr_init(ref);

	fprintf(outptr,"*Journal Article\n");
	fprintf(outptr,"Author\tYear\tTitle\tJournal\tVolume\tPages\tNotes\n");

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
				process_article(outptr,ref->data);
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
	int 	i;

	if (argc==1) {
		numrefs = read_refs(inptr,outptr);
	} else {
		for (i=1; i<argc; ++i) {
			inptr = fopen(argv[i],"r");
			if (inptr!=NULL) {
				numrefs += read_refs(inptr,outptr);
				fclose(inptr);
			} else {
				fprintf(stderr,"xml2en: cannot open %s\n",
					argv[i]);
			}
		}
	}

	fprintf(stderr,"xml2en:   Processed %ld references.\n",numrefs+1);

	return EXIT_SUCCESS;
}


