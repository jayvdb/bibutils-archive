/*
 * Bibtex to RIS translator
 *
 * C. Putnam  October, 1995
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
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

void extract_quotes(char *buf,newstring *inquotes)
{
   int quotes;
fprintf(stderr,"begin extract_quotes\n");
   newstr_init(inquotes);
   quotes=0;
   while (quotes<2 && *buf)
     {
     if (*buf=='\"' && *(buf-1)!='\\') quotes++;
     else if (quotes==1) newstr_addchar(inquotes,*buf);
     buf++;
     } 
fprintf(stderr,"end extract_quotes\n");
}

void sort_name(char *str)
 {
   char *name, *newname, **piece, **newpiece, *q;
   int name_arraysize=256;
   int piece_arraysize=32;
   int piece_counter;
   int name_counter;
   int k,active=0;

   /* Dynamically allocate both the name[] and p[] arrays.
    */
   
   name = (char *)  malloc ( name_arraysize * sizeof(char));
   piece= (char **) malloc ( name_arraysize * sizeof(char*));
   if (name==NULL || piece==NULL) {
     fprintf(stderr,"Error.  Cannot allocate memory in sort_name.\n");
     exit(1);
   }

   /* Position each element of the p array to each element
    * that's in name.
    */

   piece_counter=0;
   q=str;
   active=1;
   while (*q) 
     {
     if (active && *q!='.' && *q!=' ') 
       {
       piece[piece_counter++]=q;
       if (piece_counter==piece_arraysize) 
         {
         newpiece = (char **) malloc ( piece_arraysize * 2 * sizeof(char *));
         if (newpiece==NULL)
           {
           fprintf(stderr,"Error.  Cannot reallocate memory in sort_name.\n");
           exit(1);
           }
         piece = newpiece;
         piece_arraysize *=2;
         }
       active=0;
       }
     if (!active && (*q=='.' || *q==' ')) active=1;
     q++;
     }
   piece_counter--;
   q=piece[piece_counter];
   name_counter=0;

   while (*q && *q!=' ' && *q!='.')
     { 
     name[name_counter++]=*q++;
     if (name_counter==name_arraysize) 
        {
        newname = (char *) realloc (name, name_arraysize*2 * sizeof(char));
        if (newname==NULL) 
          {
          fprintf(stderr,"Error.  Cannot reallocate memory in sort_name.\n");
          exit(1);
          }
        name = newname;
        name_arraysize*=2;
        }
      }
   
   if (*q=='.')
      {   
     name[name_counter++]='.';
     if (name_counter==name_arraysize) 
        {
        newname = (char *) realloc (name, name_arraysize*2 * sizeof(char));
        if (newname==NULL) 
          {
          fprintf(stderr,"Error.  Cannot reallocate memory in sort_name.\n"); 
          exit(1); 
          }
        name = newname;  
        name_arraysize*=2; 
        } 
      } 
  
   if (piece_counter>0)
     {   
     name[name_counter++]=',';
     if (name_counter==name_arraysize) 
        {
        newname = (char *) realloc (name, name_arraysize*2 * sizeof(char));
        if (newname==NULL) 
          {
          fprintf(stderr,"Error.  Cannot reallocate memory in sort_name.\n"); 
          exit(1); 
          }
        name = newname;  
        name_arraysize*=2; 
        } 
      } 

   for (k=0; k< piece_counter; k++) {
     q=piece[k];
     while (*q && *q!=' ' && *q!='.') 
     {   
     name[name_counter++]=*q++;
     if (name_counter==name_arraysize) 
        {
        newname = (char *) realloc (name, name_arraysize*2 * sizeof(char));
        if (newname==NULL) 
          {
          fprintf(stderr,"Error.  Cannot reallocate memory in sort_name.\n"); 
          exit(1); 
          }
        name = newname;  
        name_arraysize*=2; 
        } 
      } 
     if (*q=='.')
     {   
     name[name_counter++]='.';
     if (name_counter==name_arraysize) 
        {
        newname = (char *) realloc (name, name_arraysize*2 * sizeof(char));
        if (newname==NULL) 
          {
          fprintf(stderr,"Error.  Cannot reallocate memory in sort_name.\n"); 
          exit(1); 
          }
        name = newname;  
        name_arraysize*=2; 
        } 
      } 
   }
   name[name_counter]='\0';
   strcpy (str,name);
   free (name);
   free (piece);
 }

 void extract_name(FILE *outptr,char *buffer,char *code)
 {
   newstring namefield;
   newstring name;
   char *newptr;
   char *p,*q,*end;
   int quotes;
   long counter;
   int arraysize=256;

   extract_quotes(buffer,&namefield);
   newstr_init(&name);

   p=namefield.data;
   do 
     {
     counter=0;
     end=search(p," and ");
     while (p!=end && *p!=0) 
       newstr_addchar(&name,*p++);
     sort_name(name.data);
     fprintf(outptr,"%s  - %s\r",code,name.data);
     if (*p) p+=4;
     } 
   while (*p);

   newstr_clear(&name);
   newstr_clear(&namefield);
 }

 void extract_easy(FILE *outptr,char *buffer,char *code)
 {
   newstring data;
   extract_quotes(buffer,&data);
   fprintf(outptr,"%s  - %s\r",code,data.data);
   newstr_clear (&data);
}

