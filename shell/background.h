#ifndef BACKGROUND_H
#define BACKGROUND_H
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <signal.h>
# include <errno.h>
# include "display.h"
# include "initialize.h"
# include "log.h"
# include "execute.h"
# include <fcntl.h>
# include <sys/time.h>
# include "utilities.h"
# include <termios.h>
 #define MAX_MESSAGES 100
extern int executebackground(char ** strings, int index);
extern int message_count;

extern void handle_sigchld(int signum);
void handle_sigint(int sig);
void handle_sigtstp(int sig);
extern char *message_queue[100];
#endif