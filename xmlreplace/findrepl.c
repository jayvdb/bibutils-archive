/*
 * find and replace routines
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "findrepl.h"

extern char progname[];

void
disposelist (findreplace *curr)
{
	findreplace *next;
	while ( curr ) {
		next=curr->next;
		newstr_free( &(curr->field) );
		newstr_free( &(curr->find) );
		newstr_free( &(curr->replace) );
		free( curr );
		curr=next;
	}
}

findreplace *
newelement( void )
{
	findreplace *curr = (findreplace *) malloc( sizeof(findreplace) );
	if ( !curr ) {
		fprintf(stderr,"%s error: cannot allocate memory in newelement.\n",progname);
		exit( EXIT_FAILURE );
	}
	newstr_init(&(curr->field));
	newstr_init(&(curr->find));
	newstr_init(&(curr->replace));
	curr->next=NULL;
	return curr;
}

findreplace *
readlist( char *filename )
{
	newstring field, find, replace;
	FILE *fp;
	findreplace *curr, *prev=NULL, *first=NULL;
	char deliminator, *p;
	char buf[512];

	fp = fopen (filename,"r");
	if ( !fp ) return NULL;

	newstr_init( &field );
	newstr_init( &find );
	newstr_init( &replace );

	while ( fgets(buf,sizeof(buf),fp) ) {
		newstr_empty( &field );
		newstr_empty( &find );
		newstr_empty( &replace );
		deliminator=buf[0];
		p=&(buf[1]);
		while (*p && *p!=deliminator && *p!='\n' && *p!='\r')
			newstr_addchar( &field, *p++ );
		if (*p==deliminator) p++;
		while (*p && *p!=deliminator && *p!='\n' && *p!='\r')
			newstr_addchar( &find, *p++ );
		if (*p==deliminator) p++;
		while (*p && *p!=deliminator && *p!='\n' && *p!='\r')
			newstr_addchar( &replace, *p++ );
		if ( field.len!=0 && find.len!=0 ) {
			curr = newelement();
			if ( !first ) first=curr;
			else prev->next = curr;
			newstr_strcpy( &(curr->field),   field.data   );
			newstr_strcpy( &(curr->find),    find.data    );
			newstr_strcpy( &(curr->replace), replace.data );
			prev = curr;
		}
	}
	fclose(fp);
	newstr_free( &replace );
	newstr_free( &find );
	newstr_free( &field );
	return first;
}


