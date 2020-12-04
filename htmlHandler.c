#include "htmlHandler.h"
#include <string.h>


void initializeFile(FILE *file, char *threadID, char *fileName)
{
  int i = 0;
  char *header[] = {
    "<!DOCTYPE html>",
    "<html><head>",
    "<meta http-equiv=\"content-type\" content=\"text/html; charset=windows-1252\"><title>",
    fileName,
    "</title><style>",
    "table, th, td {",
    "  border: 1px solid black;",
    "  border-collapse: collapse;",
    "}",
    "th, td {",
    "  padding: 5px;",
    "}",
    "th {",
    "  text-align: left;",
    "}",
    "</style>",
    "</head>",
    "<body>",
    NULL
  };

  i = 0;
  while (header[i] != NULL)
  {
    fprintf(file, "%s\n", header[i]);
    i++;
  }

  fprintf(file, "\n<h1>Subscriber: subscriber_&lt;%s&gt;.html </h1>\n\n\n", threadID);


}

void addEntry(FILE *file, char **URLs, char **captions, char *topic)
{
  int i = 0;
  if (URLs[0][0] == '\0' || captions[0][0] == '\0')
    return;

  fprintf(file, "<h2>Topic Name: %s</h2>\n\n", topic);



  i = 0;
  do
  {
    fprintf(file, "<table style=\"width:100%%\" align=\"middle\"><tbody>\n");
    fprintf(file, "<tr><th>CAPTION</th><th>PHOTO-URL</th></tr>\n");

    fprintf(file, "<tr><th>%s</th>\n", captions[i]);
    fprintf(file, "<th>%s</th></tr></tbody></table>\n", URLs[i]);
    i++;
  } while (strcmp(URLs[i], ""));




}

void closeFile(FILE *file)
{
  fprintf(file, "</body></html>\n");
  fclose(file);
}
