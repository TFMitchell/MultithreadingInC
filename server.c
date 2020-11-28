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

#define MAXLINE 256

char done = 0; //global to signal to all threads easily
pthread_mutex_t mainMTX = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER; //used for conditional waiting (to make sure threads start at the same time)

void *publisher(void *args)
{
  struct pubThread *threadInfo = (struct pubThread*) args; //no more need to cast when dereferencing

  //wait to start
  pthread_mutex_lock(&mainMTX);
  pthread_cond_wait(& (threadInfo->wait), &mainMTX);
  pthread_mutex_unlock(&mainMTX);

  while (!done) //while not done
  {
    struct topicEntry tmpEntry; //make the entry that needs to be enqueued
    //strcpy(tmpEntry.photoCaption, threadInfo->test); //fill in the information

    threadInfo->currentQueueID = 0;

    enqueue( &(threadInfo->TQ[threadInfo->currentQueueID]) , &tmpEntry); // enqueue it

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
    struct topicEntry tmpEntry; //empty entry that will be returned

    threadInfo->currentQueueID = 0;

    getEntry( &(threadInfo->TQ[threadInfo->currentQueueID]) , &tmpEntry, 0); // get the entry back

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
  int i;
  struct cleanThread *threadInfo = (struct cleanThread*) args; //no more need to cast when dereferencing

  //wait to start
  pthread_mutex_lock(&mainMTX);
  pthread_cond_wait(& (threadInfo->wait), &mainMTX);
  pthread_mutex_unlock(&mainMTX);

  while (!done)
  {
    for (i = 0; i < MAXQTOPICS; i++)
    {
      //printf("cleaner name: %s\n", threadInfo->TQ[i].name);
      dequeue( &(threadInfo->TQ[i]) );
    }
  }

  return NULL;
}



int main()
{
  int i = 0, currentPubFile = 0, currentSubFile = 0;
  size_t len = MAXLINE;
  char *buffer = malloc (len * sizeof(char));
  char *tokens[5];
  char *savePtr;
  struct topicQueue TQ[MAXQTOPICS]; //make the queue of topics

  for (i = 0; i < MAXQTOPICS; i++)
  {
    init( &(TQ[i]) , "init", 0);
  }

  struct pubThread pubThreads[NUMPROXIES/2]; //each of these holds the arguments that are passed to publisher() or subscriber()
  struct subThread subThreads[NUMPROXIES/2]; //including info about the thread
  struct cleanThread cleanThreadInfo = {.TQ = TQ, .wait = (pthread_cond_t) PTHREAD_COND_INITIALIZER};
  char *publisherFiles[MAXPUBSnSUBS];
  char *subscriberFiles[MAXPUBSnSUBS];

  for (i = 0; i < MAXPUBSnSUBS; i++)
  {
    publisherFiles[i] = malloc (MAXLINE * sizeof(char));
    subscriberFiles[i] = malloc (MAXLINE * sizeof(char));

    publisherFiles[i][0] = '\0';
    subscriberFiles[i][0] = '\0';
  }

  for (i = 0; i < NUMPROXIES/2; i++) //for each thread in both pools
  {
    //initialize thread info
    pubThreads[i].TQ = TQ;
    pubThreads[i].free = 1;
    pubThreads[i].wait = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

    pthread_create(&(pubThreads[i].id), NULL, publisher, &(pubThreads[i])); //then make the thread

    //init thread info
    subThreads[i].TQ = TQ;
    subThreads[i].free = 1;
    subThreads[i].wait = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

    pthread_create(&(subThreads[i].id), NULL, subscriber, &(subThreads[i])); //make the thread
  }


  pthread_create(&(cleanThreadInfo.id), NULL, cleaner, &cleanThreadInfo);

  sleep(2); //delay after pool is made


  do
  {
    if (getline(&buffer, &len, stdin) == 0)
      printf("Instructions didn't include \"start\"\n");

    for (i = 0; i < 5; i++)
    {
      tokens[0] = NULL;
    }

    tokens[0] = strtok_r(buffer, " \n", &savePtr);

    i = 1;
    while ( (tokens[i] = strtok_r(NULL, " \n", &savePtr)) != NULL )
    {
      i++;
    }

    inputHandler(TQ, tokens, publisherFiles, subscriberFiles);

  } while (inputHandler(TQ, tokens, publisherFiles, subscriberFiles));

  pthread_cond_broadcast(&(cleanThreadInfo.wait)); //starting cleaner

  while (!done) //main scheduler
  {
    //publisher work
    for (i = currentPubFile, i <MAXPUBSnSUBS, i++)
    {
      
    }



  }

  for (i = 0; i < NUMPROXIES/2; i++) //signal threads to continue, which allows them to see that the done flag is set
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



  pthread_join(cleanThreadInfo.id, NULL);

  for (i = 0; i < MAXQTOPICS; i++)
  {
      tqFree(&TQ[i]);
  }

  free(buffer);

  for (i = 0; i < MAXPUBSnSUBS; i++)
  {
    free(publisherFiles[i]);
    free(subscriberFiles[i]);

  }



  return 0;
}
