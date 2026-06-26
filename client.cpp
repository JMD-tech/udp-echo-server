#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
using namespace std;

#define MAX_STRING_LENGTH 100

typedef struct { int ai_family; string address; } host_address;

host_address lookup_host (const char *host, char force_ipversion = 0)
{
	host_address selected = { 0, "" };
	// Mostly from: https://gist.github.com/jirihnidek/bf7a2363e480491da72301b228b35d5d
	struct addrinfo hints, *res, *result;
	int errcode;
	char addrstr[MAX_STRING_LENGTH+1];
	void *ptr;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags |= AI_CANONNAME;

	if (force_ipversion == '4') hints.ai_family = AF_INET;
	if (force_ipversion == '6') hints.ai_family = AF_INET6;

	errcode = getaddrinfo (host, NULL, &hints, &result);
	if (errcode != 0)
	{
		perror("getaddrinfo: ");
		return selected;
	}

	res = result;

	while (res)
	{
		ptr = 0;
		switch (res->ai_family)
		{
			case AF_INET:
				ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
				break;
			case AF_INET6:
				ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
				break;
		}
		if (ptr)
		{
			inet_ntop (res->ai_family, ptr, addrstr, MAX_STRING_LENGTH);

			if (!selected.ai_family || ((res->ai_family == AF_INET6) && (selected.ai_family == AF_INET)))
			{
				selected.ai_family = res->ai_family;
				selected.address = addrstr;
			}
		}
		res = res->ai_next;
	}

	freeaddrinfo(result);

	return selected;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cout << "Please pass the ip adress and port number on which the server is listening." << endl;
		exit(-1);
	}

	char force_ipversion = 0;

	if (argc >= 4)
		force_ipversion = argv[3][0];

	host_address selected_addr = lookup_host(argv[1],force_ipversion);
	if (!selected_addr.ai_family)
	{
		cout << "Can't find valid address for " << argv[1] << endl;
		exit(-1);
	}

	union {
		sockaddr_in in4;
		sockaddr_in6 in6;
	} server;
	memset(&server, 0 ,sizeof(server));

	int sock_fd;

	if ((sock_fd = socket(selected_addr.ai_family, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket: ");
		exit(-1);
	}

	if (selected_addr.ai_family == AF_INET6)
	{
		server.in6.sin6_family = AF_INET6;
		server.in6.sin6_port = htons(atoi(argv[2]));
		if (inet_pton(AF_INET6, selected_addr.address.c_str(), &server.in6.sin6_addr) <= 0)
		{
			perror("inet_pton: ");
			exit(-1);
		}
	}
	else
	{
		server.in4.sin_family = AF_INET;
		server.in4.sin_port = htons(atoi(argv[2]));
		server.in4.sin_addr.s_addr = inet_addr(selected_addr.address.c_str());
	}

	char messageBuffer[MAX_STRING_LENGTH+1]="";
	char serverReply[MAX_STRING_LENGTH+1]="";
	int len=0;

	while (true)
	{ 
		cout << "Please enter the input to send to the server: " << endl;
		cin.getline(messageBuffer, MAX_STRING_LENGTH);

		if (strcmp(messageBuffer, "exit")==0)
		break;

		if (sendto(sock_fd, (void*) messageBuffer, (size_t) strlen(messageBuffer)+1, 0, (sockaddr *) &server, sizeof(server))==-1)
		{
			perror("sendto: ");
			continue;
		}

		if (recvfrom(sock_fd, (void*) serverReply, (size_t) (MAX_STRING_LENGTH+1), MSG_WAITALL, (sockaddr *) &server, (socklen_t*) &len)==-1)
		{
			perror("recvfrom: ");
			continue;
		}

		cout << "The server sent: " << serverReply << endl << endl;

	}

	close(sock_fd);
	exit(0);
}
