//Many of the constants are set in this header
#include <pthread.h>
#include "topicQueue.h"

#define NUMPROXIES 10
#define MAXQTOPICS 10
#define MAXPUBSnSUBS 20
#define MAXLINE 256

void queryTopics(struct topicQueue *TQ);

void queryPublishers(char **publisherFiles);

void querySubscribers(char **subscriberFiles);

int inputHandler(struct topicQueue *TQ, char **tokens, char **publisherFiles, char **subscriberFiles);
