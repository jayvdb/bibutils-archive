/*
 * uniqbib.c
 * 
 * Checks and fixes bibfiles such that they only have
 * unique citation names.
 *
 * C. Putnam Nov 1996 -> May 2003
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "newstr.h"
#include "refs.h"

char progname[] = "uniqbib";
char version[] = "version 1.1 05/22/03";

#define TRUE (1==1)
#define FALSE (!TRUE)

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

int 
whitespace (char ch)
{
	if (ch==' ' || ch=='\t' || ch=='\n' || ch=='\r') return TRUE;
	else return FALSE;
}

int 
endofline (char ch)
{
	if (ch=='\n' || ch=='\r') return TRUE;
	else return FALSE;
}


void 
get_citename(newstring *string, char *p)
{
	p++;
	while (*p && *p!=',') {
		if (!whitespace(*p)) newstr_addchar(string,*p);
		p++;
	}
}

char *
search (char *buffer,char *target)
{
	char *pbuf = buffer;
	char *ptar = target;
	char *returnptr=NULL;
	int found=FALSE;
	int pos=0;


	while (*ptar && *pbuf && !found) {

		while ((*pbuf) && (*(pbuf+pos)) && (*(ptar+pos)) && (!found)) {
			if (toupper(*(pbuf+pos))==toupper(*(ptar+pos))) {
				pos++;
				if (*(ptar+pos)=='\0') {
					found=TRUE;
					returnptr=pbuf;
				}
				else if (*(pbuf+pos)=='\0') {
					return NULL;
				}
			}
			else {
				pos=0;
				pbuf++;
			}
		}

	}

	return returnptr;
}

void 
extract_quotes(newstring *string, char *newdata)
{
	char *p;
	int endwithquotes=FALSE;

	p=strchr(newdata,'=');
	if (p==NULL) return;
	p++;
	while (whitespace(*p)) p++;
	if (*p=='\"') {
		endwithquotes=TRUE; 
		p++;
	}
	while (*p && *p!=',' && *p!=')') {
		if (endwithquotes && *p=='\"' && *(p-1)!='\\') break; 
		newstr_addchar(string,*p);
		p++;
	}
	newstr_strcat(string,"  ");
}

void
report_match( REFS *r1, REFS *r2, int nummatch ) 
{
	fprintf(stderr,"\nDuplicate identifiers:  Modifying\n");
	fprintf(stderr," # 1:  %s",r1->refname->data);
	fprintf(stderr," (%ld - %ld) == %sa\n",r1->startpos,r1->endpos,
			r1->refname->data);
	fprintf(stderr,"       %s\n",r1->source->data);
	fprintf(stderr," #%2d:  %s",nummatch+1,r2->refname->data);
	fprintf(stderr," (%ld - %ld) == %s\n",r2->startpos,r2->endpos,
			r2->refname->data);
	fprintf(stderr,"       %s\n",r2->source->data); 
}


void 
find_duplicates( REFS *first )
{
	REFS *r1, *r2;
	int  nummatch,tempmatch,i;
	char ch, *p1;

	for ( r1=first; r1; r1=r1->next) {
		nummatch=0;
		p1 = r1->refname->data;
		for ( r2=r1->next; r2; r2=r2->next ) {
			if (strcmp(p1,r2->refname->data)==0) {
				nummatch++;
				tempmatch=nummatch;
				for (i=0; i<=tempmatch/26; i++) {
					ch = ((tempmatch%26) + 'a');
					newstr_addchar(r2->refname,ch);
					tempmatch=tempmatch/26;
				}
				report_match( r1, r2, nummatch );
			}
		}
		if (nummatch>0) newstr_addchar(r1->refname,'a');
	}
}

/*
 * get_refs()
 *
 * Scans for a Bibtex reference
 * starting with the '@' character as the first
 * non-space character in a line.
 *
 */
