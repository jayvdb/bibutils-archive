/*
 * medline to xml
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "newstr.h"
#include "strsearch.h"
#include "is_ws.h"

#define TRUE  1
#define FALSE 0
#define FIELD_OTHER    0
#define FIELD_AUTHORS  1
#define FIELD_TITLE    2
#define FIELD_JOURNAL  3
#define FIELD_DATE     4
#define FIELD_ABSTRACT 5

int abstractout = FALSE;
char progname[] = "med2xml";
char version[] = "version 1.7 11/03/03";

int
endofline (char ch)
{
	if (ch=='\n' || ch=='\r') return TRUE;
	else return FALSE;
}

void 
output_abbrev (newstring *lastnamesptr, newstring *sourceptr)
{
	char *p;
	int pos;
	newstring abbrev;

	printf("  <REFNUM>");

	newstr_init(&abbrev);

	/** Get the first author's last name w/o spaces **/
	if ( lastnamesptr[0].len==0 || sourceptr->len==0 ) p = NULL;
	else p = lastnamesptr[0].data;
	while ( p && *p ) {
		if (!is_ws(*p) && *p!='{' && *p!='}' ) 
			newstr_addchar(&abbrev,*p);
		p++;
	}
	if ( abbrev.len>0 ) printf("%s",abbrev.data);
	else printf("REF");

	/** Output the year **/
	if (sourceptr!=NULL && sourceptr->data!=NULL) {
		p=strchr(sourceptr->data,'.');
		if ( p ) { 
			p++;
			while (*p && is_ws(*p)) p++;
			pos=1;
			while (*p && !is_ws(*p) && pos<5) {
				printf("%c",*p);
				p++;
				pos++;
			}
		}
	}
	newstr_free(&abbrev);
	printf("</REFNUM>\n");
}

void
output_authors( newstring* lastnames, newstring* initials, int numauthors )
{
	int i, j;
	if ( numauthors<1 ) return;
	printf("  <AUTHORS>\n");
	for (i=0; i<numauthors; ++i) {
		printf("    <AUTHOR>");
		if (lastnames[i].len>0 ) {
			newstr_encodexml( &(lastnames[i]) );
			printf("<LAST>%s</LAST>",lastnames[i].data);
		}
		if (initials[i].len>0 )
			for (j=0; j<initials[i].len; j++)
				printf("<PREF>%c.</PREF>",initials[i].data[j]);
		printf("</AUTHOR>\n");
	}
	printf("  </AUTHORS>\n");
}


