
CINCLUDE=-I ../include


all: hash_test.o hash.o
	cc $(CINCLUDE) -o hash_test hash_test.o hash.o

hash_test.o:  ../src/hash.c ../include/hash.h
	cc $(CINCLUDE) -c hash_test.c

hash.o: ../src/hash.c ../include/hash.h
	cc $(CINCLUDE) -c ../src/hash.c

clean:
	-rm *.o hash_test
