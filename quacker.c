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
#include "server.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main()
{
  struct topicQueue TQ[MAXQTOPICS]; //make the queue of topics
  struct pubThread pubThreads[NUMPROXIES/2]; //each of these holds the arguments that are passed to publisher() or subscriber(),
  struct subThread subThreads[NUMPROXIES/2];    //including info about the thread
  struct cleanThread cleanThreadInfo = {.TQ = TQ}; //special single one for the cleaner's args
  char *subscriberFiles[MAXPUBSnSUBS];//the list of filenames for pubs and subs
  char *publisherFiles[MAXPUBSnSUBS];
  int i = 0, currentPubFile = 0, currentSubFile = 0; //for iterating through arrays of filenames
  size_t len = MAXLINE;
  char *buffer = malloc (MAXLINE * sizeof(char));
  char *savePtr;
  char *tokens[5]; //input.txt will have max 5 arguments
  char tmpDone = 0;

  for (i = 0; i < MAXQTOPICS; i++) //initizlize the TQ array with ones with "init" as the name.
  {
    init( &(TQ[i]) , "init", 0);
  }

  for (i = 0; i < MAXPUBSnSUBS; i++) //initialize the list of filenames
  {
    publisherFiles[i] = malloc (MAXLINE * sizeof(char));
    subscriberFiles[i] = malloc (MAXLINE * sizeof(char));

    publisherFiles[i][0] = '\0';
    subscriberFiles[i][0] = '\0';
  }

  for (i = 0; i < NUMPROXIES/2; i++) //for each thread in both pools, set up arguments/info
  {
    //initialize thread info
    pubThreads[i].TQ = TQ;
    pubThreads[i].free = 1;
    pubThreads[i].fileName = NULL;

    pthread_create(&(pubThreads[i].id), NULL, publisher, &(pubThreads[i])); //then make the thread

    //init thread info
    subThreads[i].TQ = TQ;
    subThreads[i].free = 1;
    subThreads[i].fileName = NULL;

    pthread_create(&(subThreads[i].id), NULL, subscriber, &(subThreads[i])); //make the thread
  }


  pthread_create(&(cleanThreadInfo.id), NULL, cleaner, &cleanThreadInfo); //make the cleaner

  sleep(1);

  do
  {
    if (getline(&buffer, &len, stdin) == 0) //reads lines into buffer
      printf("Instructions didn't include \"start\"\n"); //the program will hang, but better than exiting with leaks

    for (i = 0; i < 5; i++) //need to nullify these since arguments don't always have 5
    {
      tokens[i] = NULL;
    }

    //this block aggregates everything into tokens and passes it off to inputHandler
    tokens[0] = strtok_r(buffer, " \"\n", &savePtr);
    i = 1;
    while ( (tokens[i] = strtok_r(NULL, " \"\n", &savePtr)) != NULL )
    {
      i++;
    }

  } while (inputHandler(TQ, tokens, publisherFiles, subscriberFiles));

  free(buffer);


  while (!tmpDone)
  {
    time_t begin = clock();
    tmpDone = 1; //assume we're tmpDone until proven otherwise
    for (i = 0; i < NUMPROXIES/2; i++) //go through all of the subs and pubs
    {
      if (pubThreads[i].free == 1) //when we find a free one of the pub type,
      {
        if ( publisherFiles[currentPubFile][0] != '\0' ) //and if there is a file that needs a proxy thread at this position,
        {
          pubThreads[i].fileName = publisherFiles[currentPubFile++]; //then set it, and advance the count
          pubThreads[i].free = 0; //no longer free
          tmpDone = 0; //we can't be tmpDone now.
        }
      }
      else //it is busy
        tmpDone = 0; //we're not tmpDone

      if (subThreads[i].free == 1) //when we find a free one of the sub type,
      {
        if ( subscriberFiles[currentSubFile][0] != '\0' ) //and if there is a file at this position that needs a proxy thread,
        {
          subThreads[i].fileName = subscriberFiles[currentSubFile++]; //then set it, and advance the count
          subThreads[i].free = 0; //no longer free
          tmpDone = 0; //we can't be tmpDone now.
        }
      }
      else //it is busy
        tmpDone = 0; //we're not tmpDone
    }
    if ( ((int) (clock() - begin)) > 1000000)
      printf("It took %d cycles for scheduler to finish\n", (int) (clock() - begin));
    pthread_mutex_lock(&mtx);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mtx);
    if ( ((int) (clock() - begin)) > 1000000)
      printf("It took %d cycles for scheduler to finish including lock\n", (int) (clock() - begin));
  }
  done = 1;

  pthread_mutex_lock(&mtx);
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mtx);

  for (i = 0; i < NUMPROXIES/2; i++) //wait for threads to exit
  {
    pthread_join(pubThreads[i].id, NULL);
    pthread_join(subThreads[i].id, NULL);
  }

  pthread_join(cleanThreadInfo.id, NULL); //wait for cleaner to exit

  for (i = 0; i < MAXQTOPICS; i++) //free the topics
  {
      tqFree(&TQ[i]);
  }


  for (i = 0; i < MAXPUBSnSUBS; i++)
  {
    free(publisherFiles[i]);
    free(subscriberFiles[i]);
  }

  return 0;
}
