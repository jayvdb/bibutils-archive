/*
 * refs.c
 *
 * Copyright (c) 1996-8 C. Putnam
 */
#include <stdio.h>
#include <stdlib.h>
#include "refs.h"

refs *refs_new(void)
{
  refs *CurrPtr;
  newstring *CurrStr;
  CurrPtr = (refs *)malloc (sizeof(refs));
  if (CurrPtr==NULL) {
    fprintf(stderr,"Error.  Cannot allocate memory.\n");
    exit(1);
  }
  CurrStr = (newstring *) malloc (sizeof(newstring));
  if (CurrStr==NULL) {
    fprintf(stderr,"Error.  Cannot allocate memory.\n");
    exit(1);
  }
  CurrPtr->refname=CurrStr;
  CurrStr = (newstring *) malloc (sizeof(newstring));
  if (CurrStr==NULL) {
    fprintf(stderr,"Error.  Cannot allocate memory.\n");
    exit(1);
  }
  CurrPtr->source=CurrStr; 
  CurrPtr->startpos=0;
  CurrPtr->endpos=0;
  CurrPtr->Next=NULL;
  newstr_init(CurrPtr->refname);
  newstr_init(CurrPtr->source);
  return CurrPtr;
}

void refs_dispose(refs *CurrPtr)
{
  refs *NextPtr;
  while (CurrPtr!=NULL) {
    NextPtr=CurrPtr->Next;
    newstr_clear(CurrPtr->refname);
    newstr_clear(CurrPtr->source);
    free(CurrPtr);
    CurrPtr=NextPtr;
  }
}
