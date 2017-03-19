CC=gcc
OUT=program.out
CFLAG=-o
all:
	$(CC) main.c  $(CFLAG) $(OUT)

sys_sort_test:all
	./$(OUT) generate testFile 12 20
	cat testFile
	./$(OUT) sys sort testFile 12 20
	cat testFile
lib_sort_test:all
	./$(OUT) generate testFile 10 5
	cat testFile
	./$(OUT) lib sort testFile 10 5
	cat testFile
sys_shuffle_file:all
	./$(OUT) generate testFile 10 10
	./$(OUT) sys sort testFile 10 10
	cat testFile
	./$(OUT) sys shuffle testFile 10 10
	cat testFile
lib_shuffle_file:all
	./$(OUT) generate testFile 10 10
	./$(OUT) lib sort testFile 10 10
	cat testFile
	./$(OUT) lib shuffle testFile 10 10
	cat testFile
