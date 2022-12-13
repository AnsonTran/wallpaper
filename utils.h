#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <X11/Xlib.h>
#include <dirent.h>

#define SDL_IMAGE_FLAGS		IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP | IMG_INIT_JXL | IMG_INIT_AVIF
#define SDL_FLAGS					SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
#define SDL_PIXELFORMAT		SDL_PIXELFORMAT_ARGB8888


#ifndef UTILS_H
#define UTILS_H

typedef struct {
	Window root;
	int width;
	int height;
	SDL_Texture *image;
	SDL_Window *window;
	SDL_Renderer *renderer;
} Monitor;

void captureScreen(Monitor *screen);
void setupMonitors(Monitor *monitors, int num_screens);
void randomFile(char *path, char *buf, int size);

#endif
