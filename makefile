FLAG = -Wall -g
FPC = -fPIC 

all: clientTest.o client.o clientr.o main1.o reactor.o client.o main1.c client clientTest guard singleton selectserver selectclient clientr server

main1.o: main1.c
	gcc $(FLAG) -c main1.c

client.o: client.c
	gcc $(FLAG) -c client.c

reactor.o: reactor.c
	gcc $(FLAG) -c reactor.c

clientr.o: clientr.c
	gcc $(FLAG) -c clientr.c

clientTest.o: clientTest.c
	gcc $(FLAG) -c clientTest.c

clientTest: clientTest.o
	gcc $(FLAG) -o clientTest clientTest.o

client: client.o
	gcc $(FLAG) -o client client.o

selectclient: clientr.o
	gcc $(FLAG) -o selectclient clientr.o

server: main1.o 
	gcc $(FLAG) -o server main1.o -lpthread 

guard: guard.cpp
	g++ -o guard guard.cpp -lpthread

singleton: singleton.cpp
	g++ -o singleton singleton.cpp -lpthread

selectserver: reactor.o
	gcc $(FLAG) -o selectserver reactor.o -lpthread



.PHONY: clean all

clean:
	rm -f *.o server client guard singleton selectclient selectserver clientr clientTest
