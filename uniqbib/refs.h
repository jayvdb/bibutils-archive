/*
 * refs.h
 *
 * Copyright (c) 1996-8 C. Putnam
 */

#ifndef REFS_H
#define REFS_H

#include "newstr.h"

typedef struct refs { 
  newstring *refname; 
  newstring *source;
  long startpos; 
  long endpos; 
  struct refs *next; 
} REFS;

REFS *refs_new     ( void );
void  refs_dispose ( REFS *curr );

#endif


