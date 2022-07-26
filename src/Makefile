CC = gcc
CFLAGS = -ansi -Wall -pedantic -g

all: ordenaproc ordenahilo ordena

# Proceso

ordenaproc: ordenaproc.c procordenador.o procmezclador.o procescritor.o sequence.o utils.o
	$(CC) $(CFLAGS) -o ordenaproc ordenaproc.c procordenador.o procmezclador.o procescritor.o sequence.o utils.o

procordenador.o: procordenador.c
	$(CC) $(CFLAGS) -c procordenador.c

procmezclador.o: procmezclador.c
	$(CC) $(CFLAGS) -c procmezclador.c

procescritor.o: procescritor.c
	$(CC) $(CFLAGS) -c procescritor.c

# Hilo

ordenahilo: ordenahilo.c utils.o
	$(CC) $(CFLAGS) -pthread -o ordenahilo ordenahilo.c sequence.o utils.o

# hiloordenador.o: hiloordenador.c
# 	$(CC) $(CFLAGS) -c hiloordenador.c

# hilomezclador.o: hilomezclador.c
# 	$(CC) $(CFLAGS) -c hilomezclador.c

# hiloescritor.o: hiloescritor.c
# 	$(CC) $(CFLAGS) -c hiloescritor.c

# General

ordena: ordena.c
	$(CC) $(CFLAGS) -pthread -o ordena ordena.c sequence.o utils.o

utils.o: utils.c sequence.o
	$(CC) $(CFLAGS) -c utils.c

sequence.o: sequence.c
	$(CC) $(CFLAGS) -c sequence.c

clean:
	rm *.o ordenaproc ordenahilo