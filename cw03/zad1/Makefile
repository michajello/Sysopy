CC=gcc
CFLAG=-std=c99 -Wall -o

all: main getenv

main:
	$(CC) $@.c $(CFLAG)	$@.out
	./$@.out testFile

getenv:
	$(CC) $@.c $(CFLAG)	$@.out
	./$@.out PWD

.PHONY:clean

clean:
	rm -f *.out
