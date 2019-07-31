CC=gcc
CFLAGS=-Wall -g -DNDEBUG

all=ex22_main

ex22_main: ex22.o ex22_main.o
	$(CC) $(CFLAGS) ex22_main.o ex22.o -o ex22_main

ex22.o: ex22.c
	$(CC) $(CFLAGS) -c ex22.c -o ex22.o

ex22_main.o: ex22_main.c ex22.h
	$(CC) $(CFLAGS) -c ex22_main.c -o ex22_main.o

run:
	./ex22_main

clean:
	rm -f *.o ex22_main
