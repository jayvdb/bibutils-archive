/*
 * refs.h
 *
 * Copyright (c) 1996-8 C. Putnam
 */

#ifndef REFS_H
#define REFS_H

#include "newstr.h"

typedef struct refs { 
  newstring* refname; 
  newstring* source;
  long startpos; 
  long endpos; 
  struct refs *Next; 
} refs;

refs *refs_new(void);
void refs_dispose(refs *CurrPtr);

#endif


