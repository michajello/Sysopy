
INC=include/
CFLAG=-I
CC=gcc
LIB=lib/
ARG=-g
.PHONY: staticList
.PHONY: sharedList
.PHONY: staticTree
.PHONY: sharedTree

.PHONY: clean

staticList:
	$(CC) -Wall   $(ARG) -o testList.out src/testList.c $(LIB)*libstlist.a $(CFLAG)$(INC)
	./testList.out

sharedList:
	$(CC) -Wall  $(ARG) -o  testList.out src/testList.c $(LIB)*libstlist.so $(CFLAG)$(INC)
	./testList.out

staticTree:
	$(CC) -Wall  $(ARG) -o testTree.out src/testTree.c $(LIB)libsttree.a $(CFLAG)$(INC)
	./testTree.out

sharedTree:
	$(CC) -Wall   $(ARG) -o testTree.out src/testTree.c $(LIB)libsttree.so  $(CFLAG)$(INC)
	./testTree.out

dynamicList:
	$(CC) -Wall  $(ARG)  -o testListDynamic.out src/testListDynamic.c $(LIB)libstlist.so $(CFLAG)$(INC) -ldl
	./testListDynamic.out

dynamicTree:
	$(CC) -Wall  $(ARG)   -o testTreeDynamic.out src/testTreeDynamic.c $(LIB)libsttree.so $(CFLAG)$(INC) -ldl
	./testTreeDynamic.out

clean:
	rm -f *.out
