CC = gcc
CFLAGS = -ansi -Wall -pedantic

all:
	echo "all: nothing"

tests: testrepla

testrepla: testrepla.o repla.o list.o pair.o utils.o
	$(CC) $(CFLAGS) -o testrepla testrepla.o repla.o list.o pair.o utils.o

testrepla.o: testrepla.c
	$(CC) $(CFLAGS) -c testrepla.c

repla.o: repla.c
	$(CC) $(CFLAGS) -c repla.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

pair.o: pair.c
	$(CC) $(CFLAGS) -c pair.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm *.o testrepla