CC = gcc
CFLAGS = -ansi -Wall -pedantic -g

all:
	echo "all: nothing"

test: testrepla testwc

# =========== repla =============

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

# =========== wc =============

testwc: testwc.o wc.o
	$(CC) $(CFLAGS) -o testwc testwc.o wc.o

testwc.o: testwc.c
	$(CC) $(CFLAGS) -c testwc.c

wc.o: wc.c
	$(CC) $(CFLAGS) -c wc.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm *.o testrepla