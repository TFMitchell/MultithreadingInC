//contains en/dequeue functions as well as init.
#include "topicQueue.h"
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

float DELTA = 10.0;

void init(struct topicQueue *TQ, char *name, int length)
{
  strcpy(TQ->name, name);
  TQ->length = length;
  TQ->buffer = malloc (length * sizeof(struct topicEntry));
  TQ->head = 0;
  TQ->tail = 0;
  TQ->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

}

void tqFree(struct topicQueue *TQ)
{
  free(TQ->buffer);
}

int enqueue(struct topicQueue *TQ, struct topicEntry *TE)
{
  pthread_mutex_lock(&(TQ->mutex));

  if (TQ->tail - TQ->head == TQ->length) //if full
  {
    pthread_mutex_unlock(&(TQ->mutex)); //return 0 and release lock
    sched_yield(); //deprioritize
    return 0;
  }

  TE->entryNum = TQ->tail + 1; //assign entry number starting at 1
  gettimeofday(&(TE->timeStamp), NULL); //record ToD
  TQ->buffer[TQ->tail % TQ->length] = *TE; //copy TE to the buffer
  TQ->tail++; //advance the tail

  pthread_mutex_unlock(&(TQ->mutex)); //unlock and return 1
  return 1;
}

void dequeue(struct topicQueue *TQ)
{
  pthread_mutex_lock(&(TQ->mutex));

  if (TQ->tail == TQ->head) //if empty, release mutex and return
  {
    pthread_mutex_unlock(&(TQ->mutex));
    sched_yield();
    return;
  }
  //else compare the time against delta
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);

  float delta = currentTime.tv_sec - TQ->buffer[TQ->head].timeStamp.tv_sec;

  if (delta > DELTA) //only dequeue if old
    TQ->head++;

  pthread_mutex_unlock(&(TQ->mutex));
  sched_yield();
}

int getEntry(struct topicQueue *TQ, struct topicEntry *TE, int lastEntry)
{
  pthread_mutex_lock(&(TQ->mutex));

  if (TQ->tail == TQ->head) //if empty
  {
    pthread_mutex_unlock(&(TQ->mutex));
    sched_yield();
    return 0;
  }

  //look for lastEntry+1
  int i = TQ->head; //first entry to check is at the head
  while (i < TQ->tail)
  {
    if (TQ->buffer[(TQ->head + i) % TQ->length].entryNum == lastEntry + 1) //if we have a next entry to provide
    {
      *TE = TQ->buffer[(TQ->head + i) % TQ->length]; //return it
      pthread_mutex_unlock(&(TQ->mutex));
      return 1;
    }
  }

  //if lastEntry+1 is not in our non-empty queue

  i = TQ->head; //first entry to check is head
  while (i < TQ->tail)
  {
    if (TQ->buffer[(TQ->head + i) % TQ->length].entryNum > lastEntry + 1) //return the first one that has a greater entryNum
    {
      *TE = TQ->buffer[(TQ->head + i) % TQ->length];
      pthread_mutex_unlock(&(TQ->mutex));
      return TE->entryNum;
    }
  }
  //else they're all less than or equal to the lastEntry + 1
  pthread_mutex_unlock(&(TQ->mutex));
  sched_yield();
  return 0;
}

void setDelta(float delta)
{
  DELTA = delta;
  printf("Delta is now %d.\n", (int) DELTA);
}
