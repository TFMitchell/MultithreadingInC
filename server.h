#include <pthread.h>
#include "topicQueue.h"

#define NUMPROXIES 50


struct pubThread{
    struct topicQueue *TQ;
    pthread_t id;
    char free;
    char *test;
    pthread_cond_t wait;
};

struct subThread{
    struct topicQueue *TQ;
    pthread_t id;
    char free;
    pthread_cond_t wait;
};

struct cleanARGS{
    struct topicQueue *TQ;
    char *test;
};
