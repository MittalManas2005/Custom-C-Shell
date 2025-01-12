#ifndef INITIALIZE_H
#define INITIALIZE_H
# define MAX_SIZE 4096

#define _XOPEN_SOURCE 700
# include <stdio.h> 
# include <pwd.h> // for getpwuid()
# include <unistd.h> // for the getuid() system call
# include <sys/types.h> // for uid_t datatype
# include <string.h>
# include <signal.h>
# include <limits.h>
#include <sys/wait.h> 
# include <termios.h>
# include "background.h" // for the signal handler
# include "signal_handler.h" 
extern void initialize();
extern void display();
extern char * bufferString;;
extern uid_t userID; 
extern char * userName; 
extern volatile sig_atomic_t sigint_received;
extern char systemName [MAX_SIZE]; 
extern char current_working_directory[MAX_SIZE];
extern char home_directory[MAX_SIZE]; 
extern char prev_directory[MAX_SIZE];
extern char relative_directory[MAX_SIZE]; 
extern void initialize();
extern int custom_atoi(const char *str);
extern char is_foreground_running;
extern void calculateRelativeDirectory(char *cwd, char *home, char *relative_directory);
volatile sig_atomic_t sigtstp_received;
extern pid_t global_foreground_pid ;
extern char *global_command ;
# endif