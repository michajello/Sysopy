CC=gcc
CFLAG=-o

all: stat_funtion nftw_function

stat_function:
	$(CC) $@.c $(CFLAG) $@.out
	./$@.out ../../cw02 50
nftw_function:
	$(CC) $@.c $(CFLAG) $@.out
	./$@.out ../../cw02 50

.PHONY:clean

clean:
	rm -f *.out
