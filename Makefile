CC=gcc
CFLAGS=-I.
DEPS = mancala.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mancala: mancala.o main.o gametree.o
	$(CC) -o mancala mancala.o main.o gametree.o

.PHONY: clean

clean:
	rm -f *.o mancala