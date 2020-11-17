//contains en/dequeue functions as well as init.
#include "topicQueue.h"
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

int init(struct topicQueue *TQ, int length, char *name)
{
  strcpy(TQ->name, name);
  TQ->buffer = malloc (length * sizeof(struct topicEntry));
  TQ->head = 0;
  TQ->tail = 0;
  TQ->length = length;

  return 0;
}


int enqueue(struct topicQueue *TQ, struct topicEntry *TE)
{
  if (TQ->tail - TQ->head == TQ->length)
    return -1;

  TE->entryNum = TQ->tail;
  gettimeofday(&(TE->timeStamp), NULL);
  TQ->buffer[TQ->tail % TQ->length] = *TE;
  TQ->tail++;

  return 0;
}

int dequeue(struct topicQueue *TQ, struct topicEntry *TE)
{
  if (TQ->tail == TQ->head)
    return -1;

  *TE = TQ->buffer[TQ->head % TQ->length];

  TQ->head++;

  return 0;
}
