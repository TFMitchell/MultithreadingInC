#include <pthread.h>
#include "inputHandler.h"

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

void *publisher(void *args);

void *subscriber(void *args);

void *cleaner(void *args);
