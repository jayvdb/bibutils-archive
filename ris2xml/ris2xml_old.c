#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "newstr.h"

#define TRUE (1)
#define FALSE (0)

char progname[] = "ris2xml";
char version[] = "1.3 2/15/03";

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

/* name is in format Lastname,Firstname(or inits),Suffix */
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
	while ( *p && *p!=',') {
		printf("<PREF>");
		while ( *p && *p!=' ' && *p!=',') printf("%c",*p++);
		while ( *p==' ' ) p++;
		printf("</PREF>");
	}
	while ( *p==',' || *p==' ' ) p++;
	if ( *p ) {
		printf("<SUFF>");
		while ( *p && *p!=' ' && *p!=',') printf("%c",*p++);
		while ( *p==' ' ) p++;
		printf("</SUFF>");
	}
}

/* potentially multiple names separated by semicolons */
void
outputnames( char *tag, char *value )
{
	newstring name;
	int i, len_value;

	newstr_init( &name );
	len_value = strlen( value );
	for ( i=0; i<len_value; ++i ) {
		if ( value[i]==';' ) {
			if (name.data) {
				printf("        <%s>",tag); 
				outputname( name.data );
				printf("</%s>\n",tag);
				name.data[0]='\0';
			}
		} else {
			if ( name.data==NULL || strlen(name.data)>0 || 
			     (value[i]!=' ' && value[i]!='\t'))
				newstr_addchar( &name, value[i] );
		}
	}
	printf("        <%s>",tag); 
	outputname( name.data );
	printf("</%s>\n",tag);
	newstr_free( &name );
}

void
outputauthors( newstring *tags, newstring *values, int numtags )
{
	char *authortags[] = { "AU  - ", "A1  - ", "A3  - " };
	int  nauthortags = sizeof(authortags)/sizeof(char*);
	int i,j,found=0;

	for (j=0; j<nauthortags; ++j)
		for (i=0; i<numtags; ++i ) {
			if (strcasecmp(tags[i].data,authortags[j])==0) {
				if (found==0) {
					found=1;
					printf("      <AUTHORS>\n");
				}
				outputnames( "AUTHOR", values[i].data );
				found++;
			}
		}
	if ( found ) printf("      </AUTHORS>\n");
}

void
outputeditors( newstring *tags, newstring *values, int numtags )
{
	char *editortags[] = { "ED  - ", "A2  - " };
	int  neditortags = sizeof(editortags)/sizeof(char*);
	int i,j,found=0;

	for (j=0; j<neditortags; ++j)
		for (i=0; i<numtags; ++i ) {
			if (strcasecmp(tags[i].data,editortags[j])==0) {
				if (found==0) {
					found=1;
					printf("      <EDITORS>\n");
				}
				outputnames( "EDITOR", values[i].data );
				found++;
			}
		}
	if ( found ) printf("      </EDITORS>\n");
}

void
outputseriesauthors( newstring *tags, newstring *values, int numtags )
{
	char *authortags[] = { "A3  - " };
	int  nauthortags = sizeof(authortags)/sizeof(char*);
	int i,j,found=0;

	for (j=0; j<nauthortags; ++j)
		for (i=0; i<numtags; ++i ) {
			if (strcasecmp(tags[i].data,authortags[j])==0) {
				if (found==0) {
					found=1;
					printf("      <SERIESAUTHORS>\n");
				}
				outputnames( "AUTHOR", values[i].data );
				found++;
			}
		}
	if ( found ) printf("      </SERIESAUTHORS>\n");
}

void
outputkeyword( char *value )
{
	newstring keyword;
	int i, len_value;

	newstr_init( &keyword );
	len_value = strlen( value );
	for ( i=0; i<len_value; ++i ) {
		if ( value[i]==';' ) {
			if (keyword.data) {
			printf("        <KEYWORD>%s</KEYWORD>\n",keyword.data );
			keyword.data[0]='\0';
			}
		} else {
			if ( keyword.data==NULL || strlen(keyword.data)>0 || 
			     (value[i]!=' ' && value[i]!='\t'))
				newstr_addchar( &keyword, value[i] );
		}
	}
	printf("        <KEYWORD>%s</KEYWORD>\n",keyword.data );
	newstr_free( &keyword );
}

