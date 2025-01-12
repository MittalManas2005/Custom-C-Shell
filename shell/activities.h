# ifndef ACTIVITIES_H
# define ACTIVITIES_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
#include <sys/types.h>
# include <signal.h>
#include <sys/time.h>
#define MAX_PROCESSES 100 
typedef struct {
    char command_name[100];  
    pid_t pid;               
    char state[10];     
    int isForeground; 
    struct timeval start_time;     
} ProcessInfo;

extern void add_process(char *command_name, pid_t pid, char *state, int isForeground);
extern void update_process_state(pid_t pid, char *new_state);
extern void remove_process(pid_t pid) ;
extern void update_isForeground_by_pid(pid_t pid, int new_isForeground);
extern int activities_handler(char ** strings, int index);
extern char* get_command_name_by_pid(pid_t pid);
extern ProcessInfo* get_process_by_pid(pid_t pid);
extern ProcessInfo process_table[MAX_PROCESSES];
extern struct timeval get_start_time_by_pid(pid_t pid);
extern double calculate_elapsed_time(struct timeval start_time);
extern void check_proc_state(pid_t pid);
extern int is_foreground_by_pid(pid_t pid);
extern int is_pid_present(pid_t pid);
extern void terminate_all_processes();
#endif