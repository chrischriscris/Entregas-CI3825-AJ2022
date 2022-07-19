CC = gcc
CFLAGS = -ansi -Wall -pedantic

all: ordenaproc

ordenaproc: ordenaproc.c procordenador.o procmezclador.o
	$(CC) $(CFLAGS) -o ordenaproc ordenaproc.c procordenador.o procmezclador.o utils.o

procordenador.o: procordenador.c utils.o
	$(CC) $(CFLAGS) -c procordenador.c utils.o

procmezclador.o: procmezclador.c
	$(CC) $(CFLAGS) -c procmezclador.c utils.o

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm *.o ordenaproc ordenahilo