#include <pthread.h>
#include "inputHandler.h"

#define MAXLINE 256

extern char done;// = 0; //global to signal to all threads easily
extern pthread_cond_t cond;// = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
extern pthread_mutex_t mtx;// = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

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
