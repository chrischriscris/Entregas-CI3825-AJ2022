CC = gcc
CFLAGS = -ansi -Wall -pedantic -ggdb3

all: ordenaproc

ordenaproc: ordenaproc.c procordenador.o procmezclador.o procescritor.o
	$(CC) $(CFLAGS) -o ordenaproc ordenaproc.c procordenador.o procmezclador.o utils.o procescritor.o

procordenador.o: procordenador.c
	$(CC) $(CFLAGS) -c procordenador.c

procmezclador.o: procmezclador.c
	$(CC) $(CFLAGS) -c procmezclador.c

procescritor.o: procescritor.c
	$(CC) $(CFLAGS) -c procescritor.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm *.o ordenaproc ordenahilo