REFS *
get_refs( char *filename )
{
	FILE *inptr;
	REFS *first=NULL, *CurrPtr=NULL, *PrevPtr=NULL;
	long pos;
	char line[512],*p,*startat;
	int quotations=0;

	inptr = fopen( filename, "r" );
	if ( inptr==NULL ) {
		fprintf(stderr,"Cannot open %s.\n\n",filename);
		return NULL;
	}

  while (!feof(inptr)) {

    /** Put a file pointer to the beginning of the article. **/
    pos=ftell(inptr);

    /** Get each line of the file.                          **/
    if (fgets(line,sizeof(line),inptr)!=NULL) {
      p=line;
      while (whitespace(*p)) p++;

        /*
         * If the character is not a '@', then we should ignore the
         * line, just keeping track of quotation marks and go on.
         */
      if (*p!='@' || quotations%2!=0) {
          if (CurrPtr!=NULL) {
            if (search(p,"JOURNAL=")!=NULL) extract_quotes(CurrPtr->source,p);
            if (search(p,"VOLUME=")!=NULL) extract_quotes(CurrPtr->source,p);
            if (search(p,"PAGES=")!=NULL) extract_quotes(CurrPtr->source,p);
          }
          while (*p) { 
            if (*p=='\"' && *(p-1)!='\\') quotations++;
            p++;
          }
      }

	/*
	 * If the character is a '@', take it to be the
	 * beginning of a new reference.  Read the number of
	 * lines between the '@' and the terminal ')'.  Make
	 * sure that the program ignores all ')' within
	 * quotation marks.  Send the output to read_ref() for
	 * conversion.
	 */

      else if (*p=='@') {
        if (CurrPtr!=NULL) PrevPtr=CurrPtr;
        CurrPtr = refs_new();
        if (PrevPtr!=NULL) PrevPtr->next=CurrPtr;
        if (first==NULL) first=CurrPtr;
        startat=strchr(p,'(');
        if (startat==NULL) startat=strchr(p,'{');
		if (startat!=NULL) get_citename(CurrPtr->refname,startat);
        CurrPtr->startpos=pos;
        if (PrevPtr!=NULL) PrevPtr->endpos=pos-1; 

        while (*p) {
          if (*p=='\"' && *(p-1)!='\\') quotations++;
          p++;
       }
     }
   }
 }
	if (CurrPtr!=NULL) CurrPtr->endpos=ftell(inptr);
	fclose(inptr);
	return first;
}

REFS *
get_prev( REFS *first, REFS *search )
{
	REFS *curr;
	if (first==search) return NULL;
	curr=first;
	while (curr!=NULL && curr->next!=search) curr=curr->next;
	return curr;
}

/* swap_refs()
 *
 * returns top of reference list, because it could be
 * part of the swap
 *
 */
REFS *
swap_refs( REFS *first, REFS *r1, REFS *r2 ) 
{
	REFS *b1, *b2, *tmp;

	if ( r1==r2 ) return first;

	b1 = get_prev( first, r1 );
	b2 = get_prev( first, r2 );

	if ( b2==r1 ) {
		tmp = r2->next;
		r2->next = r1;
		r1->next = tmp;
		if ( b1 ) b1->next = r2;
	} else if ( b1==r2 ) {
		tmp = r1->next;
		r1->next = r2;
		r2->next = tmp;
		if ( b2 ) b2->next = r1;
	} else {
		tmp = r2->next;
		r2->next = r1->next;
		r1->next = tmp;
		if ( b1 ) b1->next = r2;
		if ( b2 ) b2->next = r1;
	}

	if ( first==r1 ) first=r2;

	return first;
}

void 
display_list(REFS *first)
{
	REFS *curr;
	for ( curr=first; curr; curr=curr->next ) {
		fprintf(stderr,"  %s\n",curr->refname->data);
	}
}
/**
int
num_refs( REFS *r )
{
	int   nrefs = 0;
	while ( r ) {
		nrefs++;
		r = r->next;
	}
	return nrefs;
}
**/

/* order_refs()
 *
 * return 1 if r1>r2
 * return -1 if r1<r2
 * return 0 if r1==r2
 */
int
order_refs( REFS *r1, REFS *r2, char sortcode )
{
	int result = 1;  /* default don't switch */
	if ( sortcode== 'r' ) { /* sort by refnum */
		result = strcmp( r1->refname->data, r2->refname->data );
	}
	return result;
}

