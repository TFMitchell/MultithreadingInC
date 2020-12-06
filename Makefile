all:
	gcc -pthread -lpthread -g topicQueue.c inputHandler.c server.c quacker.c htmlHandler.c -o server