void extract_num(FILE *outptr,char *buffer,char *code)
{
   newstring number;
   char *newptr;
   char *p;
   int i,quotes;
   

fprintf(stderr,"Started extract_num\n");
   newstr_init(&number);

   /* Now put everything after the '=', but before the ',' into
    * our string.
    */
fprintf(stderr,"Got the string start: %s\n",buffer);
   p=buffer;
   quotes=0;
   while (quotes<2 && *p) 
     {
     if (*p=='=' || *p==',') quotes++;
     else if (quotes==1) newstr_addchar(&number,*p);
     p++; 
     } 

fprintf(stderr,"extracted bit between = and ,\n");
   
   for (i=0; i<strlen(number.data); i++)
     if ((number.data[i]<'0' || number.data[i]>'9') &&
     (toupper(number.data[i])<'A' || toupper(number.data[i])>'Z')) 
         number.data[i]=' ';
   strip_spaces(number.data);
   fprintf(outptr,"%s  - %s\r",code,number.data);
   newstr_clear(&number); 
}

 void extract_pages(FILE *outptr,char *ptr)
 {
   char num1[20],num2[20],*p,*q;
   newstring pagesfield;

   extract_quotes(ptr,&pagesfield);
   if (pagesfield.data!=NULL) {
     q=num1;
     p=strpbrk(pagesfield.data,"0123456789");
     if (p==NULL) return; 
     while (*p>='0' && *p<='9') *q++=*p++;
     *q=0;
     fprintf(outptr,"SP  - %s\r",num1);
  
   /*
    * If getting first set of pages worked,
    * let's go for set #2!
    */

    p=strpbrk(p,"0123456789");
    if (p!=NULL) {
      q=num2;
      while (!(*p<'0' || *p>'9')) *q++=*p++;
      *q=0;
      fprintf(outptr,"EP - %s\r",num2);
    }
  }
  newstr_clear(&pagesfield);
}

void refnum2keyword(FILE *outptr, char *buf)
{
  newstring keyword;

  newstr_init(&keyword);
  buf++;
  while (*buf && *buf!=',') newstr_addchar(&keyword,*buf++);
  fprintf(outptr,"KW  - %s\r",keyword.data);
  newstr_clear(&keyword);
}

    /*
     * read_ref()
     *
     * This function starts at the beginning of the reference
     * and reads the whole thing line by line.
     * The function then converts the individual reference to
     * RIS format and exits.
     *
     */

void repl_brackets (char *buffer)
{
   char *p,*q,*begin,*end;
   int found=FALSE,mod;
    /*
     * Strip out unnecessary TeX brackets!
     */

    p=buffer;
    do {
      begin = strchr (p, '{');
      end   = strchr (p, '}');
      if (begin!=NULL && end!=NULL && end>begin) {
	found=0;
	q=begin+1;
	while (q<end) {
	  if (*q=='\\' || *q=='$') found=1;
	  q++;
	}
	if (!found) {
	  mod=-1;
	  q=begin+1;
	  while (*(q-1)) {
	    *(q+mod) = *q;
	    q++;
	    if (q==end) {
	      q++;
	      mod=-2;
	    }
	  }
	}
	p=end+1;
      }
    } while (begin!=NULL && end!=NULL & end > begin);
}

void repl_ampers (char *buffer)
{
  char *p,*begin;
    /*
     * Replace \& codes with &
     */

    p=buffer;
    while ((begin=(char *)strstr(buffer,"\\&"))!=NULL) {
      p=begin+1;
      while (*(p-1)) {
	*(p-1)=*p;
	p++;
      }
      p=begin+1;
    }
}

