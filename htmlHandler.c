#include "htmlHandler.h"
#include <string.h>


void initializeFile(FILE *file, unsigned int threadID)
{

}

void addEntry(FILE *file, char **URLs, char **captions, char *topic)
{
  if (URLs[0] != NULL && captions[0] != NULL);
    //printf("URL %s caption %s\n", URLs[0], captions[0]);
}

void closeFile(FILE *file)
{
  fclose(file);
}
