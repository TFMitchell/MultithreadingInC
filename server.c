
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "server.h"
#include "htmlHandler.h"

char done = 0; //global to signal to all threads easily
pthread_cond_t cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

void *publisher(void *args)
{
  struct pubThread *threadInfo = (struct pubThread*) args; //no more need to cast when dereferencing

  while (!done) //while not done
  {

    FILE *file;
    int i = 0;

    if ( threadInfo->fileName == NULL );

    else if ( (file = fopen (threadInfo->fileName, "r") ) != NULL ) //if we can open the file
    {
      size_t len = MAXLINE;
      char *tokens[30]; //30 should leave plenty of words for the caption
      char *buffer = malloc (MAXLINE * sizeof(char)); //getting ready to read from file
      char *savePtr;

      while (getline(&buffer, &len, file) > 0)
      {
        sched_yield();
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
          printf("Proxy thread %u - type: Publisher - Executed command: sleep %s \n", (unsigned int) pthread_self(), tokens[1]);
          sleep( atoi(tokens[1]) / 1000); //sleep for that many ms, then read next entry
          continue;
        }

        else if ( !strcmp(tokens[0], "stop") )
        {
          printf("Proxy thread %u - type: Publisher - Executed command: stop \n", (unsigned int) pthread_self());
          break; //we're done
        }

        //else we are actually adding an entry

        //tmpEntry.pubID = (int) threadInfo->id;
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

        printf("Proxy thread %u - type: Publisher- Executed command: put %s %s %s \n", (unsigned int) pthread_self(), tokens[1], tokens[2], tmpCaption);

        while ( enqueue( &(threadInfo->TQ[ atoi(tokens[1]) ]) , &tmpEntry) == 0 ) //==0 means it was full
          sched_yield();


        free(tmpCaption);
      }

      //we're at eof now
      free(buffer);
      fclose(file);
      threadInfo->free = 1; //this thread is now free
      threadInfo->fileName = NULL;

    }



    pthread_mutex_lock(&mtx);
    pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);

    //it will be awaken when all files are read from, so that the while statement will evaluate to false and end the thread
  }
  return NULL; //end
}

void *subscriber(void *args)
{
  struct subThread *threadInfo = (struct subThread*) args; //no more need to cast when dereferencing

  while (!done) //while not done
  {
    //pthread_mutex_lock(&(threadInfo->mutex));
    FILE *file;
    int i = 0;

    if ( threadInfo->fileName == NULL );

    else if ( (file = fopen (threadInfo->fileName, "r") ) != NULL ) //if we could actually open the file
    {
      struct topicEntry tmpEntry;
      char *buffer = malloc (MAXLINE * sizeof(char));
      FILE *htmlFile;
      size_t len = MAXLINE;
      char *savePtr;
      char *tokens[] = {NULL, NULL}; //we only have a maximum of two arguments here


      strcpy(buffer, "");
      strcat(buffer, "subscriber_");
      strcat(filename, threadID);
      strcat(buffer, ".html");

      if ( (htmlFile = fopen(buffer, "w+")) == NULL)
      {
        printf("HTML file error in sub %u", (unsigned int) pthread_self());
        fclose(file);
        threadInfo->free = 1; //this thread is now free
        continue;
      }

      initializeFile(htmlFile, (unsigned int) pthread_self());

      while (getline(&buffer, &len, file) > 0)
      {
        sched_yield();
        tokens[0] = strtok_r(buffer, " \"\n", &savePtr); //reading the two arguments into tokens[]
        tokens[1] = strtok_r(NULL, " \"\n", &savePtr);

        if ( !strcmp(tokens[0], "sleep") ) //if we need to sleep
        {
          printf("Proxy thread %u - type: Subscriber - Executed command: sleep %s\n", (unsigned int) pthread_self(), tokens[1]);
          sleep( atoi(tokens[1]) / 1000); //then sleep
          continue;
        }

        else if ( !strcmp(tokens[0], "stop") )
        {
          printf("Proxy thread %u - type: Subscriber - Executed command: stop\n", (unsigned int) pthread_self());
          break; //we're done
        }


        char *URLs[threadInfo->TQ[ atoi(tokens[1]) ].length];
        char *captions[threadInfo->TQ[ atoi(tokens[1]) ].length];
        int lastEntry = 0;

        for (i = 0; i < threadInfo->TQ[ atoi(tokens[1]) ].length; i++)
        {
          URLs[i] = NULL;
          captions[i] = NULL;
        }

        printf("Proxy thread %u - type: Subscriber - Executed command: get %s \n", (unsigned int) pthread_self(), tokens[1]);


        i = 0;
        while(getEntry( &(threadInfo->TQ[ atoi(tokens[1]) ]) , &tmpEntry, lastEntry))
        {
          lastEntry = tmpEntry.entryNum;
          URLs[i] = tmpEntry.photoURL;
          captions[i] = tmpEntry.photoCaption;
          i++;
        }

        addEntry(file, URLs, captions, tokens[i]);

      }
      //we've reached eof
      closeFile(htmlFile);
      free(buffer);
      fclose(file);
      threadInfo->free = 1; //this thread is now free
      threadInfo->fileName = NULL;
    }

    // now wait for it to be awoken again
    //while (threadInfo->free && !done)
    //  sched_yield();
    pthread_mutex_lock(&mtx);
    pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
    //it will be awaken when all files are read from, so that the while statement will evaluate to false and end the thread
  }
  return NULL; //end
}

void *cleaner(void *args)
{
  int i;
  struct cleanThread *threadInfo = (struct cleanThread*) args; //no more need to cast when dereferencing

  //wait to start
  pthread_mutex_lock(&mtx);
  pthread_cond_wait(&cond, &mtx);
  pthread_mutex_unlock(&mtx);

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
