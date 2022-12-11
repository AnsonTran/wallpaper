#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>

static Display *dpy;
static int screens;

typedef struct {
	Window root;
	int width;
	int height;
	int depth;
	GC context;
	Pixmap pixmap;
} Monitor;

// Capture root window of each screen and store into pixmap
void captureScreen(Monitor *screen){
	XCopyArea( dpy, screen->root, screen->pixmap, screen->context,
		0, 0, // src x,y
		screen->width, screen->height, // width, height
		0, 0 // dst x,y
	);
	XFlush(dpy);
}


int main(int argc, char **argv) {
	dpy = XOpenDisplay(NULL);
	if (!dpy){
		fprintf(stderr, "Could not open XDisplay\n");
		return 1;
	}
	screens = ScreenCount(dpy);

	Monitor *monitors = malloc(sizeof(Monitor) * screens);

	for (int i=0; i<screens; i++){
		// Information about each screen
		monitors[i].root = RootWindow(dpy, i);
		monitors[i].width = DisplayWidth(dpy, i);
		monitors[i].height = DisplayHeight(dpy, i);
		monitors[i].depth = DefaultDepth(dpy, i);
		monitors[i].context = DefaultGC(dpy, i);

		// Create a pixmap for each
		monitors[i].pixmap = XCreatePixmap(dpy,
			monitors[i].root,
			monitors[i].width,
			monitors[i].height,
			monitors[i].depth
		);

		// Scrape the root window and copy into pixmap
		captureScreen(&monitors[i]);


	}
}
