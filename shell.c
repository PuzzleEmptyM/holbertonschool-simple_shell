#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100

int is_builtin_command(char *command)
{
    if (strcmp(command, "exit") == 0 || strcmp(command, "cd") == 0 || strcmp(command, "env") == 0)
    {
        return 1;
    }
    return 0;
}

void print_environment(char **env)
{
    while (*env != NULL)
    {
        printf("%s\n", *env);
        env++;
    }
}

int main(int ac, char **av, char **env)
{
    char input[MAX_COMMAND_LENGTH];
    int status;
    pid_t pid;
    char *token;

    while (1)
    {
        fflush(stdout); /* Make sure the prompt is displayed before reading input */

        /* Check if Ctrl+D (EOF) is encountered */
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        /* Remove the newline character from the input */
        input[strcspn(input, "\n")] = '\0';

        /* Skip processing if input is empty */
        if (strlen(input) == 0)
        {
            continue;
        }

        /* Tokenize the input into separate arguments and store them in av */
        token = strtok(input, " ");
        ac = 0;
        while (token != NULL && ac < MAX_COMMAND_LENGTH - 1)
        {
            av[ac++] = token;
            token = strtok(NULL, " ");
        }
        av[ac] = NULL;

        /* Check for the exit command */
        if (strcmp(av[0], "exit") == 0)
        {
            exit(EXIT_SUCCESS);
        }

        /* Check for built-in commands */
        if (is_builtin_command(av[0]))
        {
            if (strcmp(av[0], "cd") == 0)
            {
                if (ac > 1)
                {
                    if (chdir(av[1]) != 0)
                    {
                        perror("cd");
                    }
                }
            }
            else if (strcmp(av[0], "env") == 0)
            {
                print_environment(env);
            }
        }
        else
        {
            pid = fork();
            if (pid < 0)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                /* Child process */
                if (execvp(av[0], av) == -1)
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
    }

    return 0;
}
