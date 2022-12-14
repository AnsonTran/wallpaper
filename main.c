#include "utils.h"
#include "parser.h"

static Display *dpy;
static int screens;

void captureScreen(Monitor *screen){
	XImage *capture = XGetImage(dpy, screen->root, 0, 0, screen->width, screen->height, AllPlanes, ZPixmap);

	// Create an SDL surface from the screenshot
	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
		capture->data, screen->width, screen->height, 32,
		capture->bytes_per_line,
		capture->red_mask, capture->green_mask, capture->blue_mask, 0
	);

	// Convert to a texture
	screen->image = SDL_CreateTextureFromSurface(screen->renderer, surface);
	SDL_FreeSurface(surface);
}

void setupMonitors(Monitor *monitors, int num_screens) {
	for (int i=0; i<num_screens; i++){
		monitors[i] = (Monitor) {
			.root = RootWindow(dpy, i),
			.width = DisplayWidth(dpy, i),
			.height = DisplayHeight(dpy, i),
		};
		monitors[i].window = SDL_CreateWindowFrom((void*) monitors[i].root);
		monitors[i].renderer = SDL_CreateRenderer(monitors[i].window, -1, SDL_FLAGS);

		// Scrape the root window into a texture
		captureScreen(&monitors[i]);
	}
}

void randomFile(char *path, char *buf, int size) {
	int count = 0;
	DIR *dPtr;
	struct dirent *ePtr;
	int random;

	// Count number of entries
	dPtr = opendir(path);
	while ((ePtr = readdir(dPtr)) != NULL) {
		count++;
	};
	count = count - 2; // Factor out '.' and '..'

	random = rand() % count;
	rewinddir(dPtr);
	for (int i=0; i < random + 2; i++)
		ePtr = readdir(dPtr);

	// Copy name to buffer
	int len = strlen(ePtr->d_name);
	strncpy(buf, ePtr->d_name, len);
	buf[len] = '\0';

	closedir(dPtr);
	return;
}

int main(int argc, char **argv) {
	// Parse arguments
	Args arguments = {
		.directory = NULL,
		.initial_image = NULL,
		.randomize = 0,
		.duration = 1000,
		.fade = 50
	};
	parse_args(argc, argv, &arguments);

	// Initialization
	dpy = XOpenDisplay(NULL); // X11
	if (!dpy)
		fprintf(stderr, "Could not open XDisplay\n"); return 1;
	screens = ScreenCount(dpy);

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		fprintf(stderr, "Failed to initialize SDL"); return 2;// SDL
	if (IMG_Init(SDL_IMAGE_FLAGS) != 0)
		fprintf(stderr, "Failed to initialize SDL_Image"); return 3;// SDL_image

	// Get information about screen setup
	Monitor *monitors = malloc(sizeof(Monitor) * screens);
	setupMonitors(monitors, screens);

	// Load images into renderer
	SDL_Texture *src = monitors[0].image;
	SDL_Texture *dst;

	if (arguments.initial_image != NULL)
		dst = IMG_LoadTexture(monitors[0].renderer, arguments.initial_image);
	else {
		randomFile(arguments.directory, arguments.file_buf, FILE_BUF_SIZE);
		dst = IMG_LoadTexture(monitors[0].renderer, arguments.path);
	}

	int counter = 0;
	double alpha_step = (double) MAX_ALPHA / arguments.fade;

	while(1){
		// Transition between wallpapers logic
		if (counter < arguments.fade) {
			// Copy source image
			SDL_RenderCopy(monitors[0].renderer, src, NULL, NULL);

			// Copy destination image with gradually increasing alpha
			SDL_SetTextureBlendMode(dst, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(dst, counter*alpha_step);
			SDL_RenderCopy(monitors[0].renderer, dst, NULL, NULL);

		// Destroy source texture and show destination texture
		} else if (counter == arguments.fade) {
			SDL_DestroyTexture(src);
			SDL_RenderCopy(monitors[0].renderer, dst, NULL, NULL);

		// Setup for the next wallpaper
		} else if (counter == arguments.ticks - 1) {
			randomFile(arguments.directory, arguments.file_buf, FILE_BUF_SIZE);
			src = dst;
			dst = IMG_LoadTexture(monitors[0].renderer, arguments.path);

		// Display the wallpaper normally
		} else {
			SDL_RenderCopy(monitors[0].renderer, dst, NULL, NULL);
		}

		SDL_RenderPresent(monitors[0].renderer);

		// Event handling
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			break;

		counter = (counter + 1) % arguments.ticks;
	}

	// Cleanup
	free(arguments.path);
	SDL_DestroyTexture(src);
	SDL_DestroyTexture(dst);
	SDL_Quit();
	IMG_Quit();
	XCloseDisplay(dpy);

	return 0;
}
