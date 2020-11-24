/*
* Description: Project 3 main -
*
* Author: Thomas Mitchell
*
* Date: 12-5-2020
*
* Notes:
* 1. N/A
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "server.h"

#define MAXQTOPICS 128

void *publisher(void *args)
{

}

void *subscriber(void *args)
{

}

void *cleaner(void *args)
{

}


int main()
{
  int i;
  struct topicQueue TQ[MAXQTOPICS];

  init(&TQ[0], "test");

  struct threadEntry pubThreads[NUMPROXIES/2];
  struct threadEntry subThreads[NUMPROXIES/2];

  for (i = 0; i < NUMPROXIES/2; i++)
  {
    struct pubARGS pubArgs = {.TQ = TQ};
    strcpy(pubArgs.test, "hello");

    pthread_create(&(pubThreads[i].id), NULL, publisher, &pubArgs);
    pubThreads[i].free  = 1;

    struct subARGS subArgs = {.TQ = TQ};
    strcpy(subArgs.test, "hello");

    pthread_create(&(subThreads[i].id), NULL, subscriber, &subArgs);
    subThreads[i].free  = 1;
  }





  return 0;
}
