#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newstr.h"

#define TRUE (1)
#define FALSE (0)

char version[] = "1.1";

void
outputtype( newstring *tags, newstring *values, int numtags )
{
	int i, typepos;

	/* process type */
	typepos = -1;
	for (i=0; i<numtags && typepos == -1; ++i) 
		if (strncmp(tags[i].data,"TY  - ",6)==0) typepos = i;
	if (strncasecmp(values[typepos].data,"BOOK",4)==0)
		printf("      <TYPE>BOOK</TYPE>\n");
	else if (strncasecmp(values[typepos].data,"CHAP",4)==0)
		printf("      <TYPE>INBOOK</TYPE>\n");
	else if (strncasecmp(values[typepos].data,"THES",4)==0)
		printf("      <TYPE>PHDTHESIS</TYPE>\n");
	else    printf("      <TYPE>ARTICLE</TYPE>\n");
}

void
outputpages( newstring *tags, newstring *values, int numtags )
{
	int i, sp=-1, ep=-1;
	for (i=0; i<numtags && (sp==-1 || ep==-1); ++i) {
		if(strncmp(tags[i].data,"SP  - ",6)==0) sp = i;
		if(strncmp(tags[i].data,"EP  - ",6)==0) ep = i;
	}
	if ( sp==-1 && ep==-1 ) return;
	printf("      <PAGES>\n");
	if (sp!=-1) printf("        <START>%s</START>\n",values[sp].data);
	if (ep!=-1) printf("        <END>%s</END>\n",values[ep].data);
	printf("      </PAGES>\n");
}

void
outputname( char *name )
{
	char *p = name;
	if ( p == NULL ) return;
	printf("<LAST>");
	while ( *p && *p!=',') printf("%c",*p++);
	printf("</LAST>");
	if ( *p == 0 ) return;
	while ( *p==',' || *p==' ' ) p++;
	while ( *p ) {
		printf("<PREF>");
		while ( *p && *p!=' ' ) printf("%c",*p++);
		while ( *p==' ' ) p ++;
		printf("</PREF>");
	}
}

void
outputauthors( newstring *tags, newstring *values, int numtags )
{
	char autags[4][7] = { "AU  - ", "A1  - ", "A2  - ", "A3  - " };
	int i,j,found=0;
	for (j=0; j<4; ++j)
		for (i=0; i<numtags; ++i ) {
			if (strncmp(tags[i].data,autags[j],6)==0) {
				if (found==0) {
					found=1;
					printf("      <AUTHORS>\n");
				}
				printf("        <AUTHOR>");
				outputname( values[i].data );
				printf("</AUTHOR>\n");
			}
		}
	if ( found ) printf("      </AUTHORS>\n");
}

void
outputeditors( newstring *tags, newstring *values, int numtags )
{
	int i;
	for (i=0; i<numtags; ++i ) {
		if (strncmp(tags[i].data,"ED  - ",6)==0) {
			printf("      <EDITOR>%s</EDITOR>\n",values[i].data);
		}
	}
}

void
outputyear( newstring *tags, newstring *values, int numtags )
{
	int i;
	for (i=0; i<numtags; ++i) {
		if (strncmp(tags[i].data,"PY  - ",6)==0) {
			printf("      <YEAR>%s</YEAR>\n",values[i].data);
		}
	}
}

void
outputrefnum( newstring *tags, newstring *values, int numtags )
{
	int i, year=-1,name=-1;
	char *p;
	for (i=0; i<numtags && (year==-1 || name==-1); ++i ) {
		if (year==-1 && strncmp(tags[i].data,"PY  - ",6)==0) 
			year = i;
		else if (name==-1 && (
			strncmp(tags[i].data,"AU  - ",6)==0 ||
			strncmp(tags[i].data,"A1  - ",6)==0)) 
			name = i;
	}
	if (year!=-1 && name!=-1) {
		printf("      <REFNUM>");
		p = values[name].data;
		while ( p && *p && *p!=',' && *p!='\t' && *p!='\r'
			&& *p!='\n') printf("%c",*p++);
		p = values[year].data;
		while ( p && *p && *p!=',' && *p!='\t' && *p!='\r'
			&& *p!='\n') printf("%c",*p++);
		printf("</REFNUM>\n");
	}
}

void
outputeasy( newstring *tags, newstring *values, int numtags, 
	char *ristag, char *xmltag)
{
	int i;
	for (i=0; i<numtags; ++i ) {
		if (strncmp(tags[i].data,ristag,6)==0) {
			printf("      <%s>%s</%s>\n", xmltag,
				values[i].data, xmltag);
		}
	}
}

void
outputref( newstring *tags, newstring *values, int numtags )
{
	int i;
	printf("  <REF>\n");

	outputtype( tags, values, numtags );
	outputauthors( tags, values, numtags );
	outputeasy( tags, values, numtags, "TI  - ","TITLE");
	outputeasy( tags, values, numtags, "PY  - ","YEAR");
	outputeasy( tags, values, numtags, "JO  - ","JOURNAL");
	outputeasy( tags, values, numtags, "VL  - ","VOLUME");
	outputpages( tags, values, numtags );
	outputeasy( tags, values, numtags, "BT  - ","BOOKTITLE");
	outputeditors( tags, values, numtags );
	outputeasy( tags, values, numtags, "PB  - ","PUBLISHER");
	outputeasy( tags, values, numtags, "CT  - ","ADDRESS");
	outputrefnum( tags, values, numtags );
/*	outputeasy( tags, values, numtags, "KW  - ","REFNUM"); */

#ifdef DONTCOMPILE
{ 
for (i=0; i<numtags; ++i) {
		printf("     <TAG>%s</TAG><VALUE>%s</VALUE>\n",tags[i].data,values[i].data);
	}
}
#endif

	printf("  </REF>\n");
	for (i=0; i<numtags; ++i) {
		newstr_strcpy(&(tags[i]),"\0");
		newstr_strcpy(&(values[i]),"\0");
	}
}

