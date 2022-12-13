#include <argp.h>

#ifndef PARSER_H
#define PARSER_H

#define FILE_BUF_SIZE			255

typedef struct {
	char *path_buf; // Buffer that contains the full path
	char *path_buf_ptr; // Pointer to buffer where file names should be written
	char *directory;
	char *initial_image;
	int randomize;
	int delay;
	int transition_delay;
	int ticks;
} Args;

error_t parse_opt(int key, char *arg, struct argp_state *state);
void parse_args(int argc, char **argv, Args *arguments);

#endif
