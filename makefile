CC=gcc
CFLAGS=-I.
# DEPS = hellomake.h
LIBRARIES=-lxcb
OBJ = waal.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBRARIES)

waal: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBRARIES)

clean:
	rm *.o
