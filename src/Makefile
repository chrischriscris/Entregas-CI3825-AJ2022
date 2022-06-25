CC = gcc
CFLAGS = -ansi -Wall -pedantic -g

all: myutil

test: repla/testrepla wc/testwc walk_dir_tree/testwdt codif/testcodif

# =========== ejecutable =============
myutil: main.o repla.o list.o pair.o wc.o walk_dir_tree.o codif.o
	$(CC) $(CFLAGS) -o myutil main.o repla.o list.o pair.o wc.o walk_dir_tree.o codif.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

# =========== repla =============

################################################
repla/testrepla: repla/testrepla.o repla.o list.o pair.o
	$(CC) $(CFLAGS) -o repla/testrepla repla/testrepla.o repla.o list.o pair.o

repla/testrepla.o: repla/testrepla.c
	$(CC) $(CFLAGS) -o repla/testrepla.o -c repla/testrepla.c
################################################

repla.o: repla.c
	$(CC) $(CFLAGS) -c repla.c

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

pair.o: pair.c
	$(CC) $(CFLAGS) -c pair.c

# =========== wc =============

################################################
wc/testwc: wc/testwc.o wc.o
	$(CC) $(CFLAGS) -o wc/testwc wc/testwc.o wc.o

wc/testwc.o: wc/testwc.c
	$(CC) $(CFLAGS) -o wc/testwc.o -c wc/testwc.c
################################################

wc.o: wc.c
	$(CC) $(CFLAGS) -c wc.c

# =========== codif =============

codif.o: codif.c
	$(CC) $(CFLAGS) -c codif.c

################################################
codif/testcodif: codif/testcodif.o codif.o
	$(CC) $(CFLAGS) -o codif/testcodif codif/testcodif.o codif.o

codif/testcodif.o: codif/testcodif.c
	$(CC) $(CFLAGS) -o codif/testcodif.o -c codif/testcodif.c
################################################

# =========== walk_dir_tree =============

################################################
walk_dir_tree/testwdt: walk_dir_tree/testwdt.o walk_dir_tree.o
	$(CC) $(CFLAGS) -o walk_dir_tree/testwdt walk_dir_tree/testwdt.o walk_dir_tree.o

walk_dir_tree/testwdt.o: walk_dir_tree/testwdt.c
	$(CC) $(CFLAGS) -o walk_dir_tree/testwdt.o -c walk_dir_tree/testwdt.c
################################################

walk_dir_tree.o: walk_dir_tree.c
	$(CC) $(CFLAGS) -c walk_dir_tree.c

# =========== misc =============
clean:
	rm *.o

# =========== tests =============
cleantest:
	rm wc/testwc wc/testwc.o repla/testrepla repla/testrepla.o walk_dir_tree/testwdt walk_dir_tree/testwdt.o