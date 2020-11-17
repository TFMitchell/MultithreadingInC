/*
 * string_parser.c
 *
 *  Created on: Nov 8, 2020
 *      Author: gguan
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GUN_SOURCE

int count_token (char* buf, const char* delim)
{
	if (buf == NULL)
	{
		printf ("Empty string\n");
		return -1;
	}

	int len = strlen (buf);
	int n_token = 0;

	for (int i = 0; i < len; i++)
	{
		if (buf[i] == delim[0])
		{
			n_token++;
		}
	}

	n_token = n_token + 2;

	return n_token;
}

command_line str_filler (char* buf, const char* delim)
{
	command_line command;

	int num_token = count_token (buf, delim);
	char* token = NULL;
	char* savePtr = NULL;

	command.num_token = num_token;
	command.command_list = malloc (sizeof(char*) * num_token);

	token = strtok_r (buf, "\n", &savePtr);
	token = strtok_r (token, delim, &savePtr);


	for (int i = 0; i < num_token - 1; i++)
	{

		command.command_list[i] = malloc (strlen (token) + 1);

		strcpy (command.command_list[i], token);

		token = strtok_r (savePtr, delim, &savePtr);
	}

	command.command_list[num_token - 1] = NULL;

	return command;
}


void free_command_line(command_line* command)
{
	for (int i = 0; i < command->num_token; i++)
	{
		free (command->command_list[i]);
	}
	free (command->command_list);
}

//int main(int argc, char const *argv[])
//{
//
//	command_line command;
//
//	size_t len = 64;
//	char* line_buf = malloc (len);
//
//	FILE* fp = fopen ("input.txt", "r");
//
//	while (getline (&line_buf, &len, fp) != -1)
//	{
//		command = str_filler (line_buf, " ");
//
//		for (int i = 0; command.command_list[i] != NULL; i++)
//		{
//			printf ("%s\n", command.command_list[i]);
//		}
//
//		free_command_line (&command);
//		memset (&command, 0, 0);
//	}
//	free (line_buf);
//
//}
/*-----------------------------Program End-----------------------------------*/
