#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

int main() {
    char input[MAX_COMMAND_LENGTH];
    char* args[MAX_ARGS];
    int status;
    pid_t pid;
    char* token;

    while (1) {
        printf("SimpleShell> ");
        fflush(stdout); /* Make sure the prompt is displayed before reading input */
        fgets(input, sizeof(input), stdin);

        /* Remove the newline character from the input */
        input[strcspn(input, "\n")] = '\0';

        /* Tokenize the input into separate arguments */
        token = strtok(input, " ");
        int argCount = 0;
        while (token != NULL && argCount < MAX_ARGS - 1) {
            args[argCount++] = token;
            token = strtok(NULL, " ");
        }
        args[argCount] = NULL; /* The last argument must be NULL for execvp */

        /* Check for the exit command */
        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            /* Child process */
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else {
            /* Parent process */
            wait(&status);

            /* Print the command's output (you can redirect to file or another stream if desired) */
            printf("\nCommand Output:\n");
            fflush(stdout);
            system(input); /* Executes the same command using the system() function */

            /* Wait for the user to press Enter before displaying the prompt again */
            printf("\nPress Enter to continue...");
            fflush(stdout);
            getchar();
            printf("\n");
        }
    }

    return 0;
}