int
generate_authors (newstring *authorsptr, newstring** lastnamesptr, newstring** firstnamesptr)
{
  char *p,*q;
  int est,i,nauthor;
  int nblocks, dimblocks=0;
  newstring wholename;
  newstring *blocks     = NULL;
  newstring *firstnames = NULL;
  newstring *lastnames  = NULL;

  if (authorsptr==NULL || authorsptr->data==NULL) return 0;
/*
fprintf(stderr,"AUTHORLIST: '%s'\n",authorsptr->data);
*/

  /* Estimate the initial number of authors */
    est=1;
    p=authorsptr->data;
    while (*p) if (*p++==',') est++;

  /* Allocate arrays */
    lastnames  = (newstring*) malloc (sizeof(newstring)*est);
    firstnames = (newstring*) malloc (sizeof(newstring)*est);
    if (lastnames==NULL || firstnames==NULL) {
        if (lastnames!=NULL) free(lastnames);
        if (firstnames!=NULL) free(firstnames);
        lastnames = firstnames = NULL;
        return 0;
    }
    for (i=0; i<est; ++i) {
        newstr_init(&(lastnames[i]));
        newstr_init(&(firstnames[i]));
    }
    newstr_init(&wholename);

  /* For the entire list do... */
    p=authorsptr->data;
    nauthor=0;
    while (*p) {
       
       while (*p && *p!=',') newstr_addchar(&wholename,*p++);
       nblocks=0;
       q = wholename.data;
       while (*q) {
           while (*q && is_ws(*q)) q++;
           if (*q && !is_ws(*q)) nblocks ++;
           while (*q && !is_ws(*q)) q++;
       }
       if ( nblocks > dimblocks ) {
            newstring *newblocks;
            newblocks = (newstring*) realloc(blocks,sizeof(newstring)*nblocks);
            if (newblocks==NULL) {
               fprintf(stderr,"med2bib: cannot allocate memory in generate_authors().\n");
               exit(EXIT_FAILURE);
            }
            else {
               blocks = newblocks;
               for (i=dimblocks; i<nblocks; ++i) {
                   newstr_init(&(blocks[i]));
               }
               dimblocks = nblocks;
            }
       }

       for (i=0; i<nblocks; ++i) newstr_free(&(blocks[i]));
       nblocks=0;
       q = wholename.data;
       while (*q) {
           while (*q && is_ws(*q)) q++;
           while (*q && !is_ws(*q)) {
              newstr_addchar(&(blocks[nblocks]),*q);
              q++;
           }
           while (*q && is_ws(*q)) q++;
           nblocks++;
       }

       /* Handle suffixes */
       if (strcmp(blocks[nblocks-1].data,"3rd")==0 ||
           strcmp(blocks[nblocks-1].data,"3d")==0  ||
           strcmp(blocks[nblocks-1].data,"Jr")==0  ||
           strcmp(blocks[nblocks-1].data,"Sr")==0  ||
           strcmp(blocks[nblocks-1].data,"III")==0 ||
           strcmp(blocks[nblocks-1].data,"II")==0 ) {
           newstring tmp;
           if ( nblocks-2 >= 0 ) {
              tmp=blocks[nblocks-1];
              blocks[nblocks-1]=blocks[nblocks-2];
              blocks[nblocks-2]=tmp;
           }
       }
       if (nblocks>2) newstr_addchar(&(lastnames[nauthor]),'{');
       for (i=0; i<nblocks-1; ++i) {
            newstr_strcat(&(lastnames[nauthor]),blocks[i].data);
            if (nblocks>2 && i<nblocks-2) newstr_addchar(&(lastnames[nauthor]),' ');
       }
       if (nblocks>2) newstr_addchar(&(lastnames[nauthor]),'}');
       if (nblocks>1) {
          newstr_strcat(&(firstnames[nauthor]),blocks[nblocks-1].data);
       }
      newstr_free(&wholename);
      nauthor++;
      if (*p) p++;
  }

  for ( i=0; i<dimblocks; ++i ) newstr_free(&(blocks[i]));
  free(blocks);

  *lastnamesptr = lastnames;
  *firstnamesptr = firstnames;
  return est;

}

void
free_authors( newstring **lastnames, newstring **firstnames, int numauthors)
{
	int i;
	for ( i=0; i<numauthors; ++i ) {
		newstr_free(&((*lastnames)[i]));
		newstr_free(&((*firstnames)[i]));
	}
	free(*lastnames);
	free(*firstnames);
	*lastnames = *firstnames = NULL;
}

void
output_title( newstring *title )
{
	newstr_encodexml( title );
	printf("  <TITLE>%s</TITLE>\n",title->data);
}

char *
output_journal( char *p )
{
	newstring journal;
	if ( !p || !(*p) ) return p;
	newstr_init(&journal);
	while (*p && is_ws(*p)) p++;
	while (*p && *p!='.') {
		if (!is_ws(*p))
			newstr_addchar(&journal,*p++);
		else {
			newstr_addchar(&journal,' ');
			while (is_ws(*p)) p++;
		}
	}
	if (journal.len>0) {
		newstr_encodexml( &journal );
		printf("  <JOURNAL>%s</JOURNAL>\n",journal.data);
	}
	newstr_free(&journal);
	return p;
}

char *
output_date( char *p )
{
	char *months[]={ "Jan","Feb","Mar","Apr","May","Jun",
		"Jul","Aug","Sep","Oct", "Nov", "Dec" };
	int i;
	char *savep = p;

	if ( !p || !(*p) ) return p;
	p = strchr(p,'.');
	if ( p ) {
		p++;
		while (*p && is_ws(*p)) p++;
		printf("  <DATE><YEAR>");
		while (*p && !is_ws(*p) && *p!='(' && *p!=':' && *p!='.' && *p!=',' && *p!=';') {
			printf("%c",*p++);
		} 
		printf("</YEAR>");
		while (*p && is_ws(*p)) p++;
		if ( *p && *p!=';' && *p!=':' && *p!=',' && *p!='(' && *p!='.'){
			for (i=0; i<12; ++i)
				if (strsearch(p,months[i])==p) 
					printf("<MONTH>%d</MONTH>",i+1);
			while (*p && *p!=';' &&  *p!=':' && *p!=',' && *p!='(' && *p!='.' && !is_ws(*p)) p++;
			while (*p && is_ws(*p)) p++;
		}
		if ( *p && *p!=';' && *p!=':' && *p!=',' && *p!='(' && *p!='.'){
			printf("<DAY>");
			while (*p && *p!=';' && *p!=':' && *p!=','  && *p!='(' && *p!='.') printf("%c",*p++);
			printf("</DAY>");
		}
		printf("</DATE>\n");
		return p;
	} else return savep;
}

