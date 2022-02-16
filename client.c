#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>

#define MAX_MSG_SIZE 2048 

int main (int argc, char *argv[])
{
	int sockfd;
	struct addrinfo hints, *res;
	char buf[MAX_MSG_SIZE];
	char msg[MAX_MSG_SIZE] = "";

	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(argv[1], argv[2], &hints, &res);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	connect(sockfd, res->ai_addr, res->ai_addrlen);

	system("clear");

	if (fork() == 0)
	{
		while (1)
		{
			if((recv(sockfd, buf, MAX_MSG_SIZE, 0)) == -1)
			{
				perror("recv");
				close(sockfd);
				exit(EXIT_FAILURE);
			}
			printf("%s\n", buf);
		}
	}
	else
	{
		while (msg != "/exit\n")
		{
			fgets(msg, MAX_MSG_SIZE, stdin);
			fputs("\033[A\033[2k",stdout);
			if ((send(sockfd, msg, sizeof(msg), 0)) == -1)
			{
				perror("send");
				close(sockfd);
				exit(EXIT_FAILURE);
			}
		}
	}

	close(sockfd);
	exit(EXIT_SUCCESS);
}
