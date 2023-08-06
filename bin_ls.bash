#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 10
#define MAX_PATH_LENGTH 100

int parse_command(char *command, char *arguments[]) {
    int arg_count = 0;
    char *token = strtok(command, " \t\n");

    while (token != NULL) {
        arguments[arg_count++] = token;
        token = strtok(NULL, " \t\n");

        if (arg_count >= MAX_ARGUMENTS) {
            break;
        }
    }

    arguments[arg_count] = NULL;
    return arg_count;
}

int command_exists(char *command, char *path) {
    char full_path[MAX_PATH_LENGTH];
    char *token = strtok(path, ":");

    while (token != NULL) {
        snprintf(full_path, MAX_PATH_LENGTH, "%s/%s", token, command);
        if (access(full_path, X_OK) == 0) {
            return 1;
        }
        token = strtok(NULL, ":");
    }

    return 0;
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char *arguments[MAX_ARGUMENTS + 1];

    char *path = getenv("PATH");
    if (path == NULL) {
        fprintf(stderr, "PATH environment variable not found.\n");
        return 1;
    }

    while (1) {
        printf("shell> ");

        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = '\0';

        int arg_count = parse_command(command, arguments);

        if (command_exists(arguments[0], path)) {
            pid_t pid = fork();

            if (pid == -1) {
                perror("fork");
                return 1;
            }

            if (pid == 0) {
                execvp(arguments[0], arguments);
                perror("execvp");
                exit(1);
            } else {
                int status;
                wait(&status);
            }
        } else {
            fprintf(stderr, "Command not found: %s\n", arguments[0]);
        }
    }

    return 0;
}

