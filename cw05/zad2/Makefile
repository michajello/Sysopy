CC=gcc
CFLAG=-std=gnu11 -Wall -o

all: master slave

master:
	$(CC) $@.c $(CFLAG)	$@.out

slave:
	$(CC) $@.c $(CFLAG)	$@.out



.PHONY:clean

clean:
	rm -f *.out
	rm -f data
