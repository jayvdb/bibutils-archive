/*
 * tree.h
 */

struct node {
	newstring tag;
	newstring value;
	struct node *nodes[30];
	int nnodes;
};

typedef struct node node;

extern node *node_new( void );
extern node *node_build( char *tag, char *interior );
extern void  node_output( FILE *outptr, node *currnode );
extern void  node_findreplace( findreplace *list, node *topnode );
void         node_free( node *currnode );

		

