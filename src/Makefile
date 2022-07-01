CC = gcc
CFLAGS = -ansi -Wall -pedantic

all: myutil

myutil: repla.o list.o pair.o wc.o utils.o codif.o find.o main.o roll.o
	$(CC) $(CFLAGS) -o myutil main.o repla.o list.o pair.o wc.o utils.o codif.o find.o roll.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

repla.o: repla.c
	$(CC) $(CFLAGS) -c repla.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

pair.o: pair.c
	$(CC) $(CFLAGS) -c pair.c

wc.o: wc.c
	$(CC) $(CFLAGS) -c wc.c

codif.o: codif.c
	$(CC) $(CFLAGS) -c codif.c

find.o: find.c
	$(CC) $(CFLAGS) -c find.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

roll.o: roll.c
	$(CC) $(CFLAGS) -c roll.c

clean:
	rm *.o myutil