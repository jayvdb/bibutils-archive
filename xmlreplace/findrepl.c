/*
 * find and replace routines
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "findrepl.h"

void
disposelist (findreplace *top)
{
	findreplace *CurrPtr,*NextPtr;
	CurrPtr=top;
	while (CurrPtr!=NULL) {
		newstr_free( &CurrPtr->Field);
		newstr_free( &CurrPtr->Find);
		newstr_free( &CurrPtr->Replace);
		NextPtr=CurrPtr->Next;
		free(CurrPtr);
		CurrPtr=NextPtr;
	}
}

findreplace *
newelement( void )
{
	findreplace *CurrPtr;
	CurrPtr = (findreplace *) malloc (sizeof(findreplace));
	if (CurrPtr==NULL) {
		fprintf(stderr,"Error.  Cannot allocate memory in newelement.\n");
		exit(EXIT_FAILURE);
	}
	newstr_init(&(CurrPtr->Field));
	newstr_init(&(CurrPtr->Find));
	newstr_init(&(CurrPtr->Replace));
	CurrPtr->Next=NULL;
	return CurrPtr;
}

findreplace *
readlist (char *filename)
{
	FILE *fp;
	findreplace *CurrPtr, *PrevPtr, *FirstPtr;
	char deliminator, *p;
	char buf[512];

	fp = fopen (filename,"r");
	if (fp==NULL) return NULL;

	FirstPtr=NULL;
	PrevPtr=NULL;
	while (fgets(buf,sizeof(buf),fp)!=NULL) {

		CurrPtr = newelement();
		if (FirstPtr==NULL) FirstPtr=CurrPtr;
		else PrevPtr->Next=CurrPtr;

		deliminator=buf[0];
		p=&(buf[1]);
		while (*p && *p!=deliminator && *p!='\n' && *p!='\r')
			newstr_addchar(&(CurrPtr->Field),*p++);
		if (*p==deliminator) p++;
		while (*p && *p!=deliminator && *p!='\n' && *p!='\r')
			newstr_addchar(&(CurrPtr->Find),*p++);
		if (*p==deliminator) p++;
		while (*p && *p!=deliminator && *p!='\n' && *p!='\r')
			newstr_addchar(&(CurrPtr->Replace),*p++);

		PrevPtr=CurrPtr;
	}
	fclose(fp);
	return FirstPtr;
}


