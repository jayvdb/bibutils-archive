/*---------------------------------------------------------------------------

bib2xml  --   BibTeX to XML


Author:  Chris Putnam (cdputnam@scripps.edu, http://www.scripps.edu/~cdputnam/)


---------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "strsearch.h"
#include "newstr.h"

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

newstring* extract_refnum (char *buffer)
{
   char *ptr = buffer;
   int  quotes = FALSE,
        infield = TRUE;
   newstring *s;

   s = (newstring *) malloc (sizeof(newstring));
   newstr_init(s);
   while (*(ptr-1) != '('  && *(ptr-1) !='{' && *ptr !='\0') ptr++;
   while (infield && *ptr!='\0') {
      if (*ptr=='\"' && *(ptr-1)!='\\') quotes = !quotes;  /*Strip quotes*/
      else if ((*ptr=='{' || *ptr=='}')&&*(ptr-1)!='\\') /* Strip {}*/ ;
      else if (*ptr=='\r' || *ptr=='\n') newstr_addchar(s,' ') /* Strip */;
      else if ((*ptr==',' || *ptr==')') && !quotes) infield=FALSE;
      else {
          newstr_addchar(s,*ptr);
      }
      ptr++;
   }
   return s;
}

void extract_pages(char *buffer,newstring **sp, newstring **ep)
{
   newstring *start,*end;
   start = (newstring *) malloc(sizeof(newstring));
   end = (newstring *) malloc(sizeof(newstring));
   newstr_init(start);
   newstr_init(end);
   if (buffer!=NULL) {
      strip_spaces(buffer);
      while ( *buffer && (*buffer>='0' && *buffer<='9')) { newstr_addchar(start,*buffer); buffer++; }
      while ( *buffer && (!(*buffer>='0' && *buffer<='9'))) buffer++;
      while ( *buffer && (*buffer>='0' && *buffer<='9')) { newstr_addchar(end,*buffer); buffer++; }
   }
   *sp = start;
   *ep = end;
}

#define MAXELEMENT (100)
/*
 * extract_name1()
 *
 * extract names in the format "H. F. Author", w/o comma
 */ 
int extract_name1(char **buffer, newstring **s)
{
   int ok = FALSE;
   int junior = FALSE;
   int third = FALSE;
   int i,element;
   char *p,*q;
   char *name[MAXELEMENT];
   newstring *prename,*postname;
   prename = (newstring *) malloc (sizeof(newstring));
   newstr_init(prename);
   postname = (newstring *) malloc (sizeof(newstring));
   newstr_init(postname);
   if (buffer!=NULL && strlen(*buffer)>0) {
       ok = TRUE;
       p = *buffer;
       while (*p && *p!='|') { newstr_addchar(prename,*p); p++; }
       if (*p=='|') p++;
       if (strsearch(prename->data," Jr.")) { newstr_findreplace(prename," Jr."," "); junior=TRUE; }
       if (strsearch(prename->data," III")) { newstr_findreplace(prename," III"," "); third=TRUE; }
       element = 0;
       q = strtok(prename->data," \t");
       while ( q != NULL ) {
          name[element++]=q;
          if (element == MAXELEMENT) element=MAXELEMENT-1;
          q = strtok(NULL," \t");
          if (q==NULL) {
             if (element-1>0) {
		newstr_strcpy(postname,"<LAST>");
                newstr_strcat(postname,name[element-1]);
                if (junior) newstr_strcat(postname," Jr.");
                if (third)  newstr_strcat(postname," III");
		newstr_strcat(postname,"</LAST>");
                for (i=0; i<element-1; ++i) {
		       newstr_strcat(postname,"<PREF>");
                       newstr_strcat(postname,name[i]);
                       newstr_strcat(postname,"</PREF>");
                }
             }
          }
 
       }
   }
   newstr_clear(prename);
   free(prename);
   *s = postname;
   *buffer = p;
   return ok;
}

/*
 * extract_name2()
 *
 * extract names in the format "Author, H.F.", w/comma
 */ 
