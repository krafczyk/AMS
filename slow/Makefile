
CFLAGS=-I/usr/local/include -Wall -g

#CLFLAGS = -g1
LDFLAGS=-L/usr/local/lib

LOADLIBES=-lm -lamstcp

CC=gcc

OBJS =	Coo.c

recalc: Coordinates.o
	$(CC) $(CFLAGS) -g -o Coo_file Coo_file.c Coordinates.o $(LDFLAGS) $(LOADLIBES)

coo_recalc.o: Coordinates.h
	$(CC)  $(CFLAGS) -g -c Coordinates.c  $(LDFLAGS) $(LOADLIBES)





