/*
 * find and replace routines for med2bib
 *
 * Copyright (c) 1996-2000 C. Putnam
 */

#include <string.h>
#include "findrepl.h"

static findreplace *titlesub=NULL, *journalsub=NULL;

findreplace *disposelist (findreplace *top)
{
  findreplace *CurrPtr,*NextPtr;
  CurrPtr=top;
  while (CurrPtr!=NULL) {
    NextPtr=CurrPtr->Next;
    free(CurrPtr);
    CurrPtr=NextPtr;
  }
  return NULL;
}

void process_journal (newstring *journalstr)
{
  findreplace *CurrPtr;
  if (journalstr==NULL || journalstr->data==NULL) return;
  CurrPtr=journalsub;
  while (CurrPtr!=NULL) {
    newstr_findreplace(journalstr,CurrPtr->Find.data,CurrPtr->Replace.data);
    CurrPtr=CurrPtr->Next;
  }
}

void process_title (newstring *titlestr)
{
  findreplace *CurrPtr;
  if (titlestr==NULL || titlestr->data==NULL) return;
  CurrPtr=titlesub;
  while (CurrPtr!=NULL) {
    newstr_findreplace(titlestr,CurrPtr->Find.data,CurrPtr->Replace.data);
    CurrPtr=CurrPtr->Next;
  }
}

findreplace *readlist (char *filename)
{
  FILE *fp;
  findreplace *CurrPtr, *PrevPtr, *FirstPtr;
  char deliminator;
  unsigned int pos;
  char buf[512];

  fp = fopen (filename,"r");
  if (fp==NULL) return NULL;

  FirstPtr=NULL;
  PrevPtr=NULL;
  while (fgets(buf,sizeof(buf),fp)!=NULL) {
    CurrPtr = (findreplace *) malloc (sizeof(findreplace));
    if (CurrPtr==NULL) {
      fprintf(stderr,"Error.  Cannot allocate memory in readlist.\n");
      exit(1);
    }
    if (FirstPtr==NULL) FirstPtr=CurrPtr;
    else PrevPtr->Next=CurrPtr;
    newstr_init(&(CurrPtr->Find));
    newstr_init(&(CurrPtr->Replace));
    CurrPtr->Next=NULL;
    deliminator=buf[0];
    pos=1;
    while (buf[pos]!=deliminator && buf[pos]!='\n' && buf[pos]!='\r'
       && pos<strlen(buf)) {
      newstr_addchar(&(CurrPtr->Find),buf[pos]);
      pos++;
    }
    if (buf[pos]==deliminator) pos++;
    while (buf[pos]!=deliminator && buf[pos]!='\n' && buf[pos]!='\r'
       && pos<strlen(buf)) {
      newstr_addchar(&(CurrPtr->Replace),buf[pos]);
      pos++;
    }
    PrevPtr=CurrPtr;
  }
  fclose(fp);
  return FirstPtr;
}


void initialize_subs(void)
{
  char *filename;

  titlesub = disposelist(titlesub);
  filename=getenv("MED2BIB_TITLE");
  if (filename!=NULL) {
    titlesub=readlist(filename);
  }
  if (titlesub==NULL) titlesub=readlist("title.sub");

/* 
  {
    findrepl *CurrPtr;
    CurrPtr = titlesub;
    while (CurrPtr!=NULL) {
       printf("%s-->%s\n",CurrPtr->Find.data,CurrPtr->Replace.data);
       CurrPtr=CurrPtr->Next;
    }
  }
*/

  journalsub = disposelist(journalsub);

  filename=getenv("MED2BIB_JOURNAL");
  if (filename!=NULL) {
    journalsub=readlist(filename);
  }
  if (journalsub==NULL) journalsub=readlist("journal.sub");

/*
  CurrPtr = titlesub;
  while (CurrPtr!=NULL) {
    printf("%s-->%s\n",CurrPtr->Find.data,CurrPtr->Replace.data);
    CurrPtr=CurrPtr->Next;
  }
*/

}
