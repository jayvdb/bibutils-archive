/*
 * find/replace header
 *
 * Copyright (c) 1996-2000 C. Putnam
 */

#ifndef FINDREPL_H
#define FINDREPL_H

#include <stdio.h>
#include <stdlib.h>
#include "newstr.h"

typedef struct findreplace {
	newstring Field;
	newstring Find;
	newstring Replace;
	struct findreplace *Next;
}  findreplace;

findreplace *readlist( char *filename  );

#endif
