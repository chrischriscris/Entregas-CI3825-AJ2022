CC = gcc
CFLAGS = -ansi -Wall -pedantic

all: sustituir

sustituir: main.o pair.o list.o utils.o
	$(CC) $(CFLAGS) -o sustituir main.o pair.o list.o utils.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

pair.o: pair.c
	$(CC) $(CFLAGS) -c pair.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm *.o sustituir