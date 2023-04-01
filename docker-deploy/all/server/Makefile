all: main

main: main.cpp connect_func.cpp connect_func.h server.cpp server.h head.h
	g++ -g  -o main main.cpp connect_func.cpp server.cpp -lpthread

.PHONY:
	clean
clean:
	rm -rf *.o main