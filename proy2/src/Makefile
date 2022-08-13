CC = gcc
CFLAGS = -ansi -Wall -pedantic

all: ordenaproc ordenahilo ordena

# Proceso

ordenaproc: ordenaproc.c procworkers.o sequence.o utils.o
	$(CC) $(CFLAGS) -o ordenaproc ordenaproc.c procworkers.o sequence.o utils.o

procworkers.o: procworkers.c
	$(CC) $(CFLAGS) -c procworkers.c

procordenador.o: procordenador.c
	$(CC) $(CFLAGS) -c procordenador.c

procmezclador.o: procmezclador.c
	$(CC) $(CFLAGS) -c procmezclador.c

procescritor.o: procescritor.c
	$(CC) $(CFLAGS) -c procescritor.c

# Hilo

ordenahilo: ordenahilo.c sequence.o utils.o threadworkers.o sharedvars.o
	$(CC) $(CFLAGS) -pthread -o ordenahilo ordenahilo.c sequence.o utils.o threadworkers.o sharedvars.o

threadworkers.o: threadworkers.c
	$(CC) $(CFLAGS) -c threadworkers.c

sharedvars.o: sharedvars.c
	$(CC) $(CFLAGS) -c sharedvars.c

# General

ordena: ordena.c utils.o sequence.o
	$(CC) $(CFLAGS) -pthread -o ordena ordena.c sequence.o utils.o


utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

sequence.o: sequence.c
	$(CC) $(CFLAGS) -c sequence.c

clean:
	rm *.o ordenaproc ordenahilo