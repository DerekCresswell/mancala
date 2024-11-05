CC=gcc
CFLAGS=-I.
DEPS = mancala.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mancala: mancala.o main.o gametree.o arena.o
	$(CC) -o mancala main.o mancala.o gametree.o arena.o

.PHONY: clean

clean:
	rm -f *.o mancala