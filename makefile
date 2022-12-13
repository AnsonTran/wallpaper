all: compile

compile:
	gcc -lSDL2 -lSDL2_image -lX11 main.c parser.c -o waal

install: waal
	cp waal $(HOME)/Scripts/waal