int extract_name2(char **buffer, newstring **s)
{
   int ok=FALSE;
   char *p;
   newstring *prename,*postname;
   prename = (newstring *) malloc (sizeof(newstring));
   newstr_init(prename);
   postname = (newstring *) malloc (sizeof(newstring));
   newstr_init(postname);
   if (buffer!=NULL && strlen(*buffer)>0) {
       p = *buffer;
       ok = TRUE;

       /* skip past whitespace */
       while (*p && *p!='|' && (*p==' ' || *p=='\t')) p++; 

       /* get last name */
       newstr_strcpy(postname,"<LAST>");
       while (*p && *p!='|' && *p!=',') { newstr_addchar(postname,*p); p++; }
       newstr_strcat(postname,"</LAST>");

       /* skip past comma and whitespace */
       if ( *p==',' ) p++;
       while (*p && *p!='|' && (*p==' ' || *p=='\t')) p++; 

       /* add each element */
       newstr_strcpy(prename,"");
       while (*p && *p!='|') {
          if (*p!=' ' && *p!='\t') newstr_addchar(prename,*p);
          if (*p==' ' || *p=='\t' || *(p+1)=='|' || *(p+1)=='\0') {
             if (strlen(prename->data)>0) {
                 newstr_strcat(postname,"<PREF>");
                 newstr_strcat(postname,prename->data);
                 newstr_strcat(postname,"</PREF>");
                 newstr_strcpy(prename,"");
             }
          }
          p++;
       }
       if (*p=='|') p++;
   }
   newstr_clear(prename);
   free(prename);
   *s = postname;
   *buffer = p;
   return ok;
}

/*
 * send to appropriate algorithm depending on if the author name is
 * in the format:  "H. F. Author" or "Author, H. F."
 */
int extract_name(char **buffer, newstring **s)
{
	if (strchr(*buffer,',')==NULL) return extract_name1(buffer,s);
	else return extract_name2(buffer,s);
}

newstring* extract_field (char *buffer)
{
   char *ptr = buffer;
   int  quotes = FALSE,
        infield = TRUE;
   newstring *s;

   s = (newstring *) malloc (sizeof(newstring));
   newstr_init(s);
   while (*(ptr-1) != '=' && *ptr !='\0') ptr++;
   while (infield && *ptr!='\0') {
      if (*ptr=='\"' && *(ptr-1)!='\\') quotes = !quotes;  /*Strip quotes*/
      else if ((*ptr=='{' || *ptr=='}')&&*(ptr-1)!='\\') /* Strip {}*/ ;
      else if (*ptr=='\r' || *ptr=='\n') newstr_addchar(s,' ') /* Strip */;
      else if ((*ptr==',' || *ptr==')') && !quotes) infield=FALSE;
      else {
          newstr_addchar(s,*ptr);
      }
      ptr++;
   }
   return s;
}

/*#define NUMFIELDS (13)*/

