#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
using namespace std;

#define MAX_STRING_LENGTH 100
int sock_fd;

typedef union { sockaddr_in in4; sockaddr_in6 in6; } sockaddr_either;
// C-ish implementation. For pure modern C++, should use an std::bit_cast solution instead

string inet_ntos_either( const sockaddr_either *sa )
{
	char tmp[INET6_ADDRSTRLEN+1]="";
	if (sa->in4.sin_family == AF_INET)
		inet_ntop( AF_INET, &sa->in4.sin_addr, tmp, INET_ADDRSTRLEN );
	if (sa->in6.sin6_family == AF_INET6)
		inet_ntop( AF_INET6, &sa->in6.sin6_addr, tmp, INET6_ADDRSTRLEN );
	return tmp;
}

inline string inet_ntos_either( const sockaddr_either &sa ) { return inet_ntos_either(&sa); }

void* terminatorThread(void*)
{ 
	char a;
	cout << "Enter any key and press enter to shut down the server: " << endl;
	cin >> a;
	close(sock_fd);
	exit(0); //exit the whole server program
}

int main(int argc, char *argv[])
{
	if (argc==1)
	{
		cout << "Please pass the port number on which you want the server to listen." << endl;
		exit(-1);
	}

	int tid=0;
	if (pthread_create((pthread_t*) &tid, NULL, terminatorThread, NULL) == -1)
	{
		perror("pthread_create");
		exit(-1);
	}

	bool ipv6 = true;
	if (argc>=3)
		ipv6 = (strcmp(argv[2],"4") != 0);

	sockaddr_either server, client;
	memset(&server, 0, sizeof(server));
	memset(&client, 0, sizeof(client));

	if (ipv6)
	{
		if ((sock_fd = socket(AF_INET6, SOCK_DGRAM, 0)) == -1)
		{
			perror("socket IPv6");
			ipv6 = false;
		}
	}

	// Try to fallback to IPv4 only if IPv6 not supported / disabled
	if (!ipv6)
	{
		if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		{
			perror("socket IPv4");
			exit(-1);
		}
	}

	if (ipv6)
	{
		server.in6.sin6_family = AF_INET6;
		server.in6.sin6_port = htons(atoi(argv[1]));
		server.in6.sin6_addr = in6addr_any;
	}
	else
	{
		server.in4.sin_family = AF_INET;
		server.in4.sin_port = htons(atoi(argv[1]));
		server.in4.sin_addr.s_addr = INADDR_ANY;
	}

	if ((bind(sock_fd, (sockaddr*) &server, (socklen_t) sizeof(server))) == -1)
	{
		perror("bind");
		exit(-1);
	}

	char messageBuffer[MAX_STRING_LENGTH+1];

	while (true)
	{
		int len = sizeof(client);
		if (recvfrom(sock_fd, (void*) messageBuffer, (size_t) (MAX_STRING_LENGTH+1), MSG_WAITALL, (sockaddr*) &client, (socklen_t*) &len) == -1)
		{
			perror("recvfrom");
			continue;
		}

		cout << "Data Received from client (" << inet_ntos_either(client) << "): " << messageBuffer << endl << endl;
		 
		if (sendto(sock_fd, (void*) messageBuffer, (size_t) strlen(messageBuffer)+1, 0, (sockaddr*) &client, (socklen_t) len) == -1)
		{
			perror("sendto");
			continue;
		}

	}

}
