#include "utils.h"
#include "parser.h"

static Display *dpy;
static int screens;
static int file_count;
static Args arguments;

SDL_Texture *capture_screen(Monitor *screen){
	XImage *capture = XGetImage(dpy, screen->root, 0, 0, screen->width, screen->height, AllPlanes, ZPixmap);

	// Create an SDL surface from the screenshot
	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
		capture->data, screen->width, screen->height, 32,
		capture->bytes_per_line,
		capture->red_mask, capture->green_mask, capture->blue_mask, 0
	);

	// Convert to a texture
	SDL_Texture *texture = SDL_CreateTextureFromSurface(screen->renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void setup_monitors(Monitor *monitors, int num_screens) {
	Monitor *monitor_ptr = monitors;
	for (int i=0; i<num_screens; i++){
		*monitor_ptr = (Monitor) {
			.root = RootWindow(dpy, i),
			.width = DisplayWidth(dpy, i),
			.height = DisplayHeight(dpy, i),
		};
		monitor_ptr->window = SDL_CreateWindowFrom((void*) monitor_ptr->root);
		monitor_ptr->renderer = SDL_CreateRenderer(monitor_ptr->window, -1, SDL_FLAGS);

		// Load the source image
		monitor_ptr->src = capture_screen(monitor_ptr);

		// Load the destination image
		if (arguments.initial_image != NULL)
			monitor_ptr->dst = IMG_LoadTexture(monitor_ptr->renderer, arguments.initial_image);
		else {
			random_file(arguments.directory, arguments.file_buf, FILE_BUF_SIZE);
			monitor_ptr->dst = IMG_LoadTexture(monitor_ptr->renderer, arguments.path);
		}
		monitor_ptr++;
	}
}

int count_files(char *path) {
	int count = 0;
	DIR *dir_ptr;
	struct dirent *ent_ptr;

	// Open the directory
	dir_ptr = opendir(path);
	if (dir_ptr == NULL)
		return -1;

	// Count number of entries
	while ((ent_ptr = readdir(dir_ptr)) != NULL)
		count++;

	// Don't count '.' and '..'
	return count - 2;
}

int random_file(char *path, char *buf, int size) {
	DIR *dir_ptr;
	struct dirent *ent_ptr;
	int random;
	int pos = 0;

	// Open the directory
	dir_ptr = opendir(path);
	if (dir_ptr == NULL)
		return -1;

	random = rand() % file_count;

	// Move pointer to the random file or until last entry
	while (pos++ < random + 2 && (ent_ptr = readdir(dir_ptr)) != NULL);

	// Copy name to buffer
	int len = strlen(ent_ptr->d_name);
	strncpy(buf, ent_ptr->d_name, len);
	buf[len] = '\0';

	closedir(dir_ptr);
	return 0;
}

int main(int argc, char **argv) {
	// Parse arguments
	arguments = (Args) {
		.directory = NULL,
		.initial_image = NULL,
		.randomize = 0,
		.duration = 1000,
		.fade = 50
	};
	parse_args(argc, argv, &arguments);

	// Initialization
	if (!(dpy = XOpenDisplay(NULL))){
		fprintf(stderr, "Could not open XDisplay\n");
		return 1;
	}
	screens = ScreenCount(dpy);

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Failed to initialize SDL");
		return 2;
	}

	if (IMG_Init(SDL_IMAGE_FLAGS) == 0) {
		fprintf(stderr, "Failed to initialize SDL_Image");
		return 3;
	}

	// Count files in the directory
	file_count = count_files(arguments.directory);

	// Setup each monitor
	Monitor *monitors = malloc(sizeof(Monitor) * screens);
	setup_monitors(monitors, screens);

	int counter = 0;
	double alpha_step = (double) MAX_ALPHA / arguments.fade;

	while(1){
		Monitor *monitor = monitors;
		for (int i=0; i<screens; i++) {
			// Copy dst onto src with increasing alpha
			if (counter < arguments.fade) {
				SDL_RenderCopy(monitor->renderer, monitor->src, NULL, NULL);

				SDL_SetTextureBlendMode(monitor->dst, SDL_BLENDMODE_BLEND);
				SDL_SetTextureAlphaMod(monitor->dst, counter*alpha_step);
				SDL_RenderCopy(monitor->renderer, monitor->dst, NULL, NULL);

			// Destroy src texture and show dst texture
			} else if (counter == arguments.fade) {
				SDL_DestroyTexture(monitor->src);
				monitor->src = NULL;
				SDL_RenderCopy(monitor->renderer, monitor->dst, NULL, NULL);

			// Setup for the next wallpaper
			} else if (counter == arguments.ticks - 1) {
				random_file(arguments.directory, arguments.file_buf, FILE_BUF_SIZE);
				monitor->src = monitor->dst;
				monitor->dst = IMG_LoadTexture(monitor->renderer, arguments.path);
				SDL_RenderCopy(monitor->renderer, monitor->src, NULL, NULL);
			
			// Display the wallpaper normally
			} else {
				SDL_RenderCopy(monitor->renderer, monitor->dst, NULL, NULL);
			}

			SDL_RenderPresent(monitor->renderer);

			monitor++;
		}

		// Event handling
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			break;

		counter = (counter + 1) % arguments.ticks;
	}

	// Cleanup
	free(arguments.path);
	SDL_Quit();
	IMG_Quit();
	XCloseDisplay(dpy);

	return 0;
}
