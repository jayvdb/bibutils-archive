/*---------------------------------------------------------------------------

bib2en  --   BibTeX to Tab-Delineated Pre-EndNote format


Author:  Chris Putnam (cdputnam@scripps.edu, http://www.scripps.edu/~cdputnam/)


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

#define NUMFIELDS (6)

void process_article (FILE *outptr, char *buffer)
{
   char *field[NUMFIELDS]={"AUTHOR","YEAR","TITLE","JOURNAL",
                  "VOLUME","PAGES"};
   int i;
   char *p;
   newstring *s;

   for (i=0; i<NUMFIELDS; ++i) {
      if ( (p=search(buffer,field[i])) != NULL ) {

         s = extract_field(p);

         newstr_findreplace (s,"\\it ","");
         newstr_findreplace (s,"\\em ","");
         newstr_findreplace (s,"\\%","%");
         newstr_findreplace (s,"\\$","$");
         newstr_findreplace (s,"\\&","&");

         if (i!=0) {  /* Not Name */
             if (s->data!=NULL) fprintf(outptr,"%s\t",s->data);
             else fprintf(outptr,"\t");
         }
         else {
            if (s->data!=NULL) {
               newstr_findreplace (s," and ","//");
               fprintf(outptr,"%s\t",s->data);
            }
            else fprintf(outptr,"\t");
         }
         newstr_clear(s);
         free(s);
      }
      else fprintf(outptr,"\t");
   }
   
   /*
    * Put reference number in Notes field
    */
   if ( (p=search(buffer,"@")) != NULL ) {
     s = extract_refnum(p);
     if (s->data!=NULL) fprintf(outptr,"%s\n",s->data);
     else fprintf(outptr,"\n");
     newstr_clear(s);
     free(s); /* Calling function must free s as well */
   }

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
     notify("bib2en\nChristopher D. Putnam\nVersion 1.0  April 1999\n\n");
     notify("Usage:  bib2en <bibtex file> <output file>\n\n");
     notify("bib2ris converts a bibtex-formatted reference file to a tab-formatted\n");
     notify("reference file for importing into EndNote.  The bibtex citation codes are\n");
     notify("saved in the Notes field.  Conversion of names including a\n");
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
     fprintf(outptr,"*Journal Article\n");
     fprintf(outptr,"Author\tYear\tTitle\tJournal\tVolume\tPages\tNotes\n");
     numrefs = read_refs(inptr,outptr);
     fclose(inptr);
     fclose(outptr);
     fprintf(stderr,"bib2en:  Processed %ld references.\n",numrefs);
   }
   return 0;
 }


