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

void queryPublishers(char **publisherFiles)
{
  for (int i = 0; i < MAXPUBSnSUBS; i++)
  {
    if ( strcmp(publisherFiles[i], "") ) //if not free, print details
      printf("Publisher #%d is ready to read from %s.\n", i, publisherFiles[i]);
  }
}

void querySubscribers(char **subscriberFiles)
{
  for (int i = 0; i < MAXPUBSnSUBS; i++)
  {
    if ( strcmp(subscriberFiles[i], "") ) //if not free, print details
      printf("Subscriber #%d is ready to read from %s.\n", i, subscriberFiles[i]);
  }
}

int inputHandler(struct topicQueue *TQ, char **tokens, char **publisherFiles, char **subscriberFiles)
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
      queryPublishers(publisherFiles);

    else // is subscriber
      querySubscribers(subscriberFiles);
  }
  else if ( ( !strcmp(tokens[0], "add") ))
  {

    if ( !strcmp(tokens[1], "publisher") )
    {
      for (i = 0; i < MAXPUBSnSUBS; i++)
      {
        if ( !strcmp(publisherFiles[i], "") )
        {
          strcpy(publisherFiles[i], tokens[2]);
          break;
        }
      }

    }
    else //tokens[1] is subscriber
    {

      for (i = 0; i < MAXPUBSnSUBS; i++)
      {
        if ( !strcmp(subscriberFiles[i], "") )
        {
          strcpy(subscriberFiles[i], tokens[2]);
          break;
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
    return 0;

  }
  return 1;
}
