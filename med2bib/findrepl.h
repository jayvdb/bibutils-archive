/*
 * find/replace header
 *
 * Copyright (c) 1996-8 C. Putnam
 */

#ifndef FINDREPL_H
#define FINDREPL_H

#include <stdio.h>
#include <stdlib.h>
#include "newstr.h"

typedef struct findreplace {
  newstring Find;
  newstring Replace;
  struct findreplace *Next;
}  findreplace;

void initialize_subs(void);
void process_title(newstring *titleptr);
void process_journal(newstring *journalptr);

#endif
