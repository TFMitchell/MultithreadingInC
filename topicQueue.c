//contains en/dequeue functions as well as init/free.
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

  pthread_mutex_lock(&(TQ->mutex)); //get that TQ's lock

  if (TQ->tail - TQ->head == TQ->length) //if full
  {
    pthread_mutex_unlock(&(TQ->mutex)); //return 0 and release lock
    return 0;
  }
  //else not full

  TE->entryNum = TQ->tail + 1; //assign entry number starting at 1
  gettimeofday(&(TE->timeStamp), NULL); //record ToD
  TQ->buffer[TQ->tail % TQ->length] = *TE; //copy TE to the buffer
  (TQ->tail)++; //advance the tail

  pthread_mutex_unlock(&(TQ->mutex)); //unlock and return 1
  return 1;
}

void dequeue(struct topicQueue *TQ)
{

  pthread_mutex_lock(&(TQ->mutex)); //get that TQ's lock

  if (TQ->tail == TQ->head) //if empty, release mutex and stop function
  {
    pthread_mutex_unlock(&(TQ->mutex));
    return;
  }
  //else compare the time against delta

  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);

  float delta = currentTime.tv_sec - TQ->buffer[TQ->head % TQ->length].timeStamp.tv_sec;

  if (delta > DELTA) //only dequeue if old
  {
    (TQ->head)++;
    pthread_mutex_unlock(&(TQ->mutex));
  }
  else
  {
    pthread_mutex_unlock(&(TQ->mutex));
  }
}

int getEntry(struct topicQueue *TQ, struct topicEntry *TE, int lastEntry)
{
  int i = 0;
  pthread_mutex_lock(&(TQ->mutex)); //get that TQ's lock

  if (TQ->tail == TQ->head) //if empty
  {
    pthread_mutex_unlock(&(TQ->mutex));
    return 0;
  }
  //else look for lastEntry+1

  //first entry to check is at the head
  for (i = TQ->head; i < TQ->tail; i++) //while we're within the tail (which points to where the next entry would go),
  {
    if (TQ->buffer[i % TQ->length].entryNum == (lastEntry + 1)) //if we have a next entry to provide,
    {
      *TE = TQ->buffer[i % TQ->length]; //then return it
      pthread_mutex_unlock(&(TQ->mutex));
      return 1;
    }
  }
  //else lastEntry+1 is not in our non-empty queue

  //first entry to check is at the head
  for (i = TQ->head; i < TQ->tail; i++) //while we're within the tail (which points to where the next entry would go),
  {
    if (TQ->buffer[i % TQ->length].entryNum > (lastEntry + 1)) //return the first one that is greater than entryNum
    {
      *TE = TQ->buffer[i % TQ->length];

      pthread_mutex_unlock(&(TQ->mutex));
      return TE->entryNum; //this is never 0, so it works
    }
  }

  //else they're all less than or equal to the lastEntry + 1
  pthread_mutex_unlock(&(TQ->mutex));
  return 0;
}

void setDelta(float delta)
{
  DELTA = delta;
  printf("Delta is now %d.\n", (int) DELTA);
}
