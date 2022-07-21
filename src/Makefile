CC = gcc
CFLAGS = -ansi -Wall -pedantic -ggdb3

all: ordenaproc

ordenaproc: ordenaproc.c procordenador.o procmezclador.o procescritor.o Sequence.o utils.o
	$(CC) $(CFLAGS) -o ordenaproc ordenaproc.c procordenador.o procmezclador.o utils.o procescritor.o Sequence.o

procordenador.o: procordenador.c utils.o Sequence.o
	$(CC) $(CFLAGS) -c procordenador.c utils.o Sequence.o

procmezclador.o: procmezclador.c Sequence.o
	$(CC) $(CFLAGS) -c procmezclador.c Sequence.o

procescritor.o: procescritor.c
	$(CC) $(CFLAGS) -c procescritor.c

utils.o: utils.c Sequence.o
	$(CC) $(CFLAGS) -c utils.c Sequence.o

Sequence.o: Sequence.c
	$(CC) $(CFLAGS) -c Sequence.c

clean:
	rm *.o ordenaproc ordenahilo