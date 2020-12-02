#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "inputHandler.h"


void queryTopics(struct topicQueue *TQ)
{
  for (int i = 0; i < MAXQTOPICS; i++)
  {
    if ( strcmp(TQ[i].name, "init") ) //if it's not init, print
      printf("Topic ID #%d: %s Length %d.\n", i, TQ[i].name, TQ[i].length);
  }
}

void queryPublishers(char **publisherFiles)
{
  for (int i = 0; i < MAXPUBSnSUBS; i++) //for all pub filenames
  {
    if ( strcmp(publisherFiles[i], "") ) //if not blank at this position, print details
      printf("Publisher #%d is ready to read from %s.\n", i, publisherFiles[i]);
  }
}

void querySubscribers(char **subscriberFiles)
{
  for (int i = 0; i < MAXPUBSnSUBS; i++) //for all sub filenames
  {
    if ( strcmp(subscriberFiles[i], "") ) //if not blank at this position, print details
      printf("Subscriber #%d is ready to read from %s.\n", i, subscriberFiles[i]);
  }
}

int inputHandler(struct topicQueue *TQ, char **tokens, char **publisherFiles, char **subscriberFiles)
{
  int i;

  //pseudo-switch statement to see what the config file gave us
  if ( !strcmp(tokens[0], "create") ) //"topic" doesn't matter
  {
    tqFree( &(TQ[atoi(tokens[2])]) ); //replace the init one...

    init( &(TQ[atoi(tokens[2])]) , tokens[3], atoi(tokens[4])); ///...with one with a proper name
  }

  else if ( !strcmp(tokens[0], "query") ) //we're querying for one of three things
  {
    if ( !strcmp(tokens[1], "topics") ) //this will print out the topics that aren't named "init"
      queryTopics(TQ);

    else if ( !strcmp(tokens[1], "publishers") ) //this simply prints out the non-empty sections of the pub file name list
      queryPublishers(publisherFiles);

    else // is subscriber
      querySubscribers(subscriberFiles); //this simply prints out the non-empty sections of the sub file name list
  }

  else if ( ( !strcmp(tokens[0], "add") )) //we could be adding a publisher or subscriber
  {

    if ( !strcmp(tokens[1], "publisher") ) //if it's a pub,
    {
      for (i = 0; i < MAXPUBSnSUBS; i++)  //loop through all the pub files and once you find one that's empty, copy the filename to it
      {
        if ( !strcmp(publisherFiles[i], "") )
        {
          strcpy(publisherFiles[i], tokens[2]);
          break;
        }
      }

    }
    else //if it's a sub,
    {
      for (i = 0; i < MAXPUBSnSUBS; i++) //loop through all the pub files and once you find one that's empty, copy the filename to it
      {
        if ( !strcmp(subscriberFiles[i], "") )
        {
          strcpy(subscriberFiles[i], tokens[2]);
          break;
        }
      }

    }
  }
  else if ( !strcmp(tokens[0], "delta") ) //simple, just change the delta
  {
    setDelta(atoi(tokens[1]));
  }

  else //start
  {
    return 0;

  }
  return 1;
}
