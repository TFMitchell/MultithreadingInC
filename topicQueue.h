//stores given topicEntry struct and the topicQueue of topicEntries
#include <sys/time.h>
#include <pthread.h>

#define URLSIZE 1024
#define CAPSIZE 256
#define MAXNAME 50
#define MAXQENTRIES 10

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
  pthread_mutex_t mutex;
};

int init(struct topicQueue *TQ, char *name);

int enqueue(struct topicQueue *TQ, struct topicEntry *TE);

void dequeue(struct topicQueue *TQ);

int getEntry(struct topicQueue *TQ, struct topicEntry *TE, int lastEntry);
