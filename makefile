INSTALL_DIR = $(HOME)/bin

all: compile

compile:
	gcc -lSDL2 -lSDL2_image -lX11 main.c parser.c -o wallpaper

install:
	cp wallpaper $(INSTALL_DIR)/wallpaper

clean:
	rm $(INSTALL_DIR)/wallpaper
