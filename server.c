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
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "server.h"

#define MAXQTOPICS 128

char done = 0;
pthread_mutex_t mainMTX = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

void *publisher(void *args)
{

  struct pubThread *threadInfo = (struct pubThread*) args; //no more need to cast when dereferencing

  //wait to start
  pthread_mutex_lock(&mainMTX);
  pthread_cond_wait(& (threadInfo->wait), &mainMTX);
  pthread_mutex_unlock(&mainMTX);

  while (!done) //while not done
  {
    threadInfo->free = 0; //we're busy now
    struct topicEntry tmpEntry; //make the entry that needs to be enqueued
    strcpy(tmpEntry.photoCaption, threadInfo->test); //fill in the information

    enqueue(threadInfo->TQ, &tmpEntry); // enqueue it


    threadInfo->free = 1; //this thread is now free

    // now wait for it to be awoken again
    pthread_mutex_lock(&mainMTX);
    pthread_cond_wait(& (threadInfo->wait), &mainMTX);
    pthread_mutex_unlock(&mainMTX);

    //it will be awaken when all files are read from, so that the while statement will evaluate to false and end the thread
  }

  return NULL; //end
}

void *subscriber(void *args)
{
  struct subThread *threadInfo = (struct subThread*) args; //no more need to cast when dereferencing

  //wait to start
  pthread_mutex_lock(&mainMTX);
  pthread_cond_wait(& (threadInfo->wait), &mainMTX);
  pthread_mutex_unlock(&mainMTX);

  while (!done) //while not done
  {
    threadInfo->free = 0; //we're busy now
    struct topicEntry tmpEntry; //empty entry that will be returned

    getEntry(threadInfo->TQ, &tmpEntry, 0); // get the entry back

    printf("Caption: %s\n", tmpEntry.photoCaption);
    fflush(stdout);

    threadInfo->free = 1; //this thread is now free

    // now wait for it to be awoken again
    pthread_mutex_lock(&mainMTX);
    pthread_cond_wait(& (threadInfo->wait), &mainMTX);
    pthread_mutex_unlock(&mainMTX);

    //it will be awaken when all files are read from, so that the while statement will evaluate to false and end the thread
  }

  return NULL; //end

}

void *cleaner(void *args)
{

}


int main()
{
  int i;
  struct topicQueue TQ[MAXQTOPICS]; //make the queue of topics

  for (i = 0; i < MAXQTOPICS; i++)
  {
    init(&TQ[i], "test");
  }

  struct pubThread pubThreads[NUMPROXIES/2]; //each of these holds the arguments that are passed to publisher() or subscriber()
  struct subThread subThreads[NUMPROXIES/2]; //including info about the thread

  for (i = 0; i < NUMPROXIES/2; i++) //for each thread in both pools
  {
    //initialize thread info
    pubThreads[i].TQ = &(TQ[0]);
    pubThreads[i].free = 1;
    pubThreads[i].wait = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

    pthread_create(&(pubThreads[i].id), NULL, publisher, &(pubThreads[i])); //then make the thread

    //init thread info
    subThreads[i].TQ = &(TQ[0]);
    subThreads[i].free = 1;
    subThreads[i].wait = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

    pthread_create(&(subThreads[i].id), NULL, subscriber, &(subThreads[i])); //make the thread
  }


  sleep(2); //delay after pool is made

  pubThreads[0].test = "hellooooo";
  pthread_cond_broadcast(&(pubThreads[0].wait));

  sleep(1);

  pthread_cond_broadcast(&(subThreads[0].wait));

  sleep(1);

  done = 1;

  for (i = 0; i < NUMPROXIES/2; i++)
  {
    pthread_cond_broadcast(&(pubThreads[i].wait));
    pthread_cond_broadcast(&(subThreads[i].wait));
  }

  for (i = 0; i < NUMPROXIES/2; i++)
  {
    pthread_join(pubThreads[i].id, NULL);
  }

  for (i = 0; i < NUMPROXIES/2; i++)
  {
    pthread_join(subThreads[i].id, NULL);

  }

  for (i = 0; i < MAXQTOPICS; i++)
  {
    tqFree(&TQ[i]);
  }




  return 0;
}
