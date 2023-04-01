CC=g++
CFLAGS=-O3 
EXTRAFLAGS=-lpqxx -lpq -lpugixml

all: test

test: parse_test.cpp parse.hpp parse.cpp
	$(CC) $(CFLAGS) -o test parse_test.cpp parse.hpp parse.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o test

clobber:
	rm -f *~ *.o
