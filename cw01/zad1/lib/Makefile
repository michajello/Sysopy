CC = gcc
IDIR=../include
CFLAGS=-I$(IDIR)

all: libstlist.a libstlist.so libsttree.so libsttree.a

libstlist.so: list2.o
	gcc -shared -o libstlist.so list2.o

libstlist.a: list.o
	ar rc libstlist.a list.o
	ranlib libstlist.a

list.o: list.c
	gcc -c -o list.o list.c $(CFLAGS)

list2.o: list.c
	gcc -fPIC -c -o list2.o list.c $(CFLAGS)

libsttree.so: tree2.o
	gcc -shared -o libsttree.so tree2.o

libsttree.a: tree.o
	ar rc libsttree.a tree.o
	ranlib libsttree.a

tree.o: tree.c
	gcc -c -o tree.o tree.c $(CFLAGS)

tree2.o: tree.c
	gcc -fPIC -c -o tree2.o tree.c $(CFLAGS)



.PHONY: clean

clean:
	rm -f *.o
