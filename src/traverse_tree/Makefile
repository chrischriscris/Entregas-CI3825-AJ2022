CC = gcc
CFLAGS = -ansi -Wall -pedantic

all: ejecutable

ejecutable: main.o
	$(CC) $(CFLAGS) -o myutil main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm *.o myutil