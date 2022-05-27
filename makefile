FLAG = -Wall -g
FPC = -fPIC 

all: queue.o client.o server.o client.o server client guard singleton

server.o: server.c
	gcc $(FLAG) -c server.c

queue.o: queue.c
	gcc $(FLAG) -c queue.c

client.o: client.c
	gcc $(FLAG) -c client.c

client: client.o
	gcc $(FLAG) -o client client.o

server: server.o queue.o 
	gcc $(FLAG) -o server server.o queue.o -lpthread 

guard: Guardimp.cpp
	g++ -o guard Guardimp.cpp

singleton: singleton.cpp
	g++ -o singleton singleton.cpp

.PHONY: clean all

clean:
	rm -f *.o server client guard singleton
