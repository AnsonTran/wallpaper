#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "utils.h"
#include <unistd.h>

static Display *dpy;
static int screens;

typedef struct {
	Window root;
	int width;
	int height;
	int depth;
	SDL_Texture *image;
	SDL_Window *window;
	SDL_Renderer *renderer;
} Monitor;

void captureScreen(Monitor *screen){
	XImage *capture = XGetImage(dpy, screen->root, 0, 0, screen->width, screen->height, AllPlanes, ZPixmap);

	// Create an SDL surface of the screen
	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
		capture->data, screen->width, screen->height, 32,
		capture->bytes_per_line,
		capture->red_mask, capture->green_mask, capture->blue_mask, 0
	);

	// Convert to a texture
	screen->image = SDL_CreateTextureFromSurface(screen->renderer, surface);
	SDL_FreeSurface(surface);
}

int main(int argc, char **argv) {
	// Initialization
	dpy = XOpenDisplay(NULL); // X11
	if (!dpy){ fprintf(stderr, "Could not open XDisplay\n"); return 1; }
	screens = ScreenCount(dpy);
	SDL_Init(SDL_INIT_VIDEO); // SDL
	IMG_Init(SDL_IMAGE_FLAGS); // SDL_image

	Monitor *monitors = malloc(sizeof(Monitor) * screens);

	for (int i=0; i<screens; i++){
		// Information about each screen
		monitors[i] = (Monitor) {
			.root = RootWindow(dpy, i),
			.width = DisplayWidth(dpy, i),
			.height = DisplayHeight(dpy, i),
			.depth = DefaultDepth(dpy, i),
		};
		monitors[i].window = SDL_CreateWindowFrom((void*) monitors[i].root);
		monitors[i].renderer = SDL_CreateRenderer(monitors[i].window, -1, SDL_FLAGS);

		// Scrape the root window into a texture
		captureScreen(&monitors[i]);
	}
	
	// Load wallpaper into texture
	SDL_Texture *wallpaper = IMG_LoadTexture(monitors[0].renderer, "/usr/local/share/background");

	while(1){
		//SDL_SetTextureBlendMode(wallpaper, SDL_BLENDMODE_BLEND);
		//SDL_SetTextureAlphaMod(wallpaper,1);
		SDL_RenderCopy(monitors[0].renderer, monitors[0].image, NULL, NULL);
		//SDL_RenderCopy(monitors[0].renderer, wallpaper, NULL, NULL);
		SDL_RenderPresent(monitors[0].renderer);
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			break;
	}

	// Cleanup
	SDL_Quit();
	IMG_Quit();
	XCloseDisplay(dpy);
}