char *
output_volume( char *p )
{
	newstring volume;
	char *savep = p;
	if ( !p || !(*p) ) return p;
	p = strchr(p,';');
	if ( p ) {
		newstr_init( &volume );
		p++;
		while ( *p && is_ws(*p) ) p++;
		while (*p && !is_ws(*p) && *p!='(' && *p!=',' && *p!='.' && *p!=':')
			newstr_addchar( &volume, *p++ );
		if ( volume.len > 0 ) {
			newstr_encodexml( &volume );
			printf("  <VOLUME>%s</VOLUME>\n",volume.data);
		}
		newstr_free( &volume );
		return p;
	} else return savep;
}

char *
output_number( char *p ) 
{
	newstring number;
	char *savep = p;
	if ( !p || !(*p) ) return p;
	p = strchr( savep, '(' );
	if ( p ) {
		newstr_init( &number );
		p++;
		while ( *p && is_ws(*p)) p++;
		while ( *p && !is_ws(*p) && *p!=')' )
			newstr_addchar( &number, *p++ );
		if ( number.len > 0 ) {
			newstr_encodexml( &number );
			printf("  <NUMBER>%s</NUMBER>\n",number.data);
		}
		newstr_free( &number );
		return p;
	} else return savep;
}

char *
output_pages( char *p )
{
	newstring pgbegin, pgend;
	char *savep = p;
	int pos;
	if ( !p || !(*p) ) return p;
	p = strchr(p,':');
	if ( p ) {
		p++; /*skip ':' character*/
		while (*p && is_ws(*p)) p++;
		printf("  <PAGES>");
		newstr_init(&pgbegin);
		while (*p && !is_ws(*p) && *p!=',' && *p!='-' && *p!='.')
			newstr_addchar(&pgbegin,*p++);
		if ( pgbegin.len>0 ) {
			newstr_encodexml( &pgbegin );
			printf("<START>%s</START>",pgbegin.data);
		}
		while ( is_ws(*p) || *p=='-' ) p++;
		newstr_init(&pgend);
		while (*p && !is_ws(*p) && *p!=',' && *p!='-' && *p!='.')
			newstr_addchar(&pgend,*p++);
		newstr_encodexml( &pgend );
		if ( pgend.len>0 ) {
			int diff = pgbegin.len - pgend.len;
			if ( diff <= 0 )
				printf("<END>%s</END>",pgend.data);
			else {
				printf("<END>");
				for (pos=0; pos<diff; pos++ )
					printf("%c",pgbegin.data[pos]);
				printf("%s</END>",pgend.data);
			}
		}
		newstr_free (&pgbegin);
		newstr_free (&pgend);
		printf("</PAGES>\n");
		return p;
	} else return savep;
}

void
output_source( newstring *source )
{
	char *p;
	if ( !source || source->len==0 ) return;
	p = output_journal( source->data );
	p = output_date( p );
	p = output_volume( p );
	p = output_number( p );
	p = output_pages( p );
}

void 
output_fields (newstring *authorsptr, newstring *titleptr, 
               newstring *sourceptr, newstring *abstractptr)
{
	newstring *lastnames, *initials;
	int numauthors;

	printf("<REF>\n");
	printf("  <TYPE>ARTICLE</TYPE>\n");
	numauthors = generate_authors(authorsptr,&lastnames,&initials);
	output_authors(lastnames,initials,numauthors);
	output_title(titleptr); 
	output_source(sourceptr); 
	output_abbrev(lastnames,sourceptr); 
	printf("</REF>\n");
	free_authors(&lastnames,&initials,numauthors); 
}

