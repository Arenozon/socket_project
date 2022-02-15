#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	int sockfd, client_fd;
	struct addrinfo hints, *res;
	struct sockaddr_storage client_addr;
	socklen_t addr_size;
	char buf[2048] = "";

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, "3490", &hints, &res);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket failed");
		exit(1);
	}

	bind(sockfd, res->ai_addr, res->ai_addrlen);
	listen(sockfd, 5);

	addr_size = sizeof client_addr;
	client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
	printf("accepted client\n");

	if ((send(client_fd, "Welcome to the server", sizeof("Welcome to the server"), 0)) == -1)
		perror("send");
	
	while (1)
	{
		if ((recv(client_fd, buf, 2048, 0)) == 0) 
		{
			close(sockfd);
			break;
		}
		printf("%s", buf);
	}

	close(sockfd);
}