/* potentially multiple keywords, separated by keywords */
void
outputkeywords( newstring *tags, newstring *values, int numtags )
{
	int i,found=0;

	for (i=0; i<numtags; ++i ) {
		if ((strcasecmp(tags[i].data,"KW  - ")==0) &&
		    (values[i].data!=NULL) &&
		    (values[i].data[0]!='\0') ) {
			if (found==0) {
				found=1;
				printf("      <KEYWORDS>\n");
			}
			outputkeyword( values[i].data );
			found++;
		}
	}
	if ( found ) printf("      </KEYWORDS>\n");
}



void
outputyear( newstring *tags, newstring *values, int numtags )
{
	char *checktags[]={"PY  - ", "Y1  - ", "Y2  - "};
	int  nchecktags = sizeof(checktags)/sizeof(char*);
	int  i,j,found=0;
	char *p;
	for (j=0; j<nchecktags && !found; ++j) {
		for (i=0; i<numtags; ++i) {
			if (strncmp(tags[i].data,checktags[j],6)==0) {
				found++;
				/* output year */
				printf("      <DATE>");
				printf("<YEAR>");
				p = values[i].data;
				while ( p && *p && *p!='/') printf("%c",*p++);
				printf("</YEAR>");
				/* output month */
				if ( p && *p=='/' ) p++;
				while ( p && *p && (*p==' ' || *p=='\t' || *p=='\r' || *p=='\n')) p++;
				if ( p && *p && *p!='/' ) {
					printf("<MONTH>");
					while ( p && *p && *p!='/' ) 
						printf("%c",*p++);
					printf("</MONTH>");
				}
				/* output day */
				if ( p && *p=='/' ) p++;
				while ( p && *p && (*p==' ' || *p=='\t' || *p=='\r' || *p=='\n')) p++;
				if ( p && *p && *p!='/' ) {
					printf("<DAY>");
					while ( p && *p && *p!='/' ) 
						printf("%c",*p++);
					printf("</DAY>");
				}
				/* output other */
				if ( p && *p=='/' ) p++;
				while ( p && *p && (*p==' ' || *p=='\t' || *p=='\r' || *p=='\n')) p++;
				if ( p && *p && *p!='/' ) {
					printf("<OTHER>");
					while ( p && *p && *p!='/' ) 
						printf("%c",*p++);
					printf("</OTHER>");
				}
				printf("</DATE>\n");
			}
		}
	}
}

/* JF - Periodical name, full format
 * JO/JA - Standard abbreviation
 * J1/J2 - User-defined abbreviation
 * preferentially output JO/JA abbreviated form if present
 */
void
outputjournal( newstring *tags, newstring *values, int numtags )
{
	char *checktags[]={"J1  - ", "J2  - ", "JO  - ", "JA  - ", "JF  - "};
	int  nchecktags = sizeof(checktags)/sizeof(char*);
	int  i,j,found=0;

	for ( j=0; j<nchecktags && found==0; ++j ) {
		for ( i=0; i<numtags; ++i ) {
			if ((strcasecmp(tags[i].data,checktags[j])==0) &&
			    (values[i].data!=NULL) &&
			    (values[i].data[0]!='\0') ) {
				found++;
				printf("      <JOURNAL>%s</JOURNAL>\n",
						values[i].data );
			}
		}
	}
}

/* IS, CP -- issue */
void
outputissue( newstring *tags, newstring *values, int numtags )
{
	char *checktags[] = {"IS  - ", "CP  - "};
	int  nchecktags = sizeof(checktags)/sizeof(char*);
	int  i,j,found=0;

	for ( j=0; j<nchecktags && found==0; ++j ) {
		for ( i=0; i<numtags; ++i ) {
			if ((strcasecmp(tags[i].data,checktags[j])==0) &&
			    (values[i].data!=NULL) &&
			    (values[i].data[0]!='\0') ) {
				found++;
				printf("      <ISSUE>%s</ISSUE>\n",
						values[i].data );
			}
		}
	}
}

