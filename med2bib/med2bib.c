/*
 * medline to bibtex
 *
 */

#include <stdio.h>
#include <string.h>
#include "newstr.h"
#include "findrepl.h"

#define TRUE  1
#define FALSE 0
#define FIELD_OTHER    0
#define FIELD_AUTHORS  1
#define FIELD_TITLE    2
#define FIELD_SOURCE   3
#define FIELD_ABSTRACT 4

int abstractout = FALSE;

int whitespace (char ch)
{
  if (ch==' ' || ch=='\t' || ch=='\n' || ch=='\r') return TRUE;
  else return FALSE;
}

int endofline (char ch)
{
  if (ch=='\n' || ch=='\r') return TRUE;
  else return FALSE;
}

void output_abbrev (newstring *authorsptr, newstring *sourceptr)
{
  char *p;
  int pos;
  /** Output the first author's last name **/
fprintf(stderr,"output_abbrev last name %s\n",authorsptr->data);
fprintf(stderr,"output_abbrev last year %s\n",sourceptr->data);
  p=authorsptr->data;
  if (p!=NULL) {
    while (*p && whitespace(*p)) p++;
    while (*p && !whitespace(*p)) {
      printf("%c",*p); 
      p++;
    }
  }
  /** Output the last two digits of the year **/
  p==NULL;
  if (sourceptr->data!=NULL) p=strchr(sourceptr->data,',');
  if (p!=NULL) { 
    p++;
    while (*p && whitespace(*p)) p++;
    pos=1;
    while (*p && !whitespace(*p) && pos<5) {
      if (pos>2) printf("%c",*p);
      p++;
      pos++;
    }
  }
}

void output_authors (newstring *authorsptr)
{
  char *p,*q;
  int firstauthor = TRUE;
  int junior,thesecond,thethird,senior;
  int pos;
  newstring lastname,initials;
  newstring block,wholename;

  printf(",\nAUTHOR=\"");

  newstr_init(&lastname);
  newstr_init(&initials);
  newstr_init(&block);
  newstr_init(&wholename);

fprintf(stderr,"output authors %s\n",authorsptr->data);
  p=authorsptr->data;
  if (p!=NULL) {

    /* For the entire list do... */
    while (*p) {
       /* For each author do... */
       while (*p && *(p-1)!='.') newstr_addchar(&wholename,*p++);
       junior=FALSE;
       senior=FALSE;
       thesecond=FALSE;
       thethird=FALSE;
       q=wholename.data;
       if (strstr(q," Jr.")!=NULL) {
         junior=TRUE;
         newstr_findreplace(&wholename," Jr","");
       }
       if (strstr(q," Sr.")!=NULL) {
         senior=TRUE;
         newstr_findreplace(&wholename," Sr","");
       }
       if (strstr(q," II.")!=NULL) {
         thesecond=TRUE;
         newstr_findreplace(&wholename," II","");
       }
       if (strstr(q," III.")!=NULL) {
         thethird=TRUE;
         newstr_findreplace(&wholename," III","");
       }
       while (*q && *q!='.') { 

           /* Ignore initial spaces */
           while (*q && whitespace(*q)) q++;
           /* Get next block */
           while (*q && *q!='.' && !whitespace(*q)) {
             newstr_addchar(&block,*q);
             q++;
           }
           /* Ignore next bit of white space */
           while (*q && whitespace(*q)) q++;

           if (block.data!=NULL && block.dim!=0) {
              /* If lastname is null, then put there */
              /* Otherwise, if we're at '.' put at firstname */
              if (lastname.data==NULL || lastname.dim==0)
                newstr_strcat(&lastname,block.data);
              else if (*q!='.') { 
                newstr_addchar(&lastname,' ');
                newstr_strcat(&lastname,block.data);
              }
              else newstr_strcat(&initials,block.data);

              /* Now free the block */
              newstr_clear(&block);
            }
      }

      if (lastname.data!=NULL && lastname.dim!=0) {
        if (!firstauthor) printf("\nand ");
        else firstauthor=FALSE;
        if (initials.data!=NULL) {
          for (pos=0; pos < strlen(initials.data); pos++)
            printf("%c. ",initials.data[pos]);
        }
        if (lastname.data!=NULL) {
          if (junior) newstr_strcat(&lastname," Jr.");
          if (senior) newstr_strcat(&lastname," Sr.");
          if (thesecond) newstr_strcat (&lastname," II");
          if (thethird) newstr_strcat(&lastname," III");
          printf("%s",lastname.data);
        }
        newstr_clear(&lastname);
        newstr_clear(&initials);
        newstr_clear(&wholename);
      }
      if (*p) p++;
    }
  }
  printf("\"");
} 

