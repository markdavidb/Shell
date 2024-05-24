#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define COMMAND_LENGTH 100
#define COMMAND_PATH 1000
#define RED "\033[1;31m"
#define DARK_BLUE "\033[1;34m"
#define RESET "\033[0m"

int main()
{
    char command[COMMAND_LENGTH];
    char commandPath[COMMAND_PATH];

    int pid;
    int status;

    char *path = getenv("PATH");
    if (path == NULL)
    {
        printf("PATH not found\n");
        exit(1);
    }

    char pathCopy[strlen(path) + 1];
    strcpy(pathCopy, path);

    char *token;
    char *params[COMMAND_LENGTH];
    int paramsCounter = 0;

    while (1)
    {
        printf(DARK_BLUE "Shell$" RESET " ");
        fgets(command, COMMAND_LENGTH, stdin);

        command[strlen(command) - 1] = '\0'; // remove newline

        if (strcmp(command, "leave") == 0)
        {
            printf("As you wish, goodbye!\n");
            exit(0);
        }

        token = strtok(command, " ");
        
        // Reset paramsCounter for each command
        paramsCounter = 0;

        while (token != NULL) // command and its arguments
        {
            params[paramsCounter] = token;
            paramsCounter++;
            token = strtok(NULL, " ");
        }
        params[paramsCounter] = NULL;

        // Handle cd command
        if (strcmp(params[0], "cd") == 0)
        {
            if (params[1] == NULL)
            {
                fprintf(stderr, "cd: is missing an argument\n");
            }
            else
            {
                if (chdir(params[1]) != 0)
                {
                    perror("cd");
                }
            }
            continue; // Skip the child fork and start the next iteration
        }

        if ((pid = fork()) == 0)
        {
            // Child process
            if (params[0][0] == '/' || strncmp(params[0], "./", 2) == 0 || strncmp(params[0], "../", 3) == 0)
            {
                // If the command is an absolute or relative path, execute it directly
                execv(params[0], params);
            }
            else
            {
                // Search for the command in all paths
                token = strtok(pathCopy, ":");

                while (token != NULL)
                {
                    commandPath[0] = 0;

                    strcat(commandPath, token);
                    strcat(commandPath, "/");
                    strcat(commandPath, params[0]);

                    execv(commandPath, params);

                    token = strtok(NULL, ":");
                }

                printf(RED "Command not found, try again" RESET "\n");
            }

            exit(0);
        }
        else
        {
            // Parent process
            wait(&status);
            memset(params, 0, sizeof(params));
        }
    }

    return 0;
}
