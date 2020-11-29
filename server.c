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
    FILE *file;
    int i = 0;

    if ( (file = fopen (threadInfo->fileName, "r") ) != NULL )
    {
      size_t len = MAXLINE;
      char *buffer = malloc (MAXLINE * sizeof(char));
      char *savePtr;

      while (getline(&buffer, &len, file) > 0)
      {
        char *tokens[] = {NULL, NULL, NULL, NULL};
        struct topicEntry tmpEntry; //make the entry that needs to be enqueued

        tokens[0] = strtok_r(buffer, " \"\n", &savePtr);

        i = 1;
        while ( (tokens[i] = strtok_r(NULL, " \"\n", &savePtr)) != NULL)
        {
          i++;
        }

        if ( strcmp(tokens[0], "put") ) //we're skipping anything other than put for now
          continue;

        threadInfo->currentQueueID = atoi(tokens[1]);

        tmpEntry.pubID = atoi(tokens[1]);
        strcpy(tmpEntry.photoURL, tokens[2]);
        strcpy(tmpEntry.photoCaption, tokens[3]);

        enqueue( &(threadInfo->TQ[threadInfo->currentQueueID]) , &tmpEntry); // enqueue it

      }
      free(buffer);
      fclose(file);
      threadInfo->free = 1; //this thread is now free
    }

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

  struct pubThread *threadInfo = (struct pubThread*) args; //no more need to cast when dereferencing

  //wait to start
  pthread_mutex_lock(&mainMTX);
  pthread_cond_wait(& (threadInfo->wait), &mainMTX);
  pthread_mutex_unlock(&mainMTX);

  while (!done) //while not done
  {
    FILE *file;
    int lastEntry = 0;

    if ( (file = fopen (threadInfo->fileName, "r") ) != NULL )
    {
      size_t len = MAXLINE;
      char *buffer = malloc (MAXLINE * sizeof(char));
      char *savePtr;

      while (getline(&buffer, &len, file) > 0)
      {
        char *tokens[] = {NULL, NULL};
        struct topicEntry tmpEntry; //make the entry that will be passed to getEntry

        tokens[0] = strtok_r(buffer, " \"\n", &savePtr);

        tokens[1] = strtok_r(NULL, " \"\n", &savePtr);


        if ( strcmp(tokens[0], "get") ) //we're skipping anything other than put for now
          continue;

        threadInfo->currentQueueID = atoi(tokens[1]);


        getEntry( &(threadInfo->TQ[atoi(tokens[1])]) , &tmpEntry, lastEntry); // enqueue it

        lastEntry = tmpEntry.entryNum;

        printf("Photo: %s Caption: %s\n", tmpEntry.photoURL, tmpEntry.photoCaption);

      }
      free(buffer);
      fclose(file);
      threadInfo->free = 1; //this thread is now free
    }

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
  char *savePtr;
  char *tokens[5];
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
    pubThreads[i].fileName = "";
    pubThreads[i].wait = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

    pthread_create(&(pubThreads[i].id), NULL, publisher, &(pubThreads[i])); //then make the thread

    //init thread info
    subThreads[i].TQ = TQ;
    subThreads[i].free = 1;
    subThreads[i].fileName = "";
    subThreads[i].wait = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

    pthread_create(&(subThreads[i].id), NULL, subscriber, &(subThreads[i])); //make the thread
  }


  pthread_create(&(cleanThreadInfo.id), NULL, cleaner, &cleanThreadInfo);

  pthread_cond_broadcast(&(cleanThreadInfo.wait)); //starting cleaner

  for (i = 0; i < NUMPROXIES/2; i++) //start the threads running and waiting
  {
    pthread_cond_broadcast(&(pubThreads[i].wait));
    pthread_cond_broadcast(&(subThreads[i].wait));
  }

  sleep(2); //delay after pool is made


  do
  {
    if (getline(&buffer, &len, stdin) == 0)
      printf("Instructions didn't include \"start\"\n"); //the program will hang, but better than exiting with leaks


    for (i = 0; i < 5; i++)
    {
      tokens[i] = NULL;
    }

    tokens[0] = strtok_r(buffer, " \"\n", &savePtr);

    i = 1;
    while ( (tokens[i] = strtok_r(NULL, " \"\n", &savePtr)) != NULL )
    {
      i++;
    }

  } while (inputHandler(TQ, tokens, publisherFiles, subscriberFiles));



  while (!done)
  {
    done = 1; //assume we're done until proven otherwise
    for (i = 0; i < NUMPROXIES/2; i++)
    {
      if (pubThreads[i].free == 1)
      {
        if ( strcmp(publisherFiles[currentPubFile], "") ) //if there is a file at this position
        {
          pubThreads[i].fileName = publisherFiles[currentPubFile++]; //set it, and advance the count

          pubThreads[i].free = 0; //no longer free
          pthread_cond_broadcast( &(pubThreads[i].wait)); //wake it up
          done = 0; //we can't be done now.
        }

       }
       else //it is busy
        done = 0; //we're not done

      if (subThreads[i].free == 1)
      {
        if ( strcmp(subscriberFiles[currentSubFile], "") ) //if there is a file at this position
        {
          subThreads[i].fileName = subscriberFiles[currentSubFile++]; //set it, and advance the count

          subThreads[i].free = 0; //no longer free
          pthread_cond_broadcast( &(subThreads[i].wait)); //wake it up
          done = 0; //we can't be done now.
        }

       }
       else //it is busy
        done = 0; //we're not done

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
