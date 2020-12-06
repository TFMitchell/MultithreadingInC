//stores given topicEntry struct and the topicQueue of topicEntries. Defines functions for working with the ring buffer.
#include <sys/time.h>
#include <pthread.h>

#define URLSIZE 1024
#define CAPSIZE 1024
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
  int length;
  struct topicEntry *buffer;
  int head;
  int tail;
  pthread_mutex_t mutex;
};

void init(struct topicQueue *TQ, char *name, int length);

void tqFree(struct topicQueue *TQ);

int enqueue(struct topicQueue *TQ, struct topicEntry *TE);

void dequeue(struct topicQueue *TQ);

int getEntry(struct topicQueue *TQ, struct topicEntry *TE, int lastEntry);

void setDelta(float delta);
