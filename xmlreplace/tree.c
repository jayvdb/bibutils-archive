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
	int i, min_alloc=25;
	currnode = (node*) malloc( sizeof(node) );
	if ( !currnode ) {
		fprintf(stderr,"%s: cannot allocate memory\n",progname);
		exit(EXIT_FAILURE);
	}
	newstr_init( &(currnode->tag) );
	newstr_init( &(currnode->value) );
	currnode->nodes = (node**) malloc( sizeof(node*)*min_alloc );
	if ( !currnode->nodes ) {
		fprintf(stderr,"%s: cannot allocate memory\n",progname);
		exit(EXIT_FAILURE);
	}
	for (i=0; i<min_alloc; ++i)
		currnode->nodes[i] = NULL;
	currnode->maxnodes = min_alloc;
	currnode->nnodes = 0;
	return currnode;
}

void
node_moresubnodes( node *currnode )
{
	node **newnodes;
	int i, min_alloc = currnode->maxnodes*2;
	newnodes = (node**) realloc( currnode->nodes, sizeof(node*)*min_alloc );
	if ( !newnodes ) {
		fprintf(stderr,"%s: cannot allocate memory\n",progname);
		exit(EXIT_FAILURE);
	}
	currnode->nodes = newnodes;
	for ( i=currnode->maxnodes; i<min_alloc; ++i )
		currnode->nodes[i] = NULL;
	currnode->maxnodes = min_alloc;
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
			newstr_empty( &newtag );
			currnode->nnodes++;
			if (currnode->nnodes>=currnode->maxnodes)
				node_moresubnodes( currnode );
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
	int inname, indate, inpages;
	inname = ( !strcasecmp(currnode->tag.data,"LAST") ||
	           !strcasecmp(currnode->tag.data,"PREF") ||
	           !strcasecmp(currnode->tag.data,"SUFF") );
	indate = ( !strcasecmp(currnode->tag.data,"YEAR") ||
	           !strcasecmp(currnode->tag.data,"MONTH") ||
	           !strcasecmp(currnode->tag.data,"DAY") ||
	           !strcasecmp(currnode->tag.data,"DATEOTHER") );
	inpages =( !strcasecmp(currnode->tag.data,"START") ||
	           !strcasecmp(currnode->tag.data,"END") );
	if ( !inname && !indate && !inpages ) fprintf( outptr, "    " );
	if ( !strcasecmp(currnode->tag.data,"AUTHOR") ||
	     !strcasecmp(currnode->tag.data,"EDITOR") ) 
		fprintf( outptr, "    " );
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
	if ( !strcasecmp(currnode->tag.data,"AUTHORS") ||
	     !strcasecmp(currnode->tag.data,"EDITORS") ) 
		fprintf( outptr, "    ");
	fprintf(outptr,"</%s>",currnode->tag.data);
	if ( !inname && !indate && !inpages ) fprintf( outptr, "\n" );
}

void
node_findreplace( findreplace *toplist, node *currnode )
{
	findreplace *list;
	int i;
	for ( list=toplist; list; list=list->next ) {
		if ( currnode->tag.len==0 || currnode->value.len==0 ) continue;
		if ( strcasecmp(currnode->tag.data,list->field.data) ) continue;
		newstr_findreplace( &(currnode->value),
				list->find.data, list->replace.data );
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

