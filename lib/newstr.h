/*
 * newstring header file
 */

#ifndef NEWSTR_H
#define NEWSTR_H

typedef struct newstring {
	char *data;
	unsigned long dim;
	unsigned long len;
}  newstring;

newstring *newstr_new   ( void ); 
void newstr_init        ( newstring *string );
void newstr_free        ( newstring *string );
void newstr_addchar     ( newstring *string, char newchar );
void newstr_strcat      ( newstring *string, char *addstr );
void newstr_segcat      ( newstring *string, char *startat, char *endat );
void newstr_strcpy      ( newstring *string, char *addstr );
void newstr_segcpy      ( newstring *string, char *startat, char *endat );
void newstr_fprintf     ( FILE *fp, newstring *string );
void newstr_findreplace ( newstring *string, char *find, char *replace );
void newstr_empty       ( newstring *string );

/* NEWSTR_PARANOIA
 *
 * set to clear memory before it is freed or reallocated
 * note that this is slower...may be important if string
 * contains sensitive information
 */

#undef NEWSTR_PARANOIA

#endif

