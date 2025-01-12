
# ifndef NEONATE_H
# define NEONATE_H
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <dirent.h>
# include <ctype.h>
# include <termios.h>
# include <fcntl.h>
# include <signal.h>
#include <errno.h>
# include "initialize.h"


extern int neonate_handler(char ** strings, int index) ;
#ifndef DT_DIR
#define DT_DIR 4
#endif

# endif