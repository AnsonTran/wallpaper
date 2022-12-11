#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_atom.h>
#include <xcb/xproto.h>
#include <Imlib2.h>

static xcb_connection_t *conn;
static xcb_screen_t *screen;
static xcb_drawable_t win;
static int width;
static int height;

xcb_pixmap_t createPixmap(
		int depth,
		xcb_drawable_t window){

	xcb_pixmap_t pixmap = xcb_generate_id(conn);
	xcb_create_pixmap(conn, depth, pixmap, window, width, height);
	return pixmap;
}

void copyPixmap(xcb_drawable_t src, xcb_drawable_t dst){
	xcb_gcontext_t gc = xcb_generate_id(conn);
	xcb_create_gc(conn, gc, win, 0, NULL);
	xcb_copy_area(conn, src, dst, gc, 0, 0, 0, 0, width, height);
}

void setBackground(xcb_pixmap_t pixmap){
	char *root = "_XROOTPMAP_ID";
	char *eroot = "ESETROOT_PMAP_ID";

	xcb_intern_atom_cookie_t rootCookie, eRootCookie;
	xcb_atom_t rootAtom, eRootAtom;

	// Atom request and response
	rootCookie = xcb_intern_atom(conn, 0, strlen(root), root);
	eRootCookie = xcb_intern_atom(conn, 0, strlen(eroot), eroot);

	rootAtom = xcb_intern_atom_reply(conn, rootCookie, NULL)->atom;
	eRootAtom = xcb_intern_atom_reply(conn, eRootCookie, NULL)->atom;

	// Change properties
	xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win,
			rootAtom, XCB_ATOM_PIXMAP, 32, 1, &pixmap);

	xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win,
			eRootAtom, XCB_ATOM_PIXMAP, 32, 1, &pixmap);

	xcb_change_window_attributes(conn, win, XCB_CW_BACK_PIXMAP, &pixmap);
	xcb_clear_area(conn, 0, win, 0, 0, 0, 0);
	xcb_flush(conn);
}

int main (int argc, char** argv) {
	xcb_pixmap_t			pixmap;
	xcb_gcontext_t		gc;
	Imlib_Image				image, buffer;

	// Global values
  conn = xcb_connect (NULL, NULL);
	screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
	width = screen->width_in_pixels;
	height = screen->height_in_pixels;
	win = screen->root;

	pixmap = createPixmap(screen->root_depth, screen->root);

	// Copy root window contents into pixmap
	copyPixmap(win, pixmap);

	// Set as background
	setBackground(pixmap);

	// Load image
	image = imlib_load_image("/usr/local/share/background");
	imlib_context_set_image(image);

  xcb_disconnect(conn);

  return 0;
}    
