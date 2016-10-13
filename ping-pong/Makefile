CC = gcc
CFLAGS = -std=gnu99 -O3 #-DDEBUG
LIBS = -lrt

default: all

all: server client

server: server.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

client: client.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

server.o: server.c
	$(CC) $(CFLAGS) -c $<

client.o: client.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm server client *.o
