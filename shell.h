#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 10

int is_builtin_command(char *command);
void print_environment(char **env);
int execute_external_command(char **av, int *last_exit_status);

#endif