REFS *
sort_refs( REFS *first, char sortcode )
{
	REFS *curr = first, *next;
	int  swap = FALSE;

	while ( curr ) {
		next=curr->next;
		swap=FALSE;
		while ( next && !swap ) {
			if ( order_refs( curr, next, sortcode ) > 0 ) {
				first = swap_refs( first, curr, next );
				curr  = next;
				swap  = TRUE;
			}
			next = next->next;
		}
		if ( !swap ) curr=curr->next;
	}
	return first;
}

void 
put_refs( char *outfile, REFS *firstref, char *infile )
{
	newstring line;
	FILE      *inptr, *outptr;
	REFS      *curr;
	char      *p,*q;
	int       ch;

	inptr = fopen( infile, "rb" );
	if ( inptr==NULL ) {
		fprintf(stderr,"Error opening %s.\n",infile);
		return;
	}

	if ( outfile[0]=='\0' ) outptr = stdout;
	else outptr = fopen( outfile, "w" );
	if ( outptr==NULL ) {
		fprintf(stderr,"Cannot open %s for writing.\n\n",outfile);
		fclose( inptr );
		return;
	}

	newstr_init(&line);

	for ( curr=firstref; curr; curr=curr->next ) {

		if (fseek(inptr,curr->startpos,SEEK_SET)!=0) {
			fprintf(stderr,"Error positining in %s.\n",infile);
			continue;
		}

		while (!feof(inptr) && ftell(inptr)<=curr->endpos) {
			ch='\0';
			while (!feof(inptr) && ftell(inptr)<=curr->endpos && ch!='\n' && ch!='\r') {
				ch = fgetc(inptr);
				if ( ch!='\n' && ch!='\r' && ch!=EOF ) 
					newstr_addchar(&line,ch);
			}
			p=line.data;
			while ( whitespace(*p) ) p++;
			if (*p=='\0') { /* do nothing */ }
			else if (*p!='@') fprintf(outptr,"%s\n",line.data);
			else {
				q=strchr(p,'(');
				if (q==NULL) q=strchr(p,'{');
				if (q==NULL) fprintf(outptr,"%s",line.data);
				else {
					p=line.data;
					fprintf(outptr,"\n");
					while (*p && !(*(p-1)=='(' || *(p-1)=='{')) fprintf(outptr,"%c",*p++);
					fprintf(outptr,"%s",curr->refname->data);
					p=strchr(p,',');
					while (*p) fprintf(outptr,"%c",*p++);
					fprintf(outptr,"\n");
				}
			}
			newstr_empty( &line );
		}
	}
	newstr_free( &line );
	fclose(inptr);
	fclose(outptr);
}

char
get_sortcode( int *argc, char *argv[] )
{
	char sortcode = '\0';
	int  i, deletenum = 0;
	for ( i=1; i<*argc; i++ ) {
		if ( strncmp(argv[i],"-s",2)==0 ) {
			sortcode = tolower( argv[i][2] ) ;
			deletenum = i;
		}
	}
	if ( deletenum!=0 ) {
		for (i=deletenum+1; i<*argc; i++) argv[i-1]=argv[i];
		*argc = *argc-1;
	}
	return sortcode;
}

void
help( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"\n%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	fprintf(stderr,"Usage:  uniqbib <old bibtex file> [<new bibtex file>]\n\n");
}

int 
main( int argc, char *argv[] )
{
	REFS *reflist = NULL; 
	char sortcode;

	sortcode = get_sortcode( &argc, argv );

	if (argc!=2 && argc!=3) {
		help();
		exit( EXIT_FAILURE );
	}

	reflist = get_refs( argv[1] );
	if ( !reflist ) exit( EXIT_FAILURE );

	find_duplicates( reflist );

	if ( sortcode!='\0' ) reflist = sort_refs( reflist, sortcode );

	if (argc>2) put_refs( argv[2], reflist, argv[1] ); 
	else put_refs( "\0", reflist, argv[1] );

	refs_dispose( reflist );

	return EXIT_SUCCESS;
}
