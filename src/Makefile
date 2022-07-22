CC = gcc
CFLAGS = -ansi -Wall -pedantic

all: ordenaproc

ordenaproc: ordenaproc.c procordenador.o procmezclador.o procescritor.o Sequence.o utils.o
	$(CC) $(CFLAGS) -o ordenaproc ordenaproc.c procordenador.o procmezclador.o procescritor.o Sequence.o utils.o

procordenador.o: procordenador.c
	$(CC) $(CFLAGS) -c procordenador.c

procmezclador.o: procmezclador.c
	$(CC) $(CFLAGS) -c procmezclador.c

procescritor.o: procescritor.c
	$(CC) $(CFLAGS) -c procescritor.c

utils.o: utils.c Sequence.o
	$(CC) $(CFLAGS) -c utils.c

Sequence.o: Sequence.c
	$(CC) $(CFLAGS) -c Sequence.c

clean:
	rm *.o ordenaproc ordenahilo