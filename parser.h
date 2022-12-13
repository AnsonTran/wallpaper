#include <argp.h>

#ifndef PARSER_H
#define PARSER_H

typedef struct {
	char *directory;
	char *initial_image;
	int randomize;
	int delay;
	int transition_delay;
} Args;

error_t parse_opt(int key, char *arg, struct argp_state *state);
void parse_args(int argc, char **argv, Args *arguments);

#endif
