
TARGETS=client

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

client: main.cpp connect_func.cpp connect_func.h 
	g++ -g  -o main main.cpp connect_func.cpp -lpthread