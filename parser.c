#include "parser.h"
#include <stdlib.h>

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
      if (state->arg_num < 1)
        argp_usage(state);
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
		{ "delay", 'd', "SECS", 0, "Seconds before transitioning to the next wallpaper" },
		{ "transition", 't', "SECS", 0, "How long each transition takes. Must be less than the delay" },
		{ 0 }
	};
	char doc[] = "Waal - X11 wallpaper using SDL";
	char args_doc[] = "DIRECTORY";

	struct argp parser = { options, parse_opt, args_doc, doc };
	argp_parse(&parser, argc, argv, 0, 0, arguments);
};
