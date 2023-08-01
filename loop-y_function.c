#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

extern char **environ; /*To access the enviroment variables */

int main()
{
    char input[MAX_COMMAND_LENGTH];
    char* args[MAX_ARGS];
    int status;
    pid_t pid;
    int argCount;
    char* token;

    char* search_dirs[] = {"/bin", "/usr/bin", NULL}; /* Add more directories if needed */

    while (1)
    {
        printf("ShellbyUWU: ");

        /* Check if Ctrl+D (EOF) is encountered */
        if (fgets(input, sizeof(input), stdin) == NULL)
	{
            printf("\n");
            break;
        }

        /* Remove the newline character from the input */
        input[strcspn(input, "\n")] = '\0';

        /* Tokenize the input into separate arguments */
        token = strtok(input, " ");
        argCount = 0;
        while (token != NULL && argCount < MAX_ARGS - 1)
	{
            args[argCount++] = token;
            token = strtok(NULL, " ");
        }
        args[argCount] = NULL;

        /* Check for the exit command */
        if (strcmp(args[0], "exit") == 0)
	{
            break;
        }

        pid = fork();
        if (pid < 0)
	{
            perror("fork");
            exit(EXIT_FAILURE);
        }
	else if (pid == 0)
	{
            /* Child process */
            char fullPath[MAX_COMMAND_LENGTH];
            int i = 0;
            while (search_dirs[i] != NULL)
	    {
                snprintf(fullPath, sizeof(fullPath), "%s/%s", search_dirs[i], args[0]);
                if (access(fullPath, X_OK) == 0)
		{
                    if (execve(fullPath, args, environ) == -1)
		    {
                        perror("execve");
                        exit(EXIT_FAILURE);
                    }
                }
                i++;
            }
            /* If the loop completes, the command was not found */
            fprintf(stderr, "%s: command not found\n", args[0]);
            exit(EXIT_FAILURE);
        }
       	else
       	{
            /* Parent process */
            wait(&status);
        }
        fflush(stdout); /* Make sure the prompt is displayed before reading input */
    }

    return 0;
}