void output_title (newstring *titleptr)
{
  newstring title;
  char *p;
  newstr_init(&title);
fprintf(stderr,"output_title %s\n",titleptr->data);
  printf(",\nTITLE=\"");
    if (titleptr->data!=NULL && titleptr->dim!=0) {
      p=titleptr->data;
      /* Skip past initial whitespace */
      while (*p && whitespace(*p)) p++;
      /* Print out all non-return characters */
      while (*p) {
        if (!whitespace(*p)) {
          newstr_addchar(&title,*p);
          p++;
        }
        else {
          newstr_addchar(&title,' ');
          while (whitespace(*p)) p++;
        }
      }
      process_title(&title);
      printf("%s",title.data);
    }
  newstr_clear(&title);
  printf("\"");
}

void output_source (newstring *sourceptr)
{
  char *p,*savep;
  int pos;
  newstring journal;
  newstring pgbegin,pgend;
fprintf(stderr,"output_source %s\n",sourceptr->data);
fprintf(stderr,"output_journal %s\n",sourceptr->data);
    /** Output Journal **/
     printf(",\nJOURNAL=\"");
     p=sourceptr->data;
     newstr_init(&journal);
     if (p!=NULL) {
       while (*p && whitespace(*p)) p++;
       while (*p && *p!='.') {
         if (!whitespace(*p)) {
           newstr_addchar(&journal,*p);
           p++;
         }
         else {
           newstr_addchar(&journal,' ');
           while (whitespace(*p)) p++;
         }
       }
       process_journal(&journal);
       printf("%s",journal.data);
     }
     printf("\",\n");
     newstr_clear(&journal);
     savep=p;

fprintf(stderr,"output_volume %s\n",sourceptr->data);

    /** Output Volume **/
     p==NULL;
     printf("VOLUME=\"");
     if (sourceptr->data!=NULL) p=strchr(savep,'.');
     if (p!=NULL) {
       p++;
       while (*p && whitespace(*p)) p++;
       while (*p && !whitespace(*p) && *p!='(' && *p!=':' && *p!='.' && *p!=',') {
         printf("%c",*p);
         p++;
       } 
     }
    printf("\",\n");
    savep=p;

fprintf(stderr,"output_year %s\n",sourceptr->data);
    /** Output Year **/
     p==NULL;
     printf("YEAR=");
     if (sourceptr->data!=NULL) p=strchr(savep,',');
     if (p!=NULL) {
       p++;
       while (*p && whitespace(*p)) p++;
       while (*p && !whitespace(*p) && *p!=',' && *p!='.') {
         printf("%c",*p);
         p++;
       }
     }
     printf(",\n");

fprintf(stderr,"output_pages %s\n",sourceptr->data);
    /** Output Pages **/
     p==NULL;
     printf("PAGES=\"");
     newstr_init(&pgbegin);
     newstr_init(&pgend);
     if (sourceptr->data!=NULL) p=strchr(savep,':');
     if (p!=NULL) {
       while (*p && (whitespace(*p) || *p==':')) p++;
       while (*p && !whitespace(*p) && *p!=',' && *p!='-' && *p!='.') { 
         newstr_addchar(&pgbegin,*p);
         p++;
       }
       printf("%s",pgbegin.data);
       while (*p=='-' || whitespace(*p)) p++;
       while (*p && !whitespace(*p) && *p!=',') {
         newstr_addchar(&pgend,*p);
         p++;
       }
       if (pgend.data!=NULL && pgend.dim!=0) printf("--");
       if (strlen(pgend.data)>=strlen(pgbegin.data)) printf("%s",pgend.data);
       else {
         for (pos=0; pos<strlen(pgbegin.data)-strlen(pgend.data); pos++)
           printf("%c",pgbegin.data[pos]);
         printf("%s",pgend.data);
       }
     }
     printf("\"");
     newstr_clear (&pgbegin);
     newstr_clear (&pgend);
}

