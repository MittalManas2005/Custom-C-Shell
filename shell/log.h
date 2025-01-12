#ifndef LOG_H
#define LOG_H
# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
#include <unistd.h>
# include <string.h>
# include "initialize.h"
# include "input.h"
extern FILE *  log_file_descriptor;
extern void log_initialize();
extern void enterCommand(char * str); // enters teh commadn in the log file
extern void log_handler(char ** strings, int i);
# endif