/*
 * tree.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newstr.h"
#include "xml.h"
#include "findrepl.h"
#include "tree.h"

extern char progname[];

node *
node_new( void )
{
	node *currnode;
	int i;
	currnode = (node*) malloc( sizeof( *currnode) );
	if ( currnode==NULL ) {
		fprintf(stderr,"%s: cannot allocate memory\n",progname);
		exit(EXIT_FAILURE);
	}
	newstr_init( &(currnode->tag) );
	newstr_init( &(currnode->value) );
	for (i=0; i<30; ++i)
		currnode->nodes[i] = NULL;
	currnode->nnodes = 0;
	return currnode;
}

node *
node_build( char *tag, char *interior )
{
	newstring newtag, newinterior;
	node *currnode;
	char *p, *q;
	int nonspace;

	currnode = node_new();
	newstr_strcpy(&(currnode->tag),tag);

	newstr_init( &newtag );
	newstr_init( &newinterior );

	p = interior;

	while ( p!=NULL && *p ) {
		nonspace=0;
		while ( *p && *p!='<' ) {
			if (*p!='\n' && *p!='\r') {
				if ( nonspace || (*p!=' '  && *p!='\t') ){
					nonspace=1;
					newstr_addchar(&(currnode->value),*p);
				}
			}
			p++;
		}
		if ( *p == '<' ) {
			q = p;
			q++;
			while ( *q && *q!='>' ) newstr_addchar( &newtag, *q++ );
			p = xml_extractdata( p, newtag.data, &newinterior );
			currnode->nodes[currnode->nnodes] =
				node_build( newtag.data, newinterior.data );
			if (newtag.data!=NULL) newtag.data[0]='\0';
			currnode->nnodes++;
			if (currnode->nnodes>=30) {
				fprintf(stderr,"%s: MAXNODES exceeded\n",progname);
				exit(EXIT_FAILURE);
			}
		}
	}

	newstr_free( &newtag );
	newstr_free( &newinterior );

	return currnode;
}

void
node_output( FILE *outptr, node *currnode )
{
	int i;
	fprintf(outptr,"<%s>",currnode->tag.data);
	if (strcasecmp(currnode->tag.data,"REF")==0 ||
	    strcasecmp(currnode->tag.data,"AUTHORS")==0 ||
	    strcasecmp(currnode->tag.data,"EDITORS")==0) 
		fprintf(outptr,"\n");
	if (currnode->value.data!=NULL)
		fprintf(outptr,"%s",currnode->value.data);
	for (i=0; i<currnode->nnodes; ++i) {
		node_output( outptr, currnode->nodes[i] );
	}
	fprintf(outptr,"</%s>",currnode->tag.data);
	if (strcasecmp(currnode->tag.data,"LAST") &&
	    strcasecmp(currnode->tag.data,"PREF") &&
	    strcasecmp(currnode->tag.data,"SUFF") )
		fprintf(outptr,"\n");
}

void
node_findreplace( findreplace *toplist, node *currnode )
{
	findreplace *list;
	int i;
	for (list=toplist; list!=NULL; list=list->Next) {
		if (strcasecmp(currnode->tag.data,list->Field.data)==0 &&
		    currnode->value.data!=NULL && 
		    currnode->value.data[0]!='\0' ) {
			newstr_findreplace(&(currnode->value),
					list->Find.data, list->Replace.data );
		}
	}
	for (i=0; i<currnode->nnodes; ++i) {
		node_findreplace( toplist, currnode->nodes[i] );
	}
}

void
node_free( node *currnode )
{
	int i;
	for (i=0; i<currnode->nnodes; ++i) {
		node_free( currnode->nodes[i] );
		free( currnode->nodes[i] );
	}
	newstr_free( &(currnode->tag) );
	newstr_free( &(currnode->value) );
}

