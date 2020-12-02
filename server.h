#include <pthread.h>
#include "inputHandler.h"

#define MAXLINE 256

struct pubThread{
    struct topicQueue *TQ;
    pthread_t id;
    char free;
    char *fileName;
    pthread_cond_t wait;
};

struct subThread{
    struct topicQueue *TQ;
    pthread_t id;
    char free;
    char *fileName;
    pthread_cond_t wait;
};

struct cleanThread{
    pthread_t id;
    struct topicQueue *TQ;
    pthread_cond_t wait;
};

char *getDone();

void *publisher(void *args);

void *subscriber(void *args);

void *cleaner(void *args);
