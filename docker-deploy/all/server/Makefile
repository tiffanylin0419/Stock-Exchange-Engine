all: main

main: main.cpp connect_func.cpp connect_func.h server.cpp server.h head.h query_funcs.cpp parse.cpp tool.cpp query_funcs.h parse.hpp tool.h
	g++ -g  -o main main.cpp connect_func.cpp server.cpp query_funcs.cpp parse.cpp tool.cpp -lpthread

.PHONY:
	clean
clean:
	rm -rf *.o main