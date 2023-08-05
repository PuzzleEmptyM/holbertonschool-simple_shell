#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10


int is_builtin_command(char *command)
{
    if (strcmp(command, "exit") == 0 || strcmp(command, "cd") == 0)
    {
        return 1;
    }
    return 0;

}

int main(int ac, char **av, char **env)
{
	char input[MAX_COMMAND_LENGTH];
	char *args[MAX_ARGS];
	int status;
	pid_t pid;
	char *command; 
	char *token;

	while (1)
	{
	/* printf("$ "); */
        fflush(stdout); /* Make sure the prompt is displayed before reading input */

       	/* Check if Ctrl+D (EOF) is encountered */
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            /*printf("\n");*/
            break;
        }

        /* Remove the newline character from the input */
        input[strcspn(input, "\n")] = '\0';

        /* Skip processing if input is empty */
        if (strlen(input) == 0)
        {
                continue;
        }

        /* Tokenize the input into separate arguments */
        token = strtok(input, " ");
        ac = 0;
        while (token != NULL && ac < MAX_ARGS - 1)
        {
            args[ac++] = token;
            token = strtok(NULL, " ");
        }
        args[ac] = NULL;

        /* Check for the exit command */
        if (strcmp(args[0], "exit") == 0)
        {
            exit(EXIT_SUCCESS);
        }

        /* Check for built-in commands */
        if (is_builtin_command(args[0]))
	{
    		if (strcmp(command, "exit") == 0)
    		{
       	 	exit(EXIT_SUCCESS);
    		}
    		else if (strcmp(command, "cd") == 0)
    		{
			if (av[1] != NULL)
       			{
				if (chdir(av[1]) != 0)
				{
				perror("cd");
            			}
        		}
    		}
	}
	else
	{
		fprintf(stderr, "%s: command not found\n", command);
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
            if (execvp(args[0], args) == -1)
            {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            /* Parent process */
            wait(&status);
        }
    }

    return 0;
}

