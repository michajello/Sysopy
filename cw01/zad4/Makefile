

INC=include/
CFLAG= -ggdb -I
LIB=lib/


.PHONY: clean

staticList:
	gcc -Wall $(ARG) -o  testList.out src/testList.c $(LIB)libstlist.a $(CFLAG)$(INC)
	./testList.out

sharedList:
	gcc -Wall  $(ARG) -o  testList.out src/testList.c $(LIB)libstlist.so $(CFLAG)$(INC)
	./testList.out

clean:
	rm -f *.out
