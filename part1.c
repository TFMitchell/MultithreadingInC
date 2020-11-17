/*
* Description: Project 3 main -
*
* Author: Thomas Mitchell
*
* Date: 12-5-2020
*
* Notes:
* 1. N/A
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "topicQueue.h"
#include "string_parser.h"

#define LENGTH 512
#define MAXQUEUES 128

int main()
{
  struct topicQueue TQarry[MAXQUEUES];

  init(&TQarry[0], LENGTH, "test topic");

  struct topicEntry test = {.pubID = 5};

  enqueue(&TQarry[0], &test);



  return 0;
}
