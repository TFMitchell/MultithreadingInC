all:
	gcc -pthread -g topicQueue.h topicQueue.c inputHandler.c inputHandler.h server.h server.c quacker.c -o server
