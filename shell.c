#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

extern char **environ; /* To access the environment variables */

void handle_builtin_command(char *command, char **args)
{
    if (strcmp(command, "exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(command, "cd") == 0)
    {
        if (args[1] != NULL)
        {
            if (chdir(args[1]) != 0)
            {
                perror("cd");
            }
        }
    }
    else
    {
        fprintf(stderr, "%s: command not found\n", command);
    }
}

int is_builtin_command(char *command)
{
    if (strcmp(command, "exit") == 0 || strcmp(command, "cd") == 0)
    {
        return 1;
    }
    return 0;
}

int main()
{
    char input[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];
    int status;
    pid_t pid;
    int argCount;
    char *token;

    while (1)
    {
        printf("ShellbyUWU: ");
        fflush(stdout); /* Make sure the prompt is displayed before reading input */

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
            exit(EXIT_SUCCESS);
        }

        /* Check for built-in commands */
        if (is_builtin_command(args[0]))
        {
            handle_builtin_command(args[0], args);
            continue; /* Continue to the next iteration of the loop */
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
