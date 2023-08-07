#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

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
    int has_token;
    int exit_status = 0; /* Variable to store the exit status */

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
        has_token = 0;

        while (token != NULL && ac < MAX_ARGS)
        {
            tmp_av[ac++] = token;
            has_token = 1;
            token = strtok(NULL, " ");
        }

        /* Check if there are no arguments */
        if (!has_token)
        {
            continue;
        }

        tmp_av[ac] = NULL;

        /* Check for the exit command */
        if (strcmp(tmp_av[0], "exit") == 0)
        {
            exit_status = 0; /* Set the exit status to 0 as it's a normal termination */
            break; /* Exit the while loop and terminate the shell */
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
                        exit_status = 1; /* Set exit status to 1 for errors in built-in commands */
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
            /* Check if the file exists before executing it */
            if (access(tmp_av[0], F_OK) == 0)
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
                    exit_status = 1; /* Set exit status to 1 for errors in fork */
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0)
                {
                    /* Child process */
                    if (execve(av[0], av, env) == -1)
                    {
                        fprintf(stderr, "./hsh: 1: %s: %s\n", av[0], strerror(errno)); /* Print the error message with the command name */
                        exit_status = 127; /* Indicates command not found */
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
                        exit_status = WEXITSTATUS(status);
                    }
                    else
                    {
                        exit_status = 1; /* Status 1 indicates an error in the child process */
                    }
                }
            }
            else
            {
                /* File not found */
                fprintf(stderr, "./hsh: 1: %s: not found\n", tmp_av[0]);
                exit_status = 127; /* Set exit status to 127 for command not found */
            }
	}

	if (WIFEXITED(status))
        {
            exit_status = WEXITSTATUS(status);
        }
        else
        {
            exit_status = 1; 

	}
    }

    return exit_status; /* Return the exit status of the last executed command */
}