long
get_fields( FILE *fp )
{
	newstring authors,title,journal,date,source,abstract;
	long numref = 0L;
	char buf[512], titlestartch='\0';
	int field_id = FIELD_OTHER, startfield, i;

	newstr_init(&authors);
	newstr_init(&title);
	newstr_init(&journal);
	newstr_init(&date);
	newstr_init(&source);
	newstr_init(&abstract);

  while (!feof(fp)) {
    if (fgets(buf,sizeof(buf),fp)) {
      i = 0;

        while (i<sizeof(buf) && buf[i]!='\0') {
 
/*
fprintf(stderr,"field_id= %d i=%d\n",field_id,i);
fprintf(stderr,"BUF: '%s'\n",buf);
*/
 
        /*Keep parsing until we reach a number starting a line and then a ':'*/
        if (field_id == FIELD_OTHER && buf[i]!='\0') {

/*
                while (buf[i] && (! ( buf[i]>='0' && buf[i]<='9'))) {
printf("loop 1: %d '%c'\n",buf[i],buf[i]);
++i;
}
*/
                while (buf[i] && (! ( buf[i]==':' && (i>0) && 
		    isdigit(buf[i-1]) ) ) )  ++i;
                if (buf[i]==':') {
                        i++;
                        while (is_ws(buf[i])) ++i;
                        field_id = FIELD_AUTHORS;
                }
        }

        if (field_id == FIELD_AUTHORS && buf[i]!='\0') {
                /* while (is_ws(buf[i])) ++i; */
                startfield = i;
                while (buf[i]!='\r' && buf[i]!='\n' && buf[i]!='\0' && 
			buf[i]!='.' && buf[i]!=';') ++i;
                if (buf[i]=='\0') newstr_strcat(&authors,&(buf[startfield]));
                else {
			if (buf[i]=='.') {
				buf[i] = '\0';
				field_id=FIELD_TITLE;
			} else if ( buf[i]==';' ) {
				buf[i] = '\0';
				i++;
				while ( buf[i]!='\0' && buf[i]!='.' ) i++;
				field_id=FIELD_TITLE;
			} else {
				/* remove the newline and paste */
				buf[i] = '\0';
				if ( buf[i+1] == '\r' || buf[i+1] == '\n' ) 
					buf[i+1]=' ';
			}
                        newstr_strcat(&authors,&(buf[startfield]));
/*
fprintf(stderr,"buf[startfield]: '%s'\n",&(buf[startfield]));
fprintf(stderr,"Authors: '%s'\n",authors.data);
*/
                        i++;
                        if (field_id==FIELD_AUTHORS) newstr_addchar(&authors,' ');
                        else while (is_ws(buf[i])) i++;
                }
        }

        if (field_id == FIELD_TITLE && buf[i]!='\0') {
		while ( titlestartch=='\0' && is_ws(buf[i]) ) ++i;
                startfield = i;
		if ( titlestartch=='\0' ) titlestartch = buf[i];
                while (buf[i]!='\r' && buf[i]!='\n' && buf[i]!='\0') ++i;
                if (buf[i]=='\0') newstr_strcat(&title,&(buf[startfield]));
                else {
			buf[i] = '\0';
			if ( i>0 && ((buf[i-1]=='.' || buf[i-1]=='?' ) ||
			     (titlestartch=='[' && buf[i-1]==']'))) {
				if (buf[i-1]=='.') buf[i-1] = '\0';
				field_id = FIELD_JOURNAL;
				titlestartch='\0';
			} 
			if (buf[i+1]=='\r' || buf[i+1]=='\n') buf[i+1]=' ';
                        newstr_strcat(&title,&(buf[startfield]));
/*
fprintf(stderr,"buf: '%s'\n",&(buf[startfield]));
fprintf(stderr,"Title: '%s'\n",title.data);
*/
                        i++;
			if (field_id==FIELD_TITLE) newstr_addchar(&title,' ');
			else while (is_ws(buf[i])) ++i;
                }
        }

        if (field_id == FIELD_JOURNAL && buf[i]!='\0') {
                /* while (is_ws(buf[i])) ++i; */
                startfield = i;
                while ( buf[i]!='\r' && buf[i]!='\n' && buf[i]!='.' && buf[i]!='\0' ) ++i;
                if (buf[i]=='\0') newstr_strcat(&journal,&(buf[startfield]));
                else {
			if (buf[i]=='.') {
				buf[i] = '\0';
				field_id = FIELD_DATE;
			} else {
	                        /* remove the newline and paste */
				buf[i] = '\0';
				if (buf[i+1]=='\r' || buf[i+1]=='\n') 
					buf[i+1]=' ';
                        }
                        newstr_strcat(&journal,&(buf[startfield]));
                        i++;
                        if (field_id==FIELD_JOURNAL) newstr_addchar(&title,' ');
                        else while (is_ws(buf[i])) ++i;
		}
	}
        if (field_id == FIELD_DATE && buf[i]!='\0') {
                /* while (is_ws(buf[i])) ++i; */
                startfield = i;
                while ( buf[i]!='\r' && buf[i]!='\n' && buf[i]!='.' && buf[i]!='\0' ) ++i;
                if (buf[i]=='\0') newstr_strcat(&date,&(buf[startfield]));
                else {
			if (buf[i]=='.') {
				buf[i] = '\0';
				field_id = FIELD_OTHER;
			} else {
	                        /* remove the newline and paste */
				buf[i] = '\0';
				if (buf[i+1]=='\r' || buf[i+1]=='\n') 
					buf[i+1]=' ';
                        }
                        newstr_strcat(&date,&(buf[startfield]));
                        i++;
                        if (field_id==FIELD_DATE) newstr_addchar(&title,' ');
                        else while (is_ws(buf[i])) ++i;
                        /* Now process this reference and reset all */
                        if (field_id==FIELD_OTHER){
				newstr_strcpy(&source,journal.data);
				newstr_addchar(&source,'.');
				newstr_strcat(&source,date.data);
                                output_fields(&authors,&title,&source,&abstract);
				numref++;
                                fflush(stdout);
                                newstr_empty(&authors);
                                newstr_empty(&title);
				newstr_empty(&journal);
				newstr_empty(&date);
                                newstr_empty(&source);
                                newstr_empty(&abstract);
                        }
                }
        }
        }
    }

}

	newstr_free(&authors);
	newstr_free(&title);
	newstr_free(&journal);
	newstr_free(&date);
	newstr_free(&source);
	newstr_free(&abstract);

	return numref;
}