void process_article (FILE *outptr, char *buffer)
{
	char *field[]={"AUTHOR", "YEAR",   "TITLE",    "JOURNAL",
		"VOLUME", "PAGES",  "EDITOR",   "PUBLISHER",
		"ADDRESS","CHAPTER","BOOKTITLE","EDITOR", "ABSTRACT"
		};
	int NUMFIELDS;
	int i;
	char *p;
	newstring *s;

	NUMFIELDS = sizeof(field)/sizeof(char*);

	fprintf(outptr,"<REF>\n");

	/* Incollection -- part of a book with its own title */

	if (strsearch(buffer,"@ARTICLE")!=NULL) 
	           fprintf(outptr,"  <TYPE>ARTICLE</TYPE>\n");
	else if (strsearch(buffer,"@INBOOK")!=NULL || 
	    strsearch(buffer,"@INPROCEEDINGS")!=NULL ||
            strsearch(buffer,"@INCOLLECTION")!=NULL ) 
		   fprintf(outptr,"  <TYPE>INBOOK</TYPE>\n");
	else if (strsearch(buffer,"@BOOK")!=NULL) 
	           fprintf(outptr,"  <TYPE>BOOK</TYPE>\n");
	else if (strsearch(buffer,"@PHDTHESIS")!=NULL) 
	           fprintf(outptr,"  <TYPE>PHDTHESIS</TYPE>\n");
	else {
		fprintf(stderr,"bib2xml: defaulting to type ARTICLE.  Cannot identify type for:\n");
		fprintf(stderr,"%s\n",buffer);
		fprintf(outptr,"  <TYPE>ARTICLE</TYPE>\n");
	}

	for (i=0; i<NUMFIELDS; ++i) {
		if ( (p=strsearch(buffer,field[i])) != NULL ) {

			s = extract_field(p);

			newstr_findreplace (s,"\\it ","");
			newstr_findreplace (s,"\\em ","");
			newstr_findreplace (s,"\\%","%");
			newstr_findreplace (s,"\\$","$");
			newstr_findreplace (s,"\\&","&");

			if (s->data!=NULL) {
				if (i==0) {     /* Name */
				    char *q;
				    newstring *name;
				    fprintf(outptr,"  <AUTHORS>\n");
				    newstr_findreplace(s," and ","|");
				    q=s->data;
				    while (extract_name(&q,&name)) {
				      if (name!=NULL && name->data!=NULL)
				        fprintf(outptr,"    <AUTHOR>%s</AUTHOR>\n",name->data);
				        if (name!=NULL) free(name);
				    }
				    fprintf(outptr,"  </AUTHORS>\n");
				} else if (i==5) {  /* Pages */
				    newstring *sp, *ep;
				    fprintf(outptr,"  <PAGES>");
				    extract_pages(s->data,&sp,&ep);
				    if (sp->data!=NULL) fprintf(outptr,
				       "<START>%s</START>",sp->data);
				    if (ep->data!=NULL) fprintf(outptr,
				       "<END>%s</END>",ep->data);
				    newstr_clear(sp);
				    newstr_clear(ep);
				    free(sp);
				    free(ep);
				    fprintf(outptr,"</PAGES>\n");
				} else {
				    fprintf(outptr,"  <%s>%s</%s>\n",
					field[i],s->data,field[i]);
				}
       			}

         newstr_clear(s);

         free(s);
      }
   }
   
   /*
    * Put reference number in Notes field
    */
   if ( (p=strsearch(buffer,"@")) != NULL ) {
     s = extract_refnum(p);
     if (s->data!=NULL) fprintf(outptr,"  <REFNUM>%s</REFNUM>\n",s->data);
     newstr_clear(s);
     free(s); /* Calling function must free s as well */
   }

   fprintf(outptr,"</REF>\n");

   fflush(outptr);

 }

/*
 *   read_refs()
 *
 *         Reads references one at a time into buffer
 *         and send to process.
 */
long read_refs(FILE *inptr, FILE *outptr)
{
  newstring buffer;
  char line[256],*errorptr,*ptr;
  int haveref = FALSE, processref = FALSE;
  long numrefs =0L;

  newstr_init(&buffer);

  fprintf(outptr,"<XML>\n");
  fprintf(outptr,"<REFERENCES>\n");

  while (!feof(inptr)) {
      errorptr = fgets (line, sizeof(line), inptr);

      if (errorptr != NULL) {
          ptr = line;
          while (iswhitespace(*ptr)) ptr++;
          if (*ptr == '@') {
              if (haveref) processref = TRUE;
              else haveref = TRUE;
          }
      }
      else if (haveref) processref = TRUE;

      if (processref) {
          process_article(outptr,buffer.data);
          newstr_clear(&buffer);
          processref = FALSE;
          numrefs++;
      }

      if (haveref) {
          newstr_strcat(&buffer,ptr);
      }

  }

  fprintf(outptr,"</REFERENCES>\n");
  fprintf(outptr,"</XML>\n");

  newstr_clear (&buffer);
  return numrefs;
}


int 
main(int argc, char *argv[])
{
	FILE *outptr,*inptr;
	long numrefs;
	int i;

	inptr = stdin;
	outptr = stdout;

	if (argc==1) {
		numrefs = read_refs(inptr,outptr);
	} else {
		numrefs = 0;
		for (i=1; i<argc; ++i) {
			inptr = fopen( argv[i], "r" );
			if (inptr!=NULL) {
				numrefs += read_refs(inptr,outptr);
				fclose(inptr);
			} else {
				fprintf(stderr,"bib2xml: cannot open %s\n",
					argv[2]);
			}
		}
	}

	fprintf(stderr,"bib2xml:  Processed %ld references.\n",numrefs);
	return EXIT_SUCCESS;
}


