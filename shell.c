#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

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
    char *tmp_av[MAX_ARGS + 1]; /* Temporary array of character pointers for command arguments */
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

        /* Tokenize the input into separate arguments and store them in tmp_av */
        token = strtok(input, " ");
        ac = 0;
        while (token != NULL && ac < MAX_ARGS)
        {
            tmp_av[ac++] = token;
            token = strtok(NULL, " ");
        }

        /* Check if there are no arguments */
        if (ac == 0)
        {
            continue;
        }

        tmp_av[ac] = NULL;

        /* Check for the exit command */
        if (strcmp(tmp_av[0], "exit") == 0)
        {
            exit(EXIT_SUCCESS);
        }

        /* Check for built-in commands */
        if (is_builtin_command(tmp_av[0]))
        {
            if (strcmp(tmp_av[0], "cd") == 0)
            {
                if (ac > 1)
                {
                    if (chdir(tmp_av[1]) != 0)
                    {
                        perror("cd");
                    }
                }
            }
            else if (strcmp(tmp_av[0], "env") == 0)
            {
                print_environment(env);
            }
        }
        else
        {
            /* Copy the arguments from tmp_av to av before executing the command */
            int i;
            for (i = 0; i < ac; i++)
            {
                av[i] = tmp_av[i];
            }
            av[ac] = NULL;

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

                /* Check if the process terminated normally or with an error */
                if (WIFEXITED(status))
                {
                    /* Get the exit status of the child process */
                    int exit_status = WEXITSTATUS(status);
                    printf("status[%d]\n", exit_status);
                }
                else
                {
                    printf("status[1]\n"); /* Status 1 indicates an error in the child process */
                }
            }
        }
    }

    return 0;
}
