#ifndef ARGS_H
#define ARGS_H

extern void args_tellversion( char *progname );
extern int args_match( char *check, char *shortarg, char *longarg );
extern void process_charsets( int *argc, char *argv[], param *p,
	int use_input, int use_output );

#endif
