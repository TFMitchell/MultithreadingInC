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

int main()
{
  struct topicQueue TQ;
  if (init(&TQ, 512))
    printf("Error initializing topic queue.\n");



  return 0;
}
