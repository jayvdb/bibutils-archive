/*
 * uniqbib.c
 * 
 * Checks and fixes bibfiles such that they only have
 * unique citation names.
 *
 * C. Putnam  November, 1996
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "newstr.h"
#include "refs.h"

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
find_duplicates(REFS *FirstPtr)
{
  REFS *ComparePtr, *CurrPtr;
  int  nummatch,tempmatch,i;
  char ch;

  for ( ComparePtr=FirstPtr; ComparePtr!=NULL; ComparePtr=ComparePtr->next) {
/* fprintf(stderr,"ComparePtrloop\n"); fflush(stderr); */
    nummatch=0;
	for ( CurrPtr=ComparePtr->next; CurrPtr!=NULL; CurrPtr=CurrPtr->next ) {
/* fprintf(stderr,"CurrPtrloop\n"); fflush(stderr); */
      if (strcmp(ComparePtr->refname->data,CurrPtr->refname->data)==0) {
        nummatch++;
        fprintf(stderr,"\nDuplicate identifiers:  Modifying\n");
        fprintf(stderr," # 1:  %s",ComparePtr->refname->data);
        fprintf(stderr," (%ld - %ld) == %sa\n",ComparePtr->startpos,ComparePtr->endpos,ComparePtr->refname->data);
        fprintf(stderr,"       %s\n",ComparePtr->source->data);
        fprintf(stderr," #%2d:  %s",nummatch+1,CurrPtr->refname->data);
        tempmatch=nummatch;
        for (i=0; i<=tempmatch/26; i++) {
          ch = ((tempmatch%26) + 'a');
          newstr_addchar(CurrPtr->refname,ch);
          tempmatch=tempmatch/26;
        }
        fprintf(stderr," (%ld - %ld) == %s\n",CurrPtr->startpos,CurrPtr->endpos,CurrPtr->refname->data);
        fprintf(stderr,"       %s\n",CurrPtr->source->data); 
      }
    }
    if (nummatch>0) newstr_addchar(ComparePtr->refname,'a');
  }
} 

/*
 * scanfor_refs()
 *
 * This function scans for a Bibtex reference
 * starting with the '@' character as the first
 * non-space character in a line.
 *
 */
