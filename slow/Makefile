#  $Id: Makefile,v 1.4 2001/01/22 17:32:54 choutko Exp $

CFLAGS=-I/usr/local/include  -g

#CLFLAGS = -g1
LDFLAGS=-L/usr/local/lib

LOADLIBES=-lm -lamstcp

CC=cxx

OBJS =	Coo.c

recalc: Coordinates.o
	$(CC) $(CFLAGS) -g -o Coo_file Coo_file.c Coordinates.o $(LDFLAGS) $(LOADLIBES)

coo_recalc.o: Coordinates.h
	$(CC)  $(CFLAGS) -g -c Coordinates.c  $(LDFLAGS) $(LOADLIBES)