void
help( void )
{
	extern char bibutils_version[];
	fprintf(stderr,"\n%s version %s, ",progname,version);
	fprintf(stderr,"bibutils suite version %s\n",bibutils_version);
	fprintf(stderr,"Converts a medline text file into XMLx\n\n");

	fprintf(stderr,"usage: %s med_file > xml_file\n\n",progname);
        fprintf(stderr,"  med_file can be replaced with file list or omitted to use as a filter\n\n");

/*	fprintf(stderr,"  -a             include abstracts\n"); */
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
process_args( int *argc_ptr, char *argv[] )
{
	int found = FALSE;
	int i = 1,j;
	while ( i < *argc_ptr && !found ) {
		if (strncmp(argv[i],"-a",2)==0) {
			found = TRUE;
			abstractout = TRUE;
			for (j=i+1; j<*argc_ptr; j++) 
				argv[j-1]=argv[j];
			i--;
			*argc_ptr = (*argc_ptr) - 1;
		}
		else if (strcmp(argv[i],"-h")==0||strcmp(argv[i],"--help")==0){
			help();
			/* if help didn't terminate, we'd remove */
		}
		else if (strcmp(argv[i],"-v")==0||strcmp(argv[i],"--version")==0){
			tellversion();
			/* if version didn't terminate, we'd remove */
		}
		i++;
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
		numref=get_fields(stdin);
	else {
		printf("<XML>\n<REFERENCES>\n");
		for (i=1; i<argc; i++) {
			fp=fopen(argv[i],"r");
			if (fp==NULL) {
				fprintf(stderr,"%s: cannot open %s\n",
					progname, argv[i]);
			} else { 
				numref+=get_fields(fp);
				fclose(fp);
			}
		}
		printf("</REFERENCES>\n</XML>\n");
	}
	fprintf(stderr,"%s %s:  Processed %ld references.\n", progname,
			version, numref);
	return EXIT_SUCCESS;
}
 

