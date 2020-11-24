//contains en/dequeue functions as well as init.
#include "topicQueue.h"
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>

int init(struct topicQueue *TQ, char *name)
{
  strcpy(TQ->name, name);
  TQ->buffer = malloc (MAXQENTRIES * sizeof(struct topicEntry));
  TQ->head = 0;
  TQ->tail = 0;
  TQ->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

  return 0;
}


int enqueue(struct topicQueue *TQ, struct topicEntry *TE)
{
  pthread_mutex_lock(&(TQ->mutex));

  if (TQ->tail - TQ->head == MAXQENTRIES)
  {
    pthread_mutex_unlock(&(TQ->mutex));
    sched_yield();
    return 0;
  }

  TE->entryNum = TQ->tail + 1;//starting at 1
  gettimeofday(&(TE->timeStamp), NULL);
  TQ->buffer[TQ->tail % MAXQENTRIES] = *TE;
  TQ->tail++;

  pthread_mutex_unlock(&(TQ->mutex));

  return 1;
}

void dequeue(struct topicQueue *TQ)
{
  pthread_mutex_lock(&(TQ->mutex));

  if (TQ->tail - TQ->head == MAXQENTRIES)
  {
    pthread_mutex_unlock(&(TQ->mutex));
    sched_yield();
    return;
  }
  TQ->head++;

  pthread_mutex_unlock(&(TQ->mutex));
  sched_yield();
  return;
}

int getEntry(struct topicQueue *TQ, struct topicEntry *TE, int lastEntry)
{
  pthread_mutex_lock(&(TQ->mutex));

  if (TQ->tail == TQ->head) //empty
  {
    pthread_mutex_unlock(&(TQ->mutex));
    sched_yield();
    return 0;
  }

  //look for lastEntry+1

  int i = TQ->head; //first entry to check
  while (i < TQ->tail)
  {

    if (TQ->buffer[(TQ->head + i) % MAXQENTRIES].entryNum == lastEntry + 1)
    {
      *TE = TQ->buffer[(TQ->head + i) % MAXQENTRIES];
      pthread_mutex_unlock(&(TQ->mutex));
      return 1;
    }
  }

  //if lastEntry+1 is not in our non-empty queue

  i = TQ->head; //first entry to check
  while (i < TQ->tail)
  {

    if (TQ->buffer[(TQ->head + i) % MAXQENTRIES].entryNum > lastEntry + 1) //return the first one that has a greater entryNum
    {

      *TE = TQ->buffer[(TQ->head + i) % MAXQENTRIES];
      pthread_mutex_unlock(&(TQ->mutex));
      return TE->entryNum;
    }
  }
  //else they're all less than or equal to the lastEntry + 1
  pthread_mutex_unlock(&(TQ->mutex));
  sched_yield();

  return 0;
}