/* Primary title */
void
outputtitle( newstring *tags, newstring *values, int numtags )
{
	char *checktags[] = {"T1  - ", "TI  - ", "CT  - "};
	int  nchecktags = sizeof(checktags)/sizeof(char*);
	int  i,j,found=0;

	for ( j=0; j<nchecktags && found==0; ++j ) {
		for ( i=0; i<numtags; ++i ) {
			if ((strcasecmp(tags[i].data,checktags[j])==0) &&
			    (values[i].data!=NULL) &&
			    (values[i].data[0]!='\0') ) {
				found++;
				printf("      <TITLE>%s</TITLE>\n",
						values[i].data );
			}
		}
	}
}

/* Abstract */
void
outputabstract( newstring *tags, newstring *values, int numtags )
{
	char *checktags[] = {"AB  - ", "N2  - "};
	int  nchecktags = sizeof(checktags)/sizeof(char*);
	int  i,j,found=0;

	for ( j=0; j<nchecktags && found==0; ++j ) {
		for ( i=0; i<numtags; ++i ) {
			if ((strcasecmp(tags[i].data,checktags[j])==0) &&
			    (values[i].data!=NULL) &&
			    (values[i].data[0]!='\0') ) {
				if (found==0) 
				  printf("      <ABSTRACT>%s</ABSTRACT>\n",
						values[i].data );
				else
				  printf("      <NOTES2>%s</NOTES2>\n",
						values[i].data );
				found++;
			}
		}
	}
}

void
outputrefnumfromnamedate( newstring *tags, newstring *values, int numtags )
{
	int i, year=-1,name=-1;
	char *p;
	for (i=0; i<numtags && (year==-1 || name==-1); ++i ) {
		if (year==-1 && (strncmp(tags[i].data,"PY  - ",6)==0 ||
			         strncmp(tags[i].data,"Y1  - ",6)==0) )
			year = i;
		else if (name==-1 && (
			strncmp(tags[i].data,"AU  - ",6)==0 ||
			strncmp(tags[i].data,"A1  - ",6)==0)) 
			name = i;
	}
	if (year!=-1 && name!=-1) {
		printf("      <REFNUM>");
		p = values[name].data;
		while ( p && *p && *p!=',' && *p!=' ' && *p!=';' && *p!='\t' &&
		       	*p!='\r' && *p!='\n') printf("%c",*p++);
		p = values[year].data;
		while ( p && *p && *p!=',' && *p!='/' && *p!='\t' && *p!='\r'
			&& *p!='\n') printf("%c",*p++);
		printf("</REFNUM>\n");
	}
}

void
outputrefnum( newstring *tags, newstring *values, int numtags )
{
	int found=0, i;
	for (i=0; i<numtags; ++i) {
		if ((strncmp(tags[i].data,"ID  - ",6)==0) && 
		    (values[i].data!=NULL) &&
		    (values[i].data[0]!='\0') ) {
			printf("      <REFNUM>%s</REFNUM>\n",
					values[i].data);
			found = 1;
		}
	}
	if (!found) outputrefnumfromnamedate(tags,values,numtags);
}

