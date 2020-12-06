#include <pthread.h>
#include "inputHandler.h"

//contains the globals
extern char done;
extern pthread_cond_t cond;
extern pthread_mutex_t mtx;

struct pubThread{
    struct topicQueue *TQ;
    pthread_t id;
    char free;
    char *fileName;
};

struct subThread{
    struct topicQueue *TQ;
    pthread_t id;
    char free;
    char *fileName;
};

struct cleanThread{
    pthread_t id;
    struct topicQueue *TQ;
};

void *publisher(void *args);

void *subscriber(void *args);

void *cleaner(void *args);
