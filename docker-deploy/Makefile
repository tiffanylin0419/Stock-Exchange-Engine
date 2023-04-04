CC=g++
CFLAGS=-O3
EXTRAFLAGS=-lpqxx -lpq -lpugixml -lpthread

all: test

test: main.cpp query_funcs.h query_funcs.cpp tool.h tool.cpp head.h parse.hpp connect_func.cpp connect_func.h server.cpp server.h 
	$(CC) $(CFLAGS) -o test main.cpp query_funcs.cpp tool.cpp parse.cpp connect_func.cpp server.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o test

clobber:
	rm -f *~ *.o
