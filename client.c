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
	int sockfd;
	struct addrinfo hints, *res;
	char buf[2048];
	char msg[2048] = "";

	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(argv[1], argv[2], &hints, &res);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	connect(sockfd, res->ai_addr, res->ai_addrlen);

	if (fork() == 0)
	{
		while (1)
		{
			if((recv(sockfd, buf, 2048, 0)) == -1)
			{
				perror("recv");
				exit(1);
			}
			printf("%s\n", buf);
		}
	}
	else
	{
		while (msg != "/exit\n")
		{
			fgets(msg, 2048, stdin);
			if ((send(sockfd, msg, sizeof(msg), 0)) == -1)
			{
				perror("send");
				exit(1);
			}
		}
	}

	close(sockfd);
	exit(0);
}
