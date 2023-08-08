#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

/**
 * is_builtin_command - determines whether given command is built-in or not
 * @command: the given command
 * Return: 0
 */

int is_builtin_command(char *command)
{
	if (strcmp(command, "exit") == 0 || strcmp(command, "clear")
	== 0 || strcmp(command, "cd") == 0 || strcmp(command, "env") == 0)
	{
		return (1);
	}
	return (0);
}

/**
 * print_environment - prints the environment variables that are
 * passed to the program when it starts
 * @env: array of strings that contain environment variables
 * Return: nothing
 */

void print_environment(char **env)
{
	while (*env != NULL)
	{
		printf("%s\n", *env);
		env++;
	}
}

/**
 * prompt - returns to prompt. used to ignore SIGINT
 * Return: nothing
 */


void prompt()
{
	printf("\n");
	fflush(stdout);
}

/**
 * main - the main shell function!
 * @ac: argument count
 * @av: array of character pointers
 * @env: a pointer to an array of character pointers
 * Return: the exit status of the last executed command.
 */

int main(int ac, char **av, char **env)
{
	char input[MAX_COMMAND_LENGTH];
	int status;
	pid_t pid;
	/* Temporary array of character pointers for command arguments */
	char *tmp_av[MAX_ARGS + 1];
	char *token;
	int has_token;
	/* Variable to store the exit status of the last executed command */
	int last_exit_status = 0;

	while (1)
	{
		/* Make sure the prompt is displayed before reading input */
		fflush(stdout);
		signal(SIGINT, prompt);
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

		/* Tokenize the input into separate arguments and */
		/* store them in tmp_av                           */
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
			/* Set the exit status to 0 as it's a normal termination */
			last_exit_status = 0;
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
						/* Set exit status to 1 for errors */
						/* in built-in commands            */
						last_exit_status = 1;
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
				last_exit_status = 1; /* Set exit status to 1 for errors in fork */
				exit(EXIT_FAILURE);
			}
			else if (pid == 0)
			{
				/* Child process */
				if (execvp(av[0], av) == -1)
				{
					perror("execvp");
					last_exit_status = 127; /* Indicates command not found */
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
					last_exit_status = WEXITSTATUS(status);
				}
				else
				{
					last_exit_status = 1; /* Status 1 indicates an error in the child process */
				}
			}
		}
	}
	return (last_exit_status); /* Return the exit status of the last executed command */
}
