all:
	gcc -pthread -g topicQueue.h topicQueue.c string_parser.h string_parser.c server.h server.c -o quacker
