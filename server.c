#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>

#define QUEUE_SIZE  20
#define PORT "3490"
#define MAX_MSG_SIZE 2048

int main (int argc, char *argv[])
{
	int sockfd, clientfd[QUEUE_SIZE], send_size, send_pid, dont_send, current_client;
	int counter = 0;
	int int_buf = 0;
	struct addrinfo hints, *res;
	struct sockaddr_storage client_addr;
	socklen_t addr_size;
	char buf[MAX_MSG_SIZE] = "";
	char welcome_message[MAX_MSG_SIZE] = "Welcome to the server!";
	int msg_pipefd[2], counter_pipefd[2];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PORT, &hints, &res);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket failed");
		exit(1);
	}

	bind(sockfd, res->ai_addr, res->ai_addrlen);
	listen(sockfd, QUEUE_SIZE);

	addr_size = sizeof client_addr;
	
	pipe(msg_pipefd);
	pipe(counter_pipefd);
	
	while (1)
	{
		// Parent process accepts connect requests from listen queue

		clientfd[counter] = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
		send_size = send(clientfd[counter], welcome_message, sizeof(welcome_message), 0);
		if (send_size == -1)
				perror("send");
		
		kill(send_pid, 9);

		// Child process created for each client that connects
		
		if (fork() == 0)
		{	
			// Closing unused file descriptors

			close(sockfd);
			close(msg_pipefd[0]);
			close(counter_pipefd[0]);
			
			// Each process receives messages from its client and pipes the message
			// to another process which sends these messages to all other clients
			
			while (1)
			{
				if ((recv(clientfd[counter], buf, MAX_MSG_SIZE, 0)) == 0) 
				{
					close(clientfd[counter]);
					exit(EXIT_SUCCESS);
				}

				printf("Message from %d\n", counter);

				write(msg_pipefd[1], buf, MAX_MSG_SIZE);
				write(counter_pipefd[1], &counter, MAX_MSG_SIZE);
			}
		}		

		// Process created to read from pipes and send messages to clients

		if ((send_pid = fork()) == 0)
		{
			close(sockfd);
			close(counter_pipefd[1]);
			close(msg_pipefd[1]);

			while (1)
			{
				// read(counter_pipefd[0], &int_buf, MAX_MSG_SIZE);
				// dont_send = int_buf;
				// printf("Don't send message to %d\n", dont_send);
				read(msg_pipefd[0], buf, MAX_MSG_SIZE);
						
				for (int i = 0; i < counter + 1; i++)
				{
					// if (i == dont_send)
						// continue;
					if ((send(clientfd[i], buf, sizeof(buf), 0)) == -1)
					{
						perror("send");
						exit(EXIT_FAILURE);
					}
				}
			}
		}
		
		counter++;
	}

	close(sockfd);
}
