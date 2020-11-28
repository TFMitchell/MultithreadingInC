//contains structs for threads and inputHandler.c functions
#include <pthread.h>
#include "topicQueue.h"

#define NUMPROXIES 10
#define MAXQTOPICS 10
#define MAXPUBSnSUBS 20

struct pubThread{
    struct topicQueue *TQ;
    pthread_t id;
    char free;
    char *test;
    pthread_cond_t wait;
    int currentQueueID;
};

struct subThread{
    struct topicQueue *TQ;
    pthread_t id;
    char free;
    pthread_cond_t wait;
    int currentQueueID;
};

struct cleanThread{
    pthread_t id;
    struct topicQueue *TQ;
    pthread_cond_t wait;
};

void queryTopics(struct topicQueue *TQ);

void queryPublishers(struct pubThread *pubThreads);

void querySubscribers(struct subThread *subThreads);

void inputHandler(struct topicQueue *TQ, char **tokens, struct pubThread *pubThreads, struct subThread *subThreads);
