C=gcc
CFLAG=-std=c11 -Wall -o

all: test main

test:
	$(CC) $@.c $(CFLAG)	$@.out


main:
	$(CC) $@.c $(CFLAG)	$@.out
	./$@.out testFile 1 100


.PHONY:clean

clean:
	rm -f *.out
