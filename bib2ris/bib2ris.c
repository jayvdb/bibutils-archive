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

#define TRUE (1==1)
#define FALSE (!TRUE)

 void strip_spaces(str)
   char *str;
 {
   char *p,*q;
   p=str;
   q=str;
   while (*(p-1)) {
     if (*p!=' ') *q++=*p;
     p++;
   }
 }

/*
 * Search is a case-independent version of strstr()
 * it returns NULL on not finding target in buffer,
 * otherwise it returns the pointer to the position
 * in buffer.
 */
char *search (buffer,target)
  char* buffer;
  char* target;
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

char *extract_quotes(str)
   char *str;
{
   char *inquotes;
   char *newptr;
   char *p;
   int quotes;
   long counter;
   long arraysize=256;

   /* Allocate a 256 byte buffer for the string to be stored in the
    * inquotes buffer.  Keep track of how big the string is getting.
    */

   inquotes = (char *) malloc (arraysize * sizeof(char));
   if (inquotes==NULL) {
     fprintf(stderr,"Cannot allocate memory in extract_quotes.\n");
     exit(1);
   }

   /* Copy everything within the quotes in str into the inquotes array.
    * Keep track of how big the information's getting.  We'll reallocate
    * memory as needed.
    */
   
   quotes=0;
   counter=0;
   p=str;
   while (quotes<2 && *p)
     {
     if (*p=='\"' && *(p-1)!='\\') quotes++;
     else if (quotes==1) 
       {
       inquotes[counter] = *p;
       counter++;
       if (counter==arraysize) 
         {
         newptr = (char *) realloc (inquotes, arraysize*2*sizeof(char));
         if (newptr==NULL) 
           {
           fprintf(stderr,"Cannot reallocate memory in extract_name.\n");
           exit(1);
           }
         inquotes=newptr;
         arraysize*=2;
         }
       } 
     p++;
     } 
   inquotes[counter]='\0';
   return inquotes;
}

