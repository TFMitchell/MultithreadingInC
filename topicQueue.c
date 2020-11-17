//contains en/dequeue functions as well as init.
#include "topicQueue.h"
#include <stdlib.h>

int init(struct topicQueue *TQ, int length)
{
  TQ->buffer = malloc (length * sizeof(struct topicEntry));

  TQ->head = 0;
  TQ->tail = 0;
  TQ->length = length;

  return 0;
}


int enqueue(struct topicQueue *TQ, struct topicEntry *TE)
{
  return 1;
}

int dequeue(struct topicQueue *TQ, struct topicEntry *TE)
{
  return 1;
}
