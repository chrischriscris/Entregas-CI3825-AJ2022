CC = gcc
CFLAGS = -ansi -Wall -pedantic -g

all: repla/testrepla wc/testwc

test: repla/testrepla wc/testwc

# =========== repla =============

# -.-.-.-.--.-.-.-.--.-.-.-.--.-.-.-.--.-.-.-.-
repla/testrepla: repla/testrepla.o repla.o list.o pair.o
	$(CC) $(CFLAGS) -o repla/testrepla repla/testrepla.o repla.o list.o pair.o

repla/testrepla.o: repla/testrepla.c
	$(CC) $(CFLAGS) -o repla/testrepla.o -c repla/testrepla.c
# -.-.-.-.--.-.-.-.--.-.-.-.--.-.-.-.--.-.-.-.-

repla.o: repla.c
	$(CC) $(CFLAGS) -c repla.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

pair.o: pair.c
	$(CC) $(CFLAGS) -c pair.c

# =========== wc =============

# -.-.-.-.--.-.-.-.--.-.-.-.--.-.-.-.--.-.-.-.-
wc/testwc: wc/testwc.o wc.o
	$(CC) $(CFLAGS) -o wc/testwc wc/testwc.o wc.o

wc/testwc.o: wc/testwc.c
	$(CC) $(CFLAGS) -o wc/testwc.o -c wc/testwc.c
# -.-.-.-.--.-.-.-.--.-.-.-.--.-.-.-.--.-.-.-.-

wc.o: wc.c
	$(CC) $(CFLAGS) -c wc.c

# =========== misc =============

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm *.o

# =========== tests =============
cleantest:
	rm wc/testwc wc/testwc.o repla/testrepla repla/testrepla.o