void sort_name(str)
   char *str;
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

 void extract_name(outptr,buffer,code)
   FILE *outptr;
   char *buffer,*code;
 {
   char *namefield;
   char *name;
   char *newptr;
   char *p,*q,*end;
   int quotes;
   long counter;
   int arraysize=256;

   namefield = extract_quotes(buffer);

  /*   Separate the individual elements of the namefield into
   *   the name space.  Keep reallocating memory as required.
   *
   */

   arraysize=256;
   name = (char *) malloc ( arraysize * sizeof(char));
   if (name==NULL) {
     fprintf(stderr,"Cannot allocate memory in extract_name.\n");
     exit(1);
   }
   p=namefield;
   do 
     {
     counter=0;
     end=search(p," and ");
     while (p!=end && *p!=0) 
       {
       name[counter++]=*p++;
       if (counter==arraysize) 
         {
         newptr = realloc (name, arraysize * 2 * sizeof(char));
         if (newptr==NULL)
           {
           fprintf(stderr,"Cannot reallocate memory in extract_name.\n");
           exit(1); 
           }
         name=newptr;
         arraysize*=2;
         }
       }
     name[counter]='\0';
     sort_name(name);
     fprintf(outptr,"%s  - %s\r",code,name);
     if (*p) p+=4;
     } 
   while (*p);

   free (name);
   free (namefield);
 }

 void extract_easy(outptr,buffer,code)
   FILE *outptr;
   char *buffer,*code;
 {
   char *data;
   data = extract_quotes(buffer);
   fprintf(outptr,"%s  - %s\r",code,data);
   free (data);
}

 void extract_num(outptr,buffer,code)
   FILE *outptr;
   char *buffer,*code;
 {
   char *number;
   char *newptr;
   char *p;
   int i,quotes;
   long counter=0;
   long arraysize=256;
   
   /*  Allocate a block of memory to store the numbers in.
    */
 
   number = (char *) malloc ( arraysize * sizeof(char));
   if (number==NULL) {
     fprintf(stderr,"Cannot allocate memory in extract_num.\n");
     exit(1);
   }

   /* Now put everything after the '=', but before the ',' into
    * our string.
    */

   p=buffer;
   counter=0;
   quotes=0;
   while (quotes<2 && *p) 
     {
     if (*p=='=' || *p==',') quotes++;
     else if (quotes==1) 
       {
       number[counter++]=*p;
       if (counter==arraysize) 
         {
         newptr = (char *) realloc (number, sizeof(char)*2*arraysize);
         if (newptr==NULL)
           {
        fprintf(stderr,"Error.  Cannot reallocate memory in extract_number.\n");
           exit(1);
           }
         number=newptr;
         arraysize*=2;
         }
       }
     p++;
     } 
   number[counter]='\0';
   
   for (i=0; i<strlen(number); i++)
     if ((number[i]<'0' || number[i]>'9') &&
     (toupper(number[i])<'A' || toupper(number[i])>'Z')) number[i]=' ';
   strip_spaces(number);
   fprintf(outptr,"%s  - %s\r",code,number);
   free(number);
 }

 void extract_pages(outptr,ptr)
   FILE *outptr;
   char *ptr;
 {
   char num1[20],num2[20],*p,*q;
   char *pagesfield;

   pagesfield=extract_quotes(ptr);
   if (pagesfield!=NULL) {
     q=num1;
     p=strpbrk(pagesfield,"0123456789");
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
}

void refnum2keyword(outptr,ptr)
  FILE *outptr;
  char *ptr;
{
  char *p,*keyword,*newptr; 
  int counter;
  int arraysize=256;

  /*  First dynamically allocate the memory for the keyword[]
   *  array.  We'll probably never have to worry about it, but it
   *  will increase as needed.
   */

  keyword = (char *) malloc ( arraysize * sizeof(char));
  if (keyword==NULL) {
    fprintf(stderr,"Error.  Cannot allocate memory in refnum2keyword.\n");
    exit(1);
  }


  p=ptr+1;
  counter=0;
  while (*p && *p!=',')
    {
    keyword[counter++]=*p++;
    if (counter==arraysize)
      {
      newptr = (char *) realloc (keyword, arraysize * sizeof(char) * 2 );
      if (newptr==NULL)
        {
        fprintf(stderr,"Error.  Cannot reallocate memory in refnum2keyword.\n");
        exit(1);
        }
      keyword=newptr;
      arraysize*=2;
      } 
    }
  keyword[counter]='\0';
  fprintf(outptr,"KW  - %s\r",keyword);
  free(keyword);
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


 void read_ref (inptr,outptr,pos,num_lines)
   FILE *inptr,*outptr;
   long pos;
   int num_lines;
 {
   char *buffer,*p,*q,*errorptr,
        *begin,*end,line[81];
   int mod,found,i,ok,error,j;

   buffer=(char *)malloc(80*num_lines*sizeof(char));
   if (buffer==NULL) {
     fprintf(stderr,"Cannot allocate memory for reference!\n\n");
     fclose(inptr);
     fclose(outptr);
     exit(-1);
   }
   buffer[0]=0;

	error=fseek(inptr,pos,0);
	if (error) {
	  fprintf(stderr,"Error!  Cannot set pointer in data file.\n");
          fclose(outptr);
	  fclose(inptr);
	  exit (-1);
	}

   for (i=0; i<num_lines; i++) {
     errorptr=fgets(line,80,inptr);
     if (errorptr==NULL && !feof(inptr)) {
       fprintf(stderr,"Error!  Cannot read from data file in read_ref.\n");
       fclose(outptr);
       fclose(inptr);
       exit(-1);
     }
     ok=0;
     for (j=0; j<80; j++) {
       if (line[j]=='%') line[j]=0;
       if (line[j]=='\r' || line[j]=='\n') line[j]=' ';
       if (line[j]==0) ok=1;
     }
     if (!ok) line[80]=0;
     strcat(buffer,line);
   }

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

     /*
      * Determine the type of reference.
      */

   if (search(buffer,"ARTICLE")!=NULL){
     fprintf(outptr,"\nTY  - JOUR\n");
     if ((p=search(buffer,"TITLE="))!=NULL) extract_easy(outptr,p,"TI");
     if ((p=search(buffer,"JOURNAL="))!=NULL) extract_easy(outptr,p,"JO");
     if ((p=search(buffer,"VOLUME="))!=NULL) extract_num(outptr,p,"VL");
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
   free(buffer); 
   fprintf(outptr,"ER  - \n");
   fflush(outptr);
 }

     /*
      * scanfor_refs()
      *
      * This function scans for a Bibtex reference
      * starting with the '@' character as the first
      * non-space character in a line.
      *
      */

 void scanfor_refs(inptr,outptr)
   FILE *inptr,*outptr;
 {
    long pos;
    char line[81],*ptr,*errorptr;
    int num_lines,quotations,done,error;
    while (!feof(inptr)) {

	/*
	 * Put a file pointer to the beginning of the article.
	 */
      pos=ftell(inptr);

	/*
	 * Get one line of the file and put ptr to the first non-space
	 * character.
	 */

      errorptr=fgets (line,80,inptr);
      if (errorptr==NULL) {
        if (feof(inptr)) exit(0);
        else {
        fprintf(stderr,"Error.  Cannot read input file in scan_for_refs.\n");
        fclose(outptr);
        fclose(inptr);
        exit(-1);
        }
      }
      ptr=line;
      while (*ptr==' ') ptr++;

	/*
	 * If the character is a '@', take it to be the
	 * beginning of a new reference.  Read the number of
	 * lines between the '@' and the terminal ')'.  Make
	 * sure that the program ignores all ')' within
	 * quotation marks.  Send the output to read_ref() for
	 * conversion.
	 */

      if (*ptr=='@') {
	error=fseek(inptr,pos,0);
	if (error) {
	  fclose(outptr);
	  fclose(inptr);
	  fprintf(stderr,"Error!  Cannot set pointer in data file.\n");
	  exit (-1);
	}
	done=0;
	quotations=0;
	num_lines=0;
	while (!feof(inptr) && !done) {
	  errorptr=fgets(line,80,inptr);
	  if (errorptr==NULL) {
            fprintf(stderr,"Error.  Cannot read from input file in scan_for_refs #2.\n");
            fprintf(stderr,(feof(inptr))?"At end of file!":"not at end");
            fclose(outptr);
            fclose(inptr);
            exit(-1);
          }
          ptr=line;
	  while (*ptr) {
	    if (*ptr=='\"' && *(ptr-1)!='\\') quotations++;
	    if (*ptr==')' && (quotations%2==0)) done=1;
	    ptr++;
	  }
	  num_lines++;
	}
	read_ref(inptr,outptr,pos,num_lines);
      }
    }
 }

 int main(argc,argv)
   int argc;
   char *argv[];
 {
   char infile[255],outfile[255];
   FILE *outptr,*inptr;
   if (argc!=3) {
   fprintf(stderr,"bib2ris\nChristopher D. Putnam\nVersion 1.1  July 1996\n");

   fprintf(stderr,"\n\nUsage:  bib2ris <bibtex file> <output file>\n");
   fprintf(stderr,"\nbib2ris converts a bibtex-formatted reference file to an RIS formatted\nreference file for ease in document conversion.  The bibtex citation codes are\nsaved in the keyword (KW) field.  Note that conversion of names including a \nsuffix like Jr. or II may not be formatted correctly.");
   fprintf(stderr,"\n\nNote that the RIS standard requires a \\r character between the lines,\nunlike the standard UNIX \n character.  Hence, simply displaying the file via\nmore won't make the file look good on the screen, but it should be ok.\n\n");
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
     scanfor_refs(inptr,outptr);
     fclose(inptr);
     fclose(outptr);
   }
   return 0;
 }
