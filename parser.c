#include "parser.h"
#include <stdlib.h>
#include <string.h>

const char *argp_program_version = "1.0";
const char *argp_program_bug_address = "<trananson@protonmail.com>";

error_t parse_opt(int key, char *arg, struct argp_state *state) {
	Args *arguments = state->input;
	switch (key) {
		case 'r':
			arguments->randomize = 1;
			break;
		case 'i':
			arguments->initial_image = arg;
			break;
		case 'd':
			arguments->duration = atoi(arg);
			if (arguments->duration == 0)
				argp_error(state, "Duration must be a non-zero number");
			break;
		case 'f':
			arguments->fade = atoi(arg);
			if (arguments->fade == 0)
				argp_error(state, "Duration must be a non-zero number");
			break;
		case ARGP_KEY_ARG: // Non-optioned arguments
      if (state->arg_num > 1)
        argp_usage(state);
			arguments->directory = arg;
      break;
    case ARGP_KEY_END: // End of argument list
			if (arguments->directory == NULL)
				argp_usage(state);

			// Ticks is the total ticks per cycle
			arguments->ticks = arguments->duration + arguments->fade;

			int dir_len = strlen(arguments->directory);

			// Copy directory string to the buffer
			arguments->path = malloc((dir_len + FILE_BUF_SIZE + 1)*sizeof(char));
			strncpy(arguments->path, arguments->directory, dir_len);
			arguments->path[dir_len] = '/';
			arguments->path[dir_len+1] = '\0';

			// Terminate string and set pointer to the end of the buffer
			arguments->file_buf = arguments->path + dir_len + 1;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

void parse_args(int argc, char **argv, Args *arguments) {
	struct argp_option options[] = {
		{ "randomize", 'r', 0, 0, "Cycle through wallpapers at random" },
		{ "initial", 'i', "FILE", 0, "Choose a starting wallpaper" },
		{ "duration", 'd', "SECS", 0, "Wallpaper duration" },
		{ "fade", 'f', "SECS", 0, "Fade duration between wallpapers" },
		{ 0 }
	};
	char doc[] = "wallpaper - Simple X11 wallpaper script using SDL2";
	char args_doc[] = "DIRECTORY";

	struct argp parser = { options, parse_opt, args_doc, doc };
	argp_parse(&parser, argc, argv, 0, 0, arguments);
};
