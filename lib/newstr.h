/*
 * newstring header file
 */

#ifndef NEWSTR_H
#define NEWSTR_H

#include <stdio.h>
#include <string.h>

typedef struct newstring {
  char *data;
  int dim;
}  newstring;

void newstr_init    (newstring *string);
void newstr_clear   (newstring *string);
void newstr_addchar (newstring *string, char newchar);
void newstr_strcat  (newstring *string, char *addstr);
void newstr_strcpy  (newstring *string, char *addstr);
void newstr_findreplace (newstring *string, char *find, char *replace);

#endif