REFS *
scanfor_refs(FILE *inptr)
{
  long pos;
  char line[512],*p,*startat;
  REFS *CurrPtr=NULL,*FirstPtr=NULL,*PrevPtr=NULL;
  int quotations=0;

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
        if (FirstPtr==NULL) FirstPtr=CurrPtr;
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
 return FirstPtr;
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

REFS *
do_swap( REFS **CurrPtrPtr, REFS **NextPtrPtr, REFS **FirstPtrPtr ) 
{
  REFS *HolderPtr,*BeforeCurr,*BeforeNext;
  HolderPtr=(*NextPtrPtr)->next;

  BeforeCurr=get_prev(*FirstPtrPtr,*CurrPtrPtr);
  BeforeNext=get_prev(*FirstPtrPtr,*NextPtrPtr);
  if (BeforeNext==*CurrPtrPtr) {
    (*NextPtrPtr)->next=*CurrPtrPtr;
    (*CurrPtrPtr)->next=HolderPtr;
    if (BeforeCurr!=NULL) BeforeCurr->next=*NextPtrPtr;
  }
  else {
    (*NextPtrPtr)->next=(*CurrPtrPtr)->next;
    (*CurrPtrPtr)->next=HolderPtr;
    if (BeforeCurr!=NULL) BeforeCurr->next=*NextPtrPtr;
    if (BeforeNext!=NULL) BeforeNext->next=*CurrPtrPtr;
  }

  if (*FirstPtrPtr==*CurrPtrPtr) {
     *FirstPtrPtr=*NextPtrPtr;
fprintf(stderr,"Changed FirstPtr!\n");
  }
  *CurrPtrPtr=*NextPtrPtr;
  return HolderPtr;
}

void 
display_list(REFS *first)
{
	REFS *curr;
	for ( curr=first; curr; curr=curr->next ) {
		fprintf(stderr,"  %s\n",curr->refname->data);
	}
}

REFS *
sort_refs( REFS *first, char code )
{
	REFS *curr,*next;
	int swap=FALSE,done;

	curr=first;
	while (curr!=NULL) {

		/* if (swap) curr=first; */

		next=curr->next;
		done=FALSE;
		swap=FALSE;
		do {
			if (next==NULL) done=TRUE;
			else {
				if (code=='r') { /* sort by refnum */
					fprintf(stderr,"%s and %s",
						curr->refname->data,
						next->refname->data);
					if (strcmp(curr->refname->data,
						next->refname->data)<=0){
							fprintf(stderr,"\n"); 
							next=next->next;
					} else {
						fprintf(stderr," Swapping\n");
						do_swap(&curr,&next,&first);
						swap = TRUE;
					}
				/*display_list(first);*/
				}
			}
		} while (!done && !swap);
		if (!swap) curr=curr->next;
	}
	return first;
}

void 
write_file (FILE *outptr, REFS *firstref, char *filename)
{
	FILE *inptr;
	newstring line;
	REFS *CurrPtr;
	char *p,*q,ch;

	inptr = fopen( filename, "rb" );
	if ( inptr==NULL ) {
		fprintf(stderr,"Error opening %s.\n",filename);
		return;
	}

	newstr_init(&line);


	for ( CurrPtr=firstref; CurrPtr!=NULL; CurrPtr=CurrPtr->next ) {

		if (fseek(inptr,CurrPtr->startpos,SEEK_SET)!=0) {
			fprintf(stderr,"Error positining in %s.\n",filename);
			exit(1);
		}

		while (!feof(inptr) && ftell(inptr)<=CurrPtr->endpos) {
			ch='\0';
			while (!feof(inptr) && ftell(inptr)<=CurrPtr->endpos && ch!='\n' && ch!='\r') {
				ch = fgetc(inptr);
				if ( ch!='\n' && ch!='\r' ) newstr_addchar(&line,ch);
			}
			p=line.data;
			while (whitespace(*p)) p++;
			if (*p=='\0') continue;
			if (*p!='@') fprintf(outptr,"%s\n",line.data);
			else {
				q=strchr(p,'(');
				if (q==NULL) q=strchr(p,'{');
				if (q==NULL) fprintf(outptr,"%s",line.data);
				else {
					p=line.data;
					fprintf(outptr,"\n");
					while (*p && !(*(p-1)=='(' || *(p-1)=='{')) fprintf(outptr,"%c",*p++);
					fprintf(outptr,"%s",CurrPtr->refname->data);
					p=strchr(p,',');
					while (*p) fprintf(outptr,"%c",*p++);
					fprintf(outptr,"\n");
				}
			}
			line.data[0]='\0';
		}
	}
	newstr_free( &line );
	fclose(inptr);
} 

int main(int argc,char *argv[])
{
  REFS *firstref=NULL; 
  FILE *inptr,*outptr;
  int i,remove=0;
  char code='\0';

  for (i=1; i<argc; i++) {
    if (strncmp(argv[i],"-s",2)==0) {
      code=tolower(argv[i][2]);
      remove=i;
    }
  }

  if (remove!=0) {
    for (i=remove+1; i<argc; i++) argv[i-1]=argv[i];
    argc--;
  }

  if (argc!=2 && argc!=3) {
    fprintf(stderr,"Usage:  uniqbib <old bibtex file> [<new bibtex file>]\n\n");
    exit (1);
  }

  if ((inptr = fopen (argv[1],"r"))==NULL) {
    fprintf(stderr,"Cannot open %s.\n\n",argv[1]);
    exit (1);
  }

  else {
    firstref=scanfor_refs(inptr);
    fclose(inptr);
/****
fprintf(stderr,"about to count references\n"); fflush(stderr);
{
	REFS *tmp = firstref;
	int i=0;
	while ( tmp ) { i++; tmp=tmp->next; }
	fprintf(stderr,"%d references\n",i);
}
*****/
/* fprintf(stderr,"about to find_duplicates\n"); fflush(stderr); */
    find_duplicates(firstref);
/* fprintf(stderr,"did find_duplicates\n"); fflush(stderr); */
   }

  /* If an output file is specified, send the data! */
  if (argc>2) {
    if ((outptr = fopen (argv[2],"w"))==NULL) {
      fprintf(stderr,"Cannot open %s.\n\n",argv[2]);
      exit(1);
    }
    if (code!='\0') firstref=sort_refs(firstref,code);
    write_file(outptr,firstref,argv[1]); 
    fclose(outptr);
  }
   refs_dispose(firstref);
   return 0;
 }
