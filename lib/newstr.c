/*
 * newstring routines for dynamically allocated strings
 */
#include "newstr.h"

#define newstr_initlen 512

void newstr_init(newstring *string)
{
  string->dim=0;
  string->data=NULL;
}

void newstr_clear(newstring *string)
{
  string->dim=0;
  if (string->data!=NULL) free(string->data);
  string->data=NULL;
}

void newstr_addchar(newstring *string,char newchar)
{
  char *newptr;
  if (string->data==NULL || string->dim==0) {
    string->data = (char *) malloc (sizeof(char) * newstr_initlen);
    if (string->data==NULL) {
      fprintf(stderr,"Error.  Cannot allocate memory in newstr_addchar.\n");
      exit(1);
    }
    string->data[0]='\0';
    string->dim=newstr_initlen;
  }
  if (strlen(string->data)+1 > string->dim) {
    newptr = (char *) realloc (string->data, sizeof(char)*string->dim*2);
    if (newptr==NULL) {
      fprintf(stderr,"Error.  Cannot reallocate memory in newstr_addchar.\n");
      exit(1);
    }
    string->data=newptr;
    string->dim*=2;
  }
  string->data[strlen(string->data)+1]='\0';
  string->data[strlen(string->data)]=newchar;
}

void newstr_strcat (newstring *string, char *addstr)
{
  char *newptr;
  if (string->data==NULL || string->dim==0) {
   string->data = (char *) malloc (sizeof(char) * newstr_initlen);
    if (string->data==NULL) {
      fprintf(stderr,"Error.  Cannot allocate memory in newstr_strcat.\n");
      exit(1);
    }
    string->data[0]='\0';
    string->dim=newstr_initlen;
  }
  if (strlen(string->data)+strlen(addstr) > string->dim) {
    newptr = (char *) realloc (string->data,
                   sizeof(char)*(string->dim+strlen(addstr) +1));
    if (newptr==NULL) {
      fprintf(stderr,"Error.  Cannot reallocate memory in newstr_strcat.\n");
      exit(1);
    }
    string->data=newptr;
    string->dim=string->dim+strlen(addstr)+1;
  }
  strcat(string->data,addstr);
}

void newstr_strcpy (newstring *string, char *addstr)
{
  char *newptr;
  if (string->data==NULL || string->dim==0) {
   string->data = (char *) malloc (sizeof(char) * newstr_initlen);
    if (string->data==NULL) {
      fprintf(stderr,"Error.  Cannot allocate memory in newstr_strcat.\n");
      exit(1);
    }
    string->data[0]='\0';
    string->dim=newstr_initlen;
  }
  if (strlen(addstr) > string->dim) {
    newptr = (char *) realloc (string->data, sizeof(char)*strlen(addstr));
    if (newptr==NULL) {
      fprintf(stderr,"Error.  Cannot reallocate memory in newstr_strcat.\n");
      exit(1);
    }
    string->data=newptr;
    string->dim=strlen(addstr);
  }
  strcpy(string->data,addstr);
}


void newstr_findreplace (newstring *string, char *find, char *replace)
{
  unsigned int findstart,findend,diff;
  unsigned int p,pos,replace_pos;
  int minsize,freeit=0;
  char *newptr;
  int searchstart;
  if (string==NULL || find==NULL) return;
  if (replace==NULL) {
    replace=(char *) malloc (sizeof(char));
    if (replace==NULL) {
      fprintf(stderr,"Error.  Cannot allocate memorynewstr_findreplace\n");
      exit(1);
    }
    *replace='\0';
    freeit=1;
  }
  if (string->data==NULL || string->dim==0) return;
  searchstart=0;
  while (strstr(string->data + searchstart,find)!=NULL) {
    minsize = strlen(string->data) + strlen(replace) - strlen(find);
    if (string->dim < minsize) {
      newptr=(char *) realloc (string->data,sizeof(char)*minsize);
      if (newptr==NULL) {
        fprintf(stderr,"Error.  reallocatememorynewstr_findreplace.\n");
        exit(1);
      }
      string->data=newptr;
      string->dim=minsize;
    }
    findstart=(unsigned int) strstr(string->data + searchstart,find) -
(unsigned int) string->data;
    findend  =findstart + strlen(find);
    if (strlen(find)>=strlen(replace)) {
      p=findstart;
      for (replace_pos=0; replace_pos<strlen(replace); replace_pos++,p++)
        string->data[p]=replace[replace_pos];
      pos=findend;
      while (pos<=strlen(string->data)) {
        string->data[p]=string->data[pos];
        p++;
        pos++;
      }
    }
    else {
      diff = strlen(replace) - strlen(find);
      for (p=strlen(string->data); p>=findend; --p)
        string->data[p+diff] = string->data[p];
      for (p=0; p<strlen(replace); ++p)
        string->data[p+findstart]=replace[p];
    }
    searchstart=findstart + strlen(replace);
  }
  if (freeit) free(replace);
}

