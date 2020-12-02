
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "server.h"

char done = 0; //global to signal to all threads easily
pthread_mutex_t mainMTX = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER; //used for conditional waiting (to make sure threads start at the same time)

char *getDone()
{
  return &done;
}

void *publisher(void *args)
{
  struct pubThread *threadInfo = (struct pubThread*) args; //no more need to cast when dereferencing

  while (!done) //while not done
  {
    FILE *file;
    int i = 0;

    if ( (file = fopen (threadInfo->fileName, "r") ) != NULL ) //if we can open the file
    {
      size_t len = MAXLINE;
      char *tokens[30]; //30 should leave plenty of words for the caption
      char *buffer = malloc (MAXLINE * sizeof(char)); //getting ready to read from file
      char *savePtr;

      while (getline(&buffer, &len, file) > 0)
      {
        struct topicEntry tmpEntry; //make the entry that needs to be enqueued

        for (i = 0; i < 30; i++) //nullify since we have a variable amount of tokens
        {
          tokens[i] = NULL;
        }

        //this block reads into tokens[]
        tokens[0] = strtok_r(buffer, " \"\n", &savePtr);
        i = 1;
        while ( (tokens[i] = strtok_r(NULL, " \"\n", &savePtr)) != NULL)
        {
          i++;
        }

        if ( !strcmp(tokens[0], "sleep") ) //if we need to sleep
        {
          sleep( atoi(tokens[1]) / 1000); //sleep for that many ms, then read next entry
          continue;
        }

        else if ( !strcmp(tokens[0], "stop") )
        {
          break; //we're done
        }

        //else we are actually adding an entry

        tmpEntry.pubID = (int) threadInfo->id;
        strcpy(tmpEntry.photoURL, tokens[2]); //give it the url

        //next up is setting up the caption
        char *tmpCaption = malloc (MAXLINE * sizeof(char));
        tmpCaption[0] = '\0';
        i = 3;
        while (tokens[i] != NULL) //starting at 3, keep going until there's no more word in the caption
        {
          strcat(tmpCaption, tokens[i]);
          strcat(tmpCaption, " "); //add a space
          i++;
        }
        strcpy(tmpEntry.photoCaption, tmpCaption); //finally, place the caption into the tmp Entry

        do
        {
          sched_yield();
        } while ( enqueue( &(threadInfo->TQ[ atoi(tokens[1]) ]) , &tmpEntry) == 0 ); //==0 means was full

        free(tmpCaption);
      }

      //we're at eof now
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
  struct subThread *threadInfo = (struct subThread*) args; //no more need to cast when dereferencing

  while (!done) //while not done
  {
    FILE *file;
    int lastEntry = 0;

    if ( (file = fopen (threadInfo->fileName, "r") ) != NULL ) //if we could actually open the file
    {
      size_t len = MAXLINE;
      char *buffer = malloc (MAXLINE * sizeof(char));
      char *savePtr;
      char *tokens[] = {NULL, NULL}; //we only have a maximum of two arguments here

      while (getline(&buffer, &len, file) > 0)
      {
        struct topicEntry tmpEntry; //make the empty entry that will be passed to getEntry

        tokens[0] = strtok_r(buffer, " \"\n", &savePtr); //reading the two arguments into tokens[]
        tokens[1] = strtok_r(NULL, " \"\n", &savePtr);

        if ( !strcmp(tokens[0], "sleep") ) //if we need to sleep
        {
          sleep( atoi(tokens[1]) / 1000); //then sleep
          continue;
        }

        else if ( !strcmp(tokens[0], "stop") )
        {
          break; //we're done
        }

        for (int i = 0; i < 3; i++)
        {
          sched_yield();
          if (getEntry( &(threadInfo->TQ[ atoi(tokens[1]) ]) , &tmpEntry, lastEntry)) // get the entry (if we could)
          {
            lastEntry = tmpEntry.entryNum; //update last entry
            printf("Photo: %s Caption: %s\n", tmpEntry.photoURL, tmpEntry.photoCaption);
            break;
          }
        }
      }
      //we've reached eof
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
    sleep (2); //call every 2 seconds
    for (i = 0; i < MAXQTOPICS; i++)
    {
      dequeue( &(threadInfo->TQ[i]) ); //try to dequeue
      sched_yield();
    }
  }
  return NULL;
}
