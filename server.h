#include <pthread.h>
#include "inputHandler.h"


void *publisher(void *args);

void *subscriber(void *args);

void *cleaner(void *args);
