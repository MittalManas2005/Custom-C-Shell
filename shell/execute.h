# ifndef EXECUTE_H
# define EXECUTE_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "hop.h"
# include "reveal.h"
# include "log.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
# include <unistd.h>
#include <sys/time.h>
# include <math.h>
# include "display.h"
# include "initialize.h"
# include "seek.h"
# include "proclore.h"
# include "utilities.h"
# include "man.h"
# include "activities.h"
# include "ping.h"
# include "fg.h"
# include "bg.h"
# include "neonate.h"
# include "signal_handler.h"
extern int execute_command_pipe(char ** strings);
extern int execute(char ** strings, int index);
extern int flag_display_foreground;
extern int execute_command(char ** strings);
extern int execute_custom_command(char ** strings);
extern struct timeval start;
extern pid_t foreground_pid;
# endif