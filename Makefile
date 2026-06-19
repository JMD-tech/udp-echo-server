
default: all

all: client server

clean:
	rm -f client server client.o server.o

client: client.o
	g++ client.cpp -o client

server: server.o
	g++ server.cpp -lpthread -o server