void output_abstract(newstring *abstractptr)
{
  char *p;
  printf(",\nABSTRACT=\"\n");
  p=abstractptr->data;
  while (*p) {
    if (*p!='\"') printf("%c",*p);
    p++;
  }
  printf("\"\n");
}

void output_fields (newstring *authorsptr, newstring *titleptr, 
                    newstring *sourceptr, newstring *abstractptr)
{
  printf("@ARTICLE(");
  /* Output the abbreviation */
    output_abbrev(authorsptr,sourceptr);
  /* Output the authors */ 
    output_authors(authorsptr);
  /* Output the title   */
    output_title(titleptr);
  /* Output the source */
    output_source(sourceptr);
  /* If requested, output the abstract */
    if (abstractout) output_abstract(abstractptr);
  printf(")\n\n");
}

void get_fields (FILE *fp)
{
  int field_id;
  newstring authors,title,source,abstract;
  char buf[512];

  field_id = FIELD_OTHER;
  newstr_init(&authors);
  newstr_init(&title);
  newstr_init(&source);
  newstr_init(&abstract);
  while (!feof(fp)) {
    if (fgets(buf,sizeof(buf),fp)) {
      if (buf[0]==' ' && buf[1]==' ') {
        if (field_id==FIELD_AUTHORS) newstr_strcat(&authors,buf);
        else if (field_id==FIELD_TITLE) newstr_strcat(&title,buf);
        else if (field_id==FIELD_SOURCE) newstr_strcat(&source,buf);
        else if (field_id==FIELD_ABSTRACT && abstractout) newstr_strcat(&abstract,buf);
      }
      else if (strstr(buf,"Authors")!=NULL) field_id=FIELD_AUTHORS;
      else if (strstr(buf,"Title")!=NULL) field_id=FIELD_TITLE;
      else if (strstr(buf,"Source")!=NULL) field_id=FIELD_SOURCE;
      else if (strstr(buf,"Abstract")!=NULL) field_id=FIELD_ABSTRACT;
      else if (strstr(buf,"Unique Identifier")!=NULL) {
        if (!(authors.data==NULL && title.data==NULL && source.data==NULL)) 
          output_fields(&authors,&title,&source,&abstract);
        fflush(stdout);
        newstr_clear(&authors);
        newstr_clear(&title);
        newstr_clear(&source);
        newstr_clear(&abstract);
      }
      else field_id=FIELD_OTHER;
   }
   else {
     if (!(authors.data==NULL && title.data==NULL && source.data==NULL))
       output_fields(&authors,&title,&source,&abstract);
     fflush(stdout);
     newstr_clear(&authors);
     newstr_clear(&title);
     newstr_clear(&source);
     newstr_clear(&abstract);
   }
 }
}

int main(int argc, char *argv[])
{
  FILE *fp;
  int arg,argtemp,found;
  initialize_subs();

  /* Check to see if abstract should be displayed */
  arg=1;
  found=FALSE;
  while (arg<argc && !found) {
    if (strcmp(argv[arg],"-a")==0) {
     found=TRUE;
     abstractout=TRUE;
     for (argtemp=arg+1; argtemp<argc; argtemp++) argv[argtemp-1]=argv[argtemp];
     argc--;
    }
    arg++;
  } 

  if (argc==1) {
    get_fields(stdin);
  }
  else {
    for (arg=1; arg<argc; arg++) {
      fp=fopen(argv[arg],"r");
      if (fp==NULL) {
        fprintf(stderr,"Error.  Cannot open %s for reading.\n",argv[arg]);
      }
      else {
        get_fields(fp);
        fclose(fp);
      }
    }
  }
  return 0;
}
 

