#include <stdio.h>
#include <stdlib.h>

void initializeFile(FILE *file, char *threadID, char *fileName);

void addEntry(FILE *file, char **URLs, char **captions, char *topic);

void closeFile(FILE *file);