void
outputeasy( newstring *tags, newstring *values, int numtags, 
	char *ristag, char *xmltag)
{
	int i;
	for (i=0; i<numtags; ++i ) {
		if (strncmp(tags[i].data,ristag,6)==0 && (values[i].data!=NULL)
				&& (values[i].data[0]!='\0')){
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
	outputtitle( tags, values, numtags );
	outputeasy( tags, values, numtags, "T2  - ","SECONDARYTITLE");
	outputeasy( tags, values, numtags, "T3  - ","SERIESTITLE");
	outputeasy( tags, values, numtags, "BT  - ","BOOKTITLE");
	outputjournal( tags, values, numtags );
	outputyear( tags, values, numtags );
	outputeasy( tags, values, numtags, "VL  - ","VOLUME");
	outputissue( tags, values, numtags );
	outputpages( tags, values, numtags );
	outputeditors( tags, values, numtags );
	outputseriesauthors( tags, values, numtags );
	outputeasy( tags, values, numtags, "PB  - ","PUBLISHER");
	outputeasy( tags, values, numtags, "CT  - ","ADDRESS");
	outputeasy( tags, values, numtags, "AD  - ","ADDRESS");
	outputeasy( tags, values, numtags, "CY  - ","ADDRESS");
	outputeasy( tags, values, numtags, "SN  - ","SERIALNUM");
	outputeasy( tags, values, numtags, "RP  - ","REPRINTSTATUS");
	outputeasy( tags, values, numtags, "UR  - ","URL");
	outputeasy( tags, values, numtags, "N1  - ","NOTES");
	outputabstract( tags, values, numtags );
	outputkeywords( tags, values, numtags );
	outputrefnum( tags, values, numtags );

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
	while ( buf[i]==' ' || buf[i]=='\t' ) i++;
	while ( buf[i]!='\0' && buf[i]!='\t' && buf[i]!='\r' && buf[i]!='\n'){
		newstr_addchar( value, buf[i] );
		i++;
	}
}

/* RIS definition of a tag is strict:
    character 1 = uppercase alphabetic character
    character 2 = uppercase alphabetic character or digit
    character 3 = space (ansi 32)
    character 4 = space (ansi 32)
    character 5 = dash (ansi 45)
    character 6 = space (ansi 32)
*/
int
is_tag( char *buf )
{
	if (! (buf[0]>='A' && buf[0]<='Z') ) return FALSE;
	if (! (((buf[1]>='A' && buf[1]<='Z'))||(buf[1]>='0'&&buf[1]<='9')) ) return FALSE;
	if (buf[2]!=' ') return FALSE;
	if (buf[3]!=' ') return FALSE;
	if (buf[4]!='-') return FALSE;
	if (buf[5]!=' ') return FALSE;
	return TRUE;
}

long 
get_refs( FILE *fp )
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
		if ( buf[i]=='\r' || buf[i]=='\n' ) i++;

		/* Each reference starts with 'TY  - ' && ends with 'ER  - ' */
		while( buf[i]!='\0' ) {
			if (is_tag(&(buf[i]))) {
				if (strncmp(&(buf[i]),"TY  - ",6)==0) {
					if (inref==TRUE) {
						fprintf(stderr,"Error.  Reference %ld not properly terminated.\n",numrefs+1);
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
			/* not a tag, but we'll append to last values ...*/
			else if (inref) {
				int added=0;
				while ( buf[i] ) {
					if (buf[i]!='\r' && buf[i]!='\n') {
					   if (added==0) newstr_addchar(&(values[numtags-1]),' ');
					   newstr_addchar(&(values[numtags-1]),buf[i]);
					   added++;
					} 
					i++;
				}
			}
			while ( buf[i]!='\0' && buf[i]!='\t' && 
 				  buf[i]!='\r' && buf[i]!='\n' ) ++i;
			if (buf[i]=='\t'|| buf[i]=='\r' || buf[i]=='\n') ++i;
		}
	}

	/* clean up memory usage */
	for (i=0; i<maxtags; ++i) {
		newstr_free( &(tags[i]) );
		newstr_free( &(values[i]) );
	}
	free(tags);
	free(values);

	printf("</REFERENCES>\n");
	printf("</XML>\n");

	return numrefs;
}

void
help( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"\n%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	fprintf(stderr,"Converts a RIS reference file into XML\n\n");

	fprintf(stderr,"usage: %s ris_file > xml_file\n\n",progname);
        fprintf(stderr,"  ris_file can be replaced with file list or omitted to use as a filter\n\n");

	fprintf(stderr,"  -h, --help     display this help\n");
	fprintf(stderr,"  -v, --version  display version\n\n");

	fprintf(stderr,"http://www.scripps.edu/~cdputnam/bibutils.html for more details\n\n");
	exit( EXIT_SUCCESS );
}

void
tellversion( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	exit( EXIT_SUCCESS );
}

void
process_args( int *argc, char *argv[] )
{
	int i;
	for ( i=0; i<*argc; ++i ) {
		if ( strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0 ) {
			help();
			/* help terminates or we'd remove from argv */
		}
		if ( strcmp(argv[i],"-v")==0 || strcmp(argv[i],"--version")==0){
			tellversion();
			/* tellversion terminates or we'd remove from argv*/
		}
	}
}


int 
main(int argc, char *argv[])
{
	int 	i;
	FILE 	*fp;
	long 	numref = 0L;

	process_args( &argc, argv );

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
 

