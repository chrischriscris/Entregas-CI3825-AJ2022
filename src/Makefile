CC = gcc
CFLAGS = -ansi -Wall -pedantic

all: myutil

# =========== ejecutable =============
myutil: main.o repla.o list.o pair.o wc.o utils.o codif.o find.o
	$(CC) $(CFLAGS) -o myutil main.o repla.o list.o pair.o wc.o utils.o codif.o find.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

# =========== repla =============

repla.o: repla.c
	$(CC) $(CFLAGS) -c repla.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

pair.o: pair.c
	$(CC) $(CFLAGS) -c pair.c

# =========== wc =============

wc.o: wc.c
	$(CC) $(CFLAGS) -c wc.c

# =========== codif =============

codif.o: codif.c
	$(CC) $(CFLAGS) -c codif.c

# =========== wdt =============

################################################
wdt/testwdt: wdt/testwdt.o utils.o find.o wc.o pair.o list.o repla.o codif.o
	$(CC) $(CFLAGS) -o wdt/testwdt wdt/testwdt.o utils.o find.o wc.o pair.o list.o repla.o codif.o

wdt/testwdt.o: wdt/testwdt.c
	$(CC) $(CFLAGS) -o wdt/testwdt.o -c wdt/testwdt.c
################################################

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

# =========== find =============
find.o: find.c
	$(CC) $(CFLAGS) -c find.c

# =========== misc =============
clean:
	rm *.o

# =========== tests =============
cleantest:
	rm wc/testwdt wc/testwdt.o myutil