
all: compile

compile:
	gcc -lSDL2 -lSDL2_image -lX11 main.c -o waal

test:
	gcc -lSDL2 -lSDL2_image -lX11 test.c -o test

install: waal
	cp waal $(HOME)/Scripts/waal
