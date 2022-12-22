all: compile

compile:
	gcc -lSDL2 -lSDL2_image -lX11 main.c parser.c -o fiwall

install: fiwall
	cp fiwall $(HOME)/Scripts/fiwall
