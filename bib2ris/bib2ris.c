/*---------------------------------------------------------------------------

bib2ris --   BibTeX to RIS format Pre-Reference Manager 

---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

/*
 * Search is a case-independent version of strstr()
 * it returns NULL on not finding target in buffer,
 * otherwise it returns the pointer to the position
 * in buffer.
 */
char *search (char *buffer, char *target)
{
  char *pbuf,*ptar,*returnptr=NULL;
  int found=FALSE;
  int pos=0;
  pbuf=buffer;
  ptar=target;
  while (*ptar && *pbuf && !found) {

    while ((*pbuf) && (*(pbuf+pos)) && (*(ptar+pos)) && (!found)) {
      if (toupper(*(pbuf+pos))==toupper(*(ptar+pos))) {
        pos++;
        if (*(ptar+pos)=='\0') {
          found=TRUE;
          returnptr=pbuf;
        }
        else if (*(pbuf+pos)=='\0') {
          return NULL;
        }
      }
      else {
        pos=0;
        pbuf++;
      }
    }

  }
  return returnptr;
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

int extract_name(char **buffer, newstring **s)
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
       if (search(prename->data," Jr.")) { newstr_findreplace(prename," Jr."," "); junior=TRUE; }
       if (search(prename->data," III")) { newstr_findreplace(prename," III"," "); third=TRUE; }
       element = 0;
       q = strtok(prename->data," \t");
       while ( q != NULL ) {
          name[element++]=q;
          if (element == MAXELEMENT) element=MAXELEMENT-1;
          q = strtok(NULL," \t"); 
          if (q==NULL) {
             if (element-1>0) {
                newstr_strcpy(postname,name[element-1]);
                if (junior) newstr_strcat(postname," Jr.");
                if (third)  newstr_strcat(postname," III");
                if (element>1) newstr_strcat(postname,", ");
                for (i=0; i<element-1; ++i) {
                       newstr_strcat(postname,name[i]);
                       newstr_addchar(postname,' ');
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

newstring* extract_refnum (char *buffer)
{
   char *ptr = buffer;
   int  quotes = FALSE,
        infield = TRUE;
   newstring *s;

   s = (newstring *) malloc (sizeof(newstring));
   newstr_init(s);
   while (*(ptr-1) != '(' && *ptr !='\0') ptr++;
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


#define NUMFIELDS (12)

void process_article (FILE *outptr, char *buffer)
{
   char *field[NUMFIELDS]={"AUTHOR=", "YEAR=",   "TITLE=",    "JOURNAL=",
                           "VOLUME=", "PAGES=",  "EDITOR=",   "PUBLISHER=",
                           "ADDRESS=","CHAPTER=","BOOKTITLE=","EDITOR="
                          };
   char *descriptors[NUMFIELDS]={"AU","PY","TI","JO",
                                 "VL","","ED","PB",
                                 "CT","CP","BT","ED"
                                };
   int i;
   char *p;
   newstring *s;

   if (search(buffer,"@ARTICLE") != NULL )      fprintf(outptr,"\nTY  - JOUR\n");
   else if (search(buffer,"@INBOOK") != NULL || search(buffer,"INPROCEEDINGS")!=NULL )  
                                                fprintf(outptr,"\nTY  - CHAP\n");
   else if (search(buffer,"@BOOK") != NULL || search(buffer,"PHDTHESIS") != NULL ) 
                                                fprintf(outptr,"\nTY  - BOOK\n");

   for (i=0; i<NUMFIELDS; ++i) {
      if ( (p=search(buffer,field[i])) != NULL ) {

         s = extract_field(p);

         newstr_findreplace (s,"\\it ","");
         newstr_findreplace (s,"\\em ","");
         newstr_findreplace (s,"\\%","%");
         newstr_findreplace (s,"\\$","$");
         newstr_findreplace (s,"\\&","&");

         if (i==0) {  /* Name */
           if (s->data!=NULL) {
               char *q;
               newstring *name;
               newstr_findreplace (s," and ","|");
               q=s->data;
               while (extract_name(&q,&name)) {
                   if (name!=NULL && name->data!=NULL) fprintf(outptr,"%s  - %s\r",descriptors[i],name->data);
                   if (name!=NULL) free(name);
               }
            }
         }
         else if (i==5) { /* Pages */
            newstring *sp,*ep;
            extract_pages(s->data,&sp,&ep);
            if (sp->data!=NULL) fprintf(outptr,"SP  - %s\r",sp->data);
            if (ep->data!=NULL) fprintf(outptr,"EP  - %s\r",ep->data);
            newstr_clear(sp);
            newstr_clear(ep); 
            free(sp);
            free(ep);
         }
         else {  /* Not Name */
             if (s->data!=NULL) fprintf(outptr,"%s  - %s\r",descriptors[i],s->data);
         }
         newstr_clear(s);
         free(s);
      }
   }
   
   /*
    * Put reference number in Keyword field
    */
   if ( (p=search(buffer,"@")) != NULL ) {
     s = extract_refnum(p);
     if (s->data!=NULL) fprintf(outptr,"KW  - %s\r",s->data);
     newstr_clear(s);
     free(s); /* Calling function must free s as well */
   }

   fprintf(outptr,"ER  - \n");

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

   newstr_clear (&buffer);
   return numrefs;
}


 int main(int argc, char *argv[])
 {
   char infile[255],outfile[255];
   FILE *outptr,*inptr;
   if (argc!=3) {
     notify("bib2ris\nChristopher D. Putnam\nVersion 1.1  May 1999\n\n");
     notify("Usage:  bib2ris <bibtex file> <output file>\n\n");
     notify("bib2ris converts a bibtex-formatted reference file to a RIS-formatted\n");
     notify("reference file for importing into Reference Manager.  The bibtex citation codes are\n");
     notify("saved in the Keyword field.  Conversion of names including a\n");
     notify("suffix like Jr. or II may not be formatted correctly.\n\n");
   }
   if (argc<2) {
     fprintf(stderr,"Enter bibtex filename:  ");
     fscanf(stdin,"%s",infile);
   }
   else strcpy (infile,argv[1]);
   if (argc<3) {
     fprintf(stderr,"Enter output filename:  ");
     fscanf(stdin,"%s",outfile);
   }
   else strcpy (outfile,argv[2]);
   if ((inptr = fopen (infile,"r"))==NULL) {
     fprintf(stderr,"Cannot open %s.\n\n",infile);
     exit (-1);
   }

   else if ((outptr = fopen (outfile,"w"))==NULL) {
     fprintf(stderr,"Cannot open %s.\n\n",outfile);
     exit (-1);
   }

   else {
     long numrefs;
     numrefs = read_refs(inptr,outptr);
     fclose(inptr);
     fclose(outptr);
     fprintf(stderr,"bib2ris:  Processed %ld references.\n",numrefs);
   }
   return 0;
 }


