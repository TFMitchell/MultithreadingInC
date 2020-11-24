#include <pthread.h>
#include "topicQueue.h"

#define NUMPROXIES 50


struct threadEntry{
  pthread_t id;
  char free;

};

struct pubARGS{
    struct topicQueue *TQ;
    char *test;
};

struct subARGS{
    struct topicQueue *TQ;
    char *test;
};

struct cleanARGS{
    struct topicQueue *TQ;
    char *test;
};
