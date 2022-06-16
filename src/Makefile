CC = gcc
CFLAGS = -ansi -Wall -pedantic

all: sustituir

sustituir:  pair.o list.o sustituir.o utils.o
	$(CC) $(CFLAGS) -o sustituir sustituir.o pair.o list.o utils.o

sustituir.o: sustituir.c
	$(CC) $(CFLAGS) -c sustituir.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

pair.o: pair.c
	$(CC) $(CFLAGS) -c pair.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm *.o sustituir