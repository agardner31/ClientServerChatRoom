#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>	   //write
#include <sys/poll.h>

#include <pthread.h> //for threading , link with lpthread

void *connection_handler(void *);

static int MAX_MESSAGE = 1024 * sizeof(char);
//mutex locked linked list of all sockets connected at a time
struct Node
{
	struct Node *next;
	int socket;
	char *username;
};
struct Node *head = NULL;
struct Node *tail = NULL;
void broadcast(int, struct Node *, char *, int, char *);

int main(int argc, char *argv[])
{
	int socket_desc, new_socket, c, *new_sock;
	struct sockaddr_in server, client;
	char message[MAX_MESSAGE];

	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	//Bind
	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		puts("bind failed");
		return 1;
	}
	while (1)
	{
		//Listen
		listen(socket_desc, 1);

		//Accept an incoming connection
		puts("Group Chat Started:");
		c = sizeof(struct sockaddr_in);
		new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);

		//figure out here how often it is accepted/how that works
		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = new_socket;

		if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}

		//Now join the thread , so that we don't terminate before the thread
		pthread_join(sniffer_thread, NULL);

		//for each socket, check if there is a new message, if so broadcast it simple as that

		if (new_socket < 0)
		{
			perror("accept failed");
			return 1;
		}
	}
	return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
	int sock = *(int *)socket_desc;
	int read_size;
	char message[MAX_MESSAGE];
	char *tempMessage;
	char client_message[MAX_MESSAGE];
	char username[MAX_MESSAGE];
	char *password = "cs3251secret";
	struct Node *userNode;

	if (read_size = recv(sock, client_message, MAX_MESSAGE, 0) > 0)
	{
		strcpy(username, client_message);
	}
	else
	{
		free(socket_desc);
		free(username);
		perror("recv failed");
		return 0;
	}
	if (read_size = recv(sock, client_message, MAX_MESSAGE, 0) > 0)
	{
		if (strcmp(client_message, password) != 0)
		{
			tempMessage = "Incorrect password. Connection cancelled";
			strncpy(message, tempMessage, strlen(tempMessage));
			write(sock, message, MAX_MESSAGE);
			free(username);
			fflush(stdout);
			free(socket_desc);
			return 0;
		}
		else
		{
			//user is added to chat, and username added to linked list
			userNode = (struct Node *)malloc(sizeof(struct Node));
			if (head == NULL)
			{
				head = userNode;
				tail = userNode;
			}
			else if (head->next == NULL)
			{
				head->next = userNode;
				tail = userNode;
			}
			else
			{
				tail->next = userNode;
				tail = userNode;
			}
			userNode->socket = sock;
			userNode->username = username;

			memset(message, 0, MAX_MESSAGE);
			tempMessage = "~~Welcome to the group chat :) Send a message at any time. Type 'Exit' to leave.~~";
			strncpy(message, tempMessage, strlen(tempMessage));
			write(sock, message, MAX_MESSAGE);
			memset(message, 0, MAX_MESSAGE);
			strcpy(message, "has just joined the chat.");
			broadcast(sock, head, message, MAX_MESSAGE, username);
			memset(message, 0, MAX_MESSAGE);
			//user has joined chat goes in log and is broadcasted to everyone and added to data structs

			//int read_size;
			struct Node *currBoi = head;
			//char username[MAX_MESSAGE];
			//char client_message[MAX_MESSAGE];
			while (currBoi != NULL)
			{
				if (currBoi->socket == sock)
				{
					strcpy(username, currBoi->username);
				}
				currBoi = currBoi->next;
			}
			while (1)
			{
				if ((read_size = recv(sock, client_message, MAX_MESSAGE, 0)) > 0)
				{
					if (strcmp(client_message, "Exit") == 0)
					{
						memset(message, 0, MAX_MESSAGE);
						strcpy(message, " has just left the chat.");
						//broadcast that client left and add to log
						broadcast(sock, head, message, MAX_MESSAGE, username);
						memset(message, 0, MAX_MESSAGE);
						//remove them from linked list
						struct Node *curr = head;
						while (curr != NULL)
						{
							if (curr->next->socket == sock)
							{
								if (curr->next->next == NULL)
								{
									curr->next == NULL;
									tail = curr;
								}
								else
								{
									curr->next = curr->next->next;
								}
							}
							curr = curr->next;
						}
						fflush(stdout);
						//free(message);
						return 0;
					}
					write(sock, client_message, MAX_MESSAGE); //get rid in a sec
					broadcast(sock, head, client_message, MAX_MESSAGE, username);
				}
			}
		}
	}
	else
	{
		perror("recv failed");
		free(socket_desc);
		free(username);
		return 0;
	}
	return 0;
}

void broadcast(int socket_desc, struct Node *head, char *message, int size, char *username)
{
	struct Node *currBoi = head;
	char newMessage[MAX_MESSAGE];
	//do this concat without messing up strings
	strcpy(newMessage, username);
	strcat(newMessage, ": ");
	strcat(newMessage, message);
	puts(newMessage);
	while (currBoi != NULL)
	{
		if (currBoi->socket != socket_desc)
		{
			write(socket_desc, newMessage, MAX_MESSAGE);
		}
		currBoi = currBoi->next;
	}
	memset(newMessage, 0, MAX_MESSAGE);
}