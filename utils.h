#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <X11/Xlib.h>
#include <dirent.h>

#ifndef UTILS_H
#define UTILS_H

#define SDL_IMAGE_FLAGS		IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP | IMG_INIT_JXL | IMG_INIT_AVIF
#define SDL_FLAGS					SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
#define SDL_PIXELFORMAT		SDL_PIXELFORMAT_ARGB8888
#define MAX_ALPHA					255


typedef struct {
	Window root;
	int width;
	int height;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *src;
	SDL_Texture *dst;
} Monitor;

SDL_Texture *capture_screen(Monitor *screen);
void setup_monitors(Monitor *monitors, int num_screens);
int random_file(char *path, char *buf, int size);

#endif
