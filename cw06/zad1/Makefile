CC=gcc
IDIR=include
CFLAGS=-std=gnu11 -Wall -I$(IDIR) -o 

all: client server

client:
	$(CC) $@.c $(CFLAGS) $@.out

server:
	$(CC) $@.c $(CFLAGS) $@.out

.PHONY:clean
clean:
	rm -f *.out
