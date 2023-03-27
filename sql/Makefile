CC=g++
CFLAGS=-O3
EXTRAFLAGS=-lpqxx -lpq

all: test

test: main.cpp query_funcs.h query_funcs.cpp tool.h tool.cpp
	$(CC) $(CFLAGS) -o test main.cpp query_funcs.cpp tool.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o test

clobber:
	rm -f *~ *.o
