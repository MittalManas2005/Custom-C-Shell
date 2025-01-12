#ifndef UTILITIES_H
#define UTILITIES_H

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern int count_pipes(char **cmds);
extern void setup_pipes(int pipe_count, int pipes[][2]);
extern int isCustomCommand(char *command);
extern int redirection_handler(char ** strings, int * saved_stdin, int * saved_stdout , int * saved_stderr);
extern void restore_redirection(int saved_stdin, int saved_stdout, int saved_stderr);
#endif