void process_ref (FILE *outptr, char *buffer)
{
   char *p;

fprintf(stderr,"Recieved this buffer: %s\n ",buffer);
/*
fprintf(stderr,"Started process_ref\n");
   repl_brackets(buffer);
fprintf(stderr,"Replaced brackets\n");
   repl_ampers(buffer);
fprintf(stderr,"Replaced ampers\n");
fprintf(stderr,"Ended Replacements\n");
*/     /*
      * Determine the type of reference.
      */
fprintf(stderr,"About to search \n");
if (search(buffer,"ARTICLE")) fprintf(stderr,"found ARTICLE \n");
else fprintf(stderr,"couldn't find ARTICLE\n");

   if (search(buffer,"ARTICLE")!=NULL){
     fprintf(outptr,"\nTY  - JOUR\n");
     if ((p=search(buffer,"TITLE="))!=NULL) extract_easy(outptr,p,"TI");
fprintf(stderr,"done with Title\n");
     if ((p=search(buffer,"JOURNAL="))!=NULL) extract_easy(outptr,p,"JO");
fprintf(stderr,"done with Journal\n");
     if ((p=search(buffer,"VOLUME="))!=NULL) extract_num(outptr,p,"VL");
fprintf(stderr,"done with ARTICLE\n");
   }
   else if (search(buffer,"INBOOK")!=NULL){
     fprintf(outptr,"\nTY  - CHAP\n");
     if ((p=search(buffer,"TITLE="))!=NULL) extract_easy(outptr,p,"CT");
     if ((p=search(buffer,"BOOKTITLE="))!=NULL) extract_easy(outptr,p,"BT");
     if ((p=search(buffer,"EDITOR="))!=NULL) extract_name(outptr,p,"ED");
     if ((p=search(buffer,"PUBLISHER="))!=NULL) extract_easy(outptr,p,"PB");
     if ((p=search(buffer,"ADDRESS="))!=NULL) extract_easy(outptr,p,"CT");
     if ((p=search(buffer,"CHAPTER="))!=NULL) extract_num(outptr,p,"CP");
   }
   else if (search(buffer,"BOOK")!=NULL || strstr(buffer,"PHDTHESIS")!=NULL){
     fprintf(outptr,"\nTY  - BOOK\n");
     if ((p=search(buffer,"BOOKTITLE="))!=NULL) extract_easy(outptr,p,"BT");
     if ((p=search(buffer,"EDITOR="))!=NULL) extract_name(outptr,p,"ED");
     if ((p=search(buffer,"PUBLISHER="))!=NULL) extract_easy(outptr,p,"PB");
     if ((p=search(buffer,"ADDRESS="))!=NULL) extract_easy(outptr,p,"CT");
   }
   /*
    * Search for general portions in the buffer.
    */

   if ((p=search(buffer,"AUTHOR="))!=NULL) extract_name(outptr,p,"AU");
   if ((p=search(buffer,"PAGES="))!=NULL) extract_pages(outptr,p);
   if ((p=search(buffer,"("))!=NULL) refnum2keyword(outptr,p);
   if ((p=search(buffer,"YEAR="))!=NULL) extract_num(outptr,p,"PY");


    /*
     * End it.
     */
   fprintf(outptr,"ER  - \n");
   fflush(outptr);
fprintf(stderr,"Ended process_ref\n");
 }

void read_refs(FILE *inptr, FILE *outptr)
{
  newstring buffer;
  char line[81],*errorptr,*ptr;
  int haveref = FALSE;

fprintf(stderr,"Started read_refs\n");
  newstr_init(&buffer);
  while (!feof(inptr)) {
    errorptr = fgets (line, sizeof(line), inptr);
fprintf(stderr,"Line = %s\n",line);
if (buffer.data!=NULL) fprintf(stderr,"Buffer = %s\n",buffer.data);
else
fprintf(stderr,"buffer.data = NULL \n");
    if (errorptr == NULL && haveref) process_ref (outptr, buffer.data);
    if (errorptr == NULL) {
      newstr_clear(&buffer);
      return;
    }
    ptr = line;
    while (iswhitespace(*ptr)) ptr++;
    if (*ptr=='@') {
      if (haveref) {
        process_ref(outptr,buffer.data);
        newstr_strcpy(&buffer,ptr);
      }
      else {
        haveref = TRUE;
        newstr_strcat(&buffer,ptr);   
      }
    }
    else if (haveref) newstr_strcat (&buffer,line);
  }
  newstr_clear(&buffer);
}
  

 int main(argc,argv)
   int argc;
   char *argv[];
 {
   char infile[255],outfile[255];
   FILE *outptr,*inptr;
   if (argc!=3) {
     notify("bib2ris\nChristopher D. Putnam\nVersion 1.1  July 1996\n\n");
     notify("Usage:  bib2ris <bibtex file> <output file>\n\n");
     notify("bib2ris converts a bibtex-formatted reference file to an RIS formatted\n");
     notify("reference file for ease in document conversion.  The bibtex citation codes are\n");
     notify("saved in the keyword (KW) field.  Note that conversion of names including a\n");
     notify("suffix like Jr. or II may not be formatted correctly.\n\n");
     notify("Note that the RIS standard requires a \\r character between the lines,\n");
     notify("unlike the standard UNIX \\n charactere  Hence, simply displaying the file via\n");
     notify("more won't make the file look good on the screen, but it should be ok.\n\n");
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
printf("Opened in file and outfile.\n");
     read_refs(inptr,outptr);
     fclose(inptr);
     fclose(outptr);
   }
   return 0;
 }
