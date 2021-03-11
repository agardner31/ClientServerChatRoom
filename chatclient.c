#include <stdio.h>
#include <string.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

char *inputTranslation(char *);
static int MAX_MESSAGE = 1024 * sizeof(char);
void *chat_thread(void *);

int main(int argc, char *argv[])
{
		char *serverIP = "127.0.0.1";
		int serverPortNum = 8888;

		//parse inputs to get username, and password
		if (argc != 5 || strcmp(argv[1], "-username") != 0 || strcmp(argv[3], "-password") != 0)
		{
			puts("Not a valid argument form. Input should look like this:");
			puts("./chatclient -username BobTheBuilder -password cs3251NotSecret");
			return 1;
		}
		char *username, *password;
		username = argv[2];
		if (strlen(username) > MAX_MESSAGE)
		{
			puts("Username too large. Max username size of 1024 characters.");
			return 0;
		}
		password = argv[4];
		if (strlen(username) > MAX_MESSAGE)
		{
			puts("Password too large. Max password size of 1024 characters.");
			return 0;
		}
		char *testUsername = calloc(MAX_MESSAGE, 1);
		char *testPassword = calloc(MAX_MESSAGE, 1);
		strncpy(testUsername, username, strlen(username));
		strncpy(testPassword, password, strlen(password));

		int socket_desc;
		struct sockaddr_in server;
		char *message, server_reply[MAX_MESSAGE];

		//Create socket
		socket_desc = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_desc == -1)
		{
			printf("Could not create socket");
		}

		server.sin_addr.s_addr = inet_addr(serverIP);
		server.sin_family = AF_INET;
		server.sin_port = htons(serverPortNum);

		//Connect to remote server

		if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
		{
			puts("could not connect to server");
			return 0;
		}

		//Send username
		if (send(socket_desc, testUsername, MAX_MESSAGE, 0) < 0)
		{
			puts("connection error - try again");
			return 0;
		}
		free(testUsername);

		//send password
		if (send(socket_desc, testPassword, MAX_MESSAGE, 0) < 0)
		{
			puts("connection error - try again");
			return 0;
		}
		free(testPassword);

		//check validity of password
		if (recv(socket_desc, server_reply, MAX_MESSAGE, 0) < 0)
		{
			puts("connection error - try again");
			return 0;
		}
		char *wrongPassword = "Incorrect password. Connection cancelled";
		char server_reply_substring[40];
		strncpy(server_reply_substring, server_reply, 40);
		if (strcmp(wrongPassword, server_reply_substring) == 0)
		{
			puts(server_reply_substring);
			return 0;
		}
		puts(server_reply);

		//create char thread
		pthread_t sniffer_thread;
		int *new_sock;
		new_sock = malloc(1);
		*new_sock = socket_desc;

		if (pthread_create(&sniffer_thread, NULL, chat_thread, (void *)new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}
		//Now join the thread , so that we don't terminate before the thread
		pthread_join(sniffer_thread, NULL);
		//join char thread
		//Receive all group chat activity :)
}

char *inputTranslation(char *input)
{
	if (strcmp(input, ":)") == 0)
	{
		return "feeling happy";
	}
	else if (strcmp(input, ":(") == 0)
	{
		return "feeling sad";
	}
	else if (strcmp(input, ":mytime") == 0)
	{
		struct timeval time;
		gettimeofday(&time, NULL);
		char timeBuffer[MAX_MESSAGE];
		struct tm *hourlyTime = localtime(&(time.tv_sec));
		strftime(timeBuffer, MAX_MESSAGE, "%I:%M", hourlyTime);
		input = timeBuffer;
	}
	else if (strcmp(input, ":+1hr") == 0)
	{
		struct timeval time;
		gettimeofday(&time, NULL);
		char timeBuffer[MAX_MESSAGE];
		time.tv_sec += 3600;
		struct tm *hourlyTime = localtime(&(time.tv_sec));
		strftime(timeBuffer, MAX_MESSAGE, "%I:%M", hourlyTime);
		input = timeBuffer;
	}
	return input;
}

void *chat_thread(void *socket_desc)
{
	int sock = *(int *)socket_desc;
	char server_reply[MAX_MESSAGE];
	char input[MAX_MESSAGE];
	while (1)
	{
		struct timeval timeout;
		timeout.tv_sec = 1;
		//do this if socket_desc buffer has stuff received
		while (select(sock, NULL, NULL, NULL, &timeout) > 0)
		{
			if (recv(sock, server_reply, MAX_MESSAGE, 0) > 0)
			{
				puts(server_reply);
			}
		}
		//do this if stdin has something in it
		if (1)
		{
			fgets(input, MAX_MESSAGE, stdin);
			char newInput[MAX_MESSAGE];
			strncpy(newInput, input, strlen(input) - sizeof(char));
			if (strlen(input) > 1024)
			{
				puts("Message is too big. Character limit is 1024 :)");
			}
			else if (strcmp(newInput, "Exit") == 0)
			{
				send(sock, newInput, MAX_MESSAGE, 0);
				puts("~~You have left the chat.~~");
				return 0;
			}
			else
			{
				char *userInput = calloc(MAX_MESSAGE, 1);
				char *translatedInput = inputTranslation(newInput);
				strncpy(userInput, translatedInput, strlen(translatedInput));
				send(sock, userInput, MAX_MESSAGE, 0);
				memset(userInput, 0, MAX_MESSAGE);

			}
			memset(newInput, 0, MAX_MESSAGE);
			fflush(stdin);
		}
	}
	return 0;
}
