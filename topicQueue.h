//stores given topicEntry struct and the topicQueue of topicEntries
#include <sys/time.h>

#define URLSIZE 1024
#define CAPSIZE 256

struct topicEntry{
  int entryNum;
  struct timeval timeStamp;
  int pubID;
  char photoURL[URLSIZE];
  char photoCaption[CAPSIZE];
};

struct topicQueue{
  struct topicEntry *buffer;
  int head;
  int tail;
  int length;
};

int init(struct topicQueue *TQ, int length);

int enqueue(struct topicQueue *TQ, struct topicEntry *TE);

int dequeue(struct topicQueue *TQ, struct topicEntry *TE);
