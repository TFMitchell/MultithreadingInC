//stores given topicEntry struct and the topicQueue of topicEntries
#include <sys/time.h>
#include <stdio.h>

#define URLSIZE 1024
#define CAPSIZE 256
#define MAXNAME 50

struct topicEntry{
  int entryNum;
  struct timeval timeStamp;
  int pubID;
  char photoURL[URLSIZE];
  char photoCaption[CAPSIZE];
};

struct topicQueue{
  char name[MAXNAME];
  struct topicEntry *buffer;
  int head;
  int tail;
  int length;
};

int init(struct topicQueue *TQ, int length, char *name);

int enqueue(struct topicQueue *TQ, struct topicEntry *TE);

int dequeue(struct topicQueue *TQ, struct topicEntry *TE);
