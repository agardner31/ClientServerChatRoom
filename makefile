all: chatclient chatserver

chatclient: chatclient.c
	gcc -o chatclient chatclient.c -lpthread
chatserver: chatserver.c
	gcc -o chatserver chatserver.c -lpthread
clean:
	$(RM) chatclient
	$(RM) chatserver