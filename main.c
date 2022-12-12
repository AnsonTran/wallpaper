#include "utils.h"

static Display *dpy;
static int screens;

const char *argp_program_version = "1.0";
const char *argp_program_bug_address = "<trananson@protonmail.com>";
static char doc[] = "Waal - X11 wallpaper using SDL";

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
	static struct argp argp = { 0, 0, 0, doc };
	argp_parse(&argp, argc, argv, 0, 0, 0);

	int long_delay = 1000;
	int transition_delay = 50;
	int counter = 0;
	double alpha_step = 255.0 / transition_delay;

	char *path = "/usr/local/share/wallpaper";
	int len = strlen(path);

	int buf_size = 100;
	char *fullpath = malloc((len + buf_size + 1)*sizeof(char));
	strncpy(fullpath, path, len);
	fullpath[len] = '/';

	char *file = fullpath + len + 1;

	// Initialization
	dpy = XOpenDisplay(NULL); // X11
	if (!dpy){ fprintf(stderr, "Could not open XDisplay\n"); return 1; }
	screens = ScreenCount(dpy);
	SDL_Init(SDL_INIT_VIDEO); // SDL
	IMG_Init(SDL_IMAGE_FLAGS); // SDL_image

	// Get information about screen setup
	Monitor *monitors = malloc(sizeof(Monitor) * screens);
	setupMonitors(monitors, screens);

	// Load initial image into renderer
	SDL_Texture *src = monitors[0].image;
	SDL_Texture *dst = IMG_LoadTexture(monitors[0].renderer, "/usr/local/share/background");

	while(1){
		// Transition between wallpapers logic
		if (counter < transition_delay) {
			// Copy source image
			SDL_RenderCopy(monitors[0].renderer, src, NULL, NULL);

			// Copy destination image with gradually increasing alpha
			SDL_SetTextureBlendMode(dst, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(dst, counter*alpha_step);
			SDL_RenderCopy(monitors[0].renderer, dst, NULL, NULL);
		// Otherwise just show the image
		} else {
			SDL_RenderCopy(monitors[0].renderer, dst, NULL, NULL);
		}
		SDL_RenderPresent(monitors[0].renderer);

		// Event handling
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			break;

		// Setup to change wallpaper
		if (counter == 499) {
			SDL_DestroyTexture(src);
			randomFile(path, file, buf_size);
			src = dst;
			dst = IMG_LoadTexture(monitors[0].renderer, fullpath);
		}

		counter = (counter + 1) % long_delay;
	}

	// Cleanup
	SDL_DestroyTexture(src);
	SDL_DestroyTexture(dst);
	SDL_Quit();
	IMG_Quit();
	XCloseDisplay(dpy);

	return 0;
}
