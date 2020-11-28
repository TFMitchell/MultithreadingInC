#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "inputHandler.h"


void queryTopics(struct topicQueue *TQ)
{
  for (int i = 0; i < MAXQTOPICS; i++)
  {
    if ( strcmp(TQ[i].name, "init") ) //if it's not init
      printf("Topic ID #%d: %s Length %d.\n", i, TQ[i].name, TQ[i].length);
  }
}

void queryPublishers(struct pubThread *pubThreads)
{
  for (int i = 0; i < NUMPROXIES/2; i++)
  {
    if (pubThreads[i].free == 0) //if not free, print details
      printf("Publisher #%d is active.\n", (int) pubThreads[i].id);
  }
}

void querySubscribers(struct subThread *subThreads)
{
  for (int i = 0; i < NUMPROXIES/2; i++)
  {
    if (subThreads[i].free == 0) //if not free, print details
      printf("Subscriber #%d is active.\n", (int) subThreads[i].id);
  }
}

void inputHandler(struct topicQueue *TQ, char **tokens, struct pubThread *pubThreads, struct subThread *subThreads)
{
  int i;

  //pseudo-switch statement to see what the config file gave us
  if ( !strcmp(tokens[0], "create") ) //"topic" doesn't matter
  {
    tqFree( &(TQ[atoi(tokens[2])]) );

    init( &(TQ[atoi(tokens[2])]) , tokens[3], atoi(tokens[4]));
  }

  else if ( !strcmp(tokens[0], "query") )
  {
    if ( !strcmp(tokens[1], "topics") )
      queryTopics(TQ);

    else if ( !strcmp(tokens[1], "publishers") )
      queryPublishers(pubThreads);

    else // is subscriber
      querySubscribers(subThreads);
  }
  else if ( ( !strcmp(tokens[0], "add") ))
  {
    if ( !strcmp(tokens[1], "publisher") )
    {
      for (i = 0; i < NUMPROXIES/2; i++)
      {
        if (pubThreads[i].free == 1)
        {
          pubThreads[i].free = 0;
          //set the arguments (probably a char* that has the name of the file)
          break;
        }
        else if (i == NUMPROXIES/2 - 1)
        {
          //have to wait on a thread
        }
      }

    }
    else //tokens[1] is subscriber
    {
      for (i = 0; i < NUMPROXIES/2; i++)
      {
        if (subThreads[i].free == 1)
        {
          subThreads[i].free = 0;
          //set the arguments (probably a char* that has the name of the file)
          break;
        }
        else if (i == NUMPROXIES/2 - 1)
        {
          //have to wait on a thread
        }
      }
    }
  }
  else if ( !strcmp(tokens[0], "delta") )
  {
    setDelta(atoi(tokens[1]));
  }

  else //start
  {
    for (i = 0; i < NUMPROXIES/2; i++) //starting the threads
    {
      pthread_cond_broadcast(&(pubThreads[i].wait));
      pthread_cond_broadcast(&(subThreads[i].wait));
    }

  }
}
