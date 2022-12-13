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
			arguments->delay = atoi(arg);
			if (arguments->delay == 0)
				argp_error(state, "Delay must be a non-zero number");
			break;
		case 't':
			arguments->transition_delay = atoi(arg);
			if (arguments->transition_delay == 0)
				argp_error(state, "Transition delay must be a non-zero number");
			break;
		case ARGP_KEY_ARG:
			// Too many arguments
      if (state->arg_num > 1)
        argp_usage(state);
			arguments->directory = arg;
      break;
    case ARGP_KEY_END:
			// Ticks is the total ticks per cycle
			arguments->ticks = arguments->delay + arguments->transition_delay;

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
		{ "delay", 'd', "SECS", 0, "Wallpaper duration" },
		{ "transition", 't', "SECS", 0, "Duration of transition between wallpapers" },
		{ 0 }
	};
	char doc[] = "Waal - X11 wallpaper using SDL";
	char args_doc[] = "DIRECTORY";

	struct argp parser = { options, parse_opt, args_doc, doc };
	argp_parse(&parser, argc, argv, 0, 0, arguments);
};
