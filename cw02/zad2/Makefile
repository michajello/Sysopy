CC=gcc
CFLAG=-o

all: stat_funtion nftw_function

stat_function:
	$(CC) $@.c $(CFLAG) $@.out
	./$@.out ../../cw02 3000
nftw_function:
	$(CC) $@.c $(CFLAG) $@.out
	./$@.out ../../cw02 3000

.PHONY:clean

clean:
	rm -f *.out
