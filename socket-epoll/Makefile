CC=gcc
FLAGS=-std=c99 -O2
LIBS=-lpthread -levent

all: server client

server: server.o
	${CC} ${FLAGS} $< -o $@ ${LIBS}

server.o: server.c
	${CC} ${FLAGS} -c $<

client: client.o
	${CC} ${FLAGS} $< -o$@ ${LIBS}

client.o: client.c
	${CC} ${FLAGS} -c $<

clean:
	rm *.o server client
