#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

# include <stdio.h> 
# include <pwd.h> // for getpwuid()
# include <unistd.h> // for the getuid() system call
# include <sys/types.h> // for uid_t datatype
# include <string.h>
# include <signal.h>
# include <sys/wait.h>
# include "initialize.h"
# include "utilities.h"
# include "activities.h"
# include "background.h"
extern void handle_sigint(int sig);
extern void handle_sigtstp(int sig);
extern void update_foreground_pid (int signum);
# endif