void
addtag( newstring *tag, newstring *value, char *buf )
{
	int i;
	/* since we've already seen a tag, we know this is safe */
	for (i=0; i<6; ++i) newstr_addchar( tag, buf[i] );
	while ( buf[i]!='\0' && buf[i]!='\t' && buf[i]!='\r' && buf[i]!='\n'){
		newstr_addchar( value, buf[i] );
		i++;
	}
}

/* RIS definition of a tag is strict:
    character 1 = uppercase alphabetic character
    character 2 = uppercase alphabetic character
    character 3 = space (ansi 32)
    character 4 = space (ansi 32)
    character 5 = dash (ansi 45)
    character 6 = space (ansi 32)
*/
int
is_tag( char *buf )
{
	if (! (buf[0]>='A' && buf[0]<='Z') ) return FALSE;
	if (! (buf[1]>='A' && buf[1]<='Z') ) return FALSE;
	if (buf[2]!=' ') return FALSE;
	if (buf[3]!=' ') return FALSE;
	if (buf[4]!='-') return FALSE;
	if (buf[5]!=' ') return FALSE;
	return TRUE;
}

long get_refs( FILE *fp )
{
	char buf[1024];
	newstring *tags;
	newstring *values;
	int i;
	long numrefs = 0;
	int numtags = 0;
	int maxtags = 64;
	int inref = FALSE;

	/* prepare memory usage */
	tags   = (newstring *) malloc( sizeof (newstring) * maxtags );
	values = (newstring *) malloc( sizeof (newstring) * maxtags );
	if ( tags==NULL || values==NULL ) {
		fprintf(stderr,"ris2xml:  cannot allocate memory\n");
		if (tags)   free(tags);
		if (values) free(values);
		return 0;
	}
	for (i=0; i<maxtags; ++i) {
		newstr_init( &(tags[i]) );
		newstr_init( &(values[i]) );
	}

	printf("<XML>\n");
	printf("<REFERENCES>\n");

	while( fgets( buf, sizeof(buf), fp ) != NULL ) {
		i = 0;

		/* Each reference starts with 'TY  - ' && ends with 'ER  - ' */
		while( buf[i]!='\0' ) {
			if (is_tag(&(buf[i]))) {
				if (strncmp(&(buf[i]),"TY  - ",6)==0) {
					if (inref==TRUE) {
						fprintf(stderr,"Error.  Reference %d not properly terminated.\n",numrefs+1);
						outputref( tags, values, numtags);
						numrefs ++;
					}
					inref = TRUE;
					numtags = 0;
					addtag(&(tags[numtags]),
					       &(values[numtags]),
					       &(buf[i]) );
					numtags++;
				} else if (strncmp(&(buf[i]),"ER  - ",6)==0) {
					inref = FALSE;
					numrefs ++;
					outputref( tags, values, numtags );
				} else if (inref==TRUE) {
					addtag(&(tags[numtags]),
					       &(values[numtags]),
					       &(buf[i]) );
					numtags++;
					if ( numtags == maxtags ) {
	newstring *newtags, *newvalues;
	maxtags *= 2;
	newtags   = (newstring *) realloc(tags, sizeof (newstring) * maxtags );
	newvalues = (newstring *) realloc(tags, sizeof (newstring) * maxtags );
	if ( newtags==NULL || newvalues==NULL ) {
		fprintf(stderr,"ris2xml:  cannot allocate memory\n");
		if (newtags)   free(newtags); else free(tags);
		if (newvalues) free(newvalues); else free(values);
		return 0;
	}
					}
				} else {
					fprintf(stderr,"Error.  Tagged line not in properly started reference.\n");
					fprintf(stderr,"Ignored: '%s'\n",&(buf[i]));
				}
			}
			while ( buf[i]!='\0' && buf[i]!='\t' && 
 				  buf[i]!='\r' && buf[i]!='\n' ) ++i;
			if (buf[i]=='\t'|| buf[i]=='\r' || buf[i]=='\n') ++i;
		}
	}

	/* clean up memory usage */
	for (i=0; i<maxtags; ++i) {
		newstr_clear( &(tags[i]) );
		newstr_clear( &(values[i]) );
	}
	free(tags);
	free(values);

	printf("</REFERENCES>\n");
	printf("</XML>\n");

	return numrefs;
}

int 
main(int argc, char *argv[])
{
	int 	i;
	FILE 	*fp;
	long 	numref = 0L;

	if (argc==1) 
		numref=get_refs(stdin);
	else {
		for (i=1; i<argc; i++) {
			fp=fopen(argv[i],"r");
			if (fp==NULL) {
				fprintf(stderr,"ris2xml %s: cannot open %s\n",
					version, argv[i]);
			} else { 
				numref+=get_refs(fp);
				fclose(fp);
			}
		}
	}
	fprintf(stderr,"ris2xml %s:  Processed %ld references.\n",
		version, numref);
	return EXIT_SUCCESS;
}
 

