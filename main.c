#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define COMMAND_LENGTH 100
#define COMMAND_PATH 1000

int main()
{

	char command[COMMAND_LENGTH];
	char commandPath[COMMAND_PATH];
	int pid;
	int status;
	char *path = getenv("PATH");
	char pathCopy[strlen(path) + 1];
	strcpy(pathCopy, path);

	char *token;
	char *params[COMMAND_LENGTH];
	int paramsCounter = 0;

	while (1)
	{
		printf("shell:~$");
		fgets(command, COMMAND_LENGTH, stdin);

		command[strlen(command) - 1] = '\0'; // remove newline

		if (strcmp(command, "leave") == 0)
		{

			printf("As you wish, goodbye!\n");
			exit(0);
		}

		token = strtok(command, " ");
		
		while (token != NULL)
		{
			params[paramsCounter] = token;
			paramsCounter++;
			token = strtok(NULL, " ");
		}
		params[paramsCounter] = NULL;

		if ((pid = fork()) == 0)
		{

			token = strtok(pathCopy, ":");

			while (token != NULL) // here we search for the command in the all paths
			{

				commandPath[0] = 0;

				strcat(commandPath, token);
				strcat(commandPath, "/");		// add / to the end of the path
				strcat(commandPath, params[0]); // add the command after / so it will look like usr/bin/command

				execv(commandPath, params);

				token = strtok(NULL, ":");
			}

			printf("Command not found, try again\n");
			exit(0);
		}
		else
		{
			wait(&status);
			
		}

		memset(params, 0, sizeof(params));
		paramsCounter = 0;
	}

	return 0